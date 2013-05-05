<?php

class PiInfo {

	private $path;
	private $inquiries;
	private $content;

	function __construct() {
		$this->path = new PiInquiryPath();
		$this->inquiries = array();
		$this->content = array();
	}

	function getCurrentPath() {
		return $this->path;
	}

	function getInquiryInfo($inquiry_id) {
		if (!isset($this->inquiries[$inquiry_id])) {
			$this->inquiries[$inquiry_id] = new PiInquiryInfo($inquiry_id);
		}

		return $this->inquiries[$inquiry_id];
	}

	function getContentManager($inquiry_id) {
		if (!isset($this->content[$inquiry_id])) {
			$this->content[$inquiry_id] = new PiInquiryContent($this->getInquiryInfo($inquiry_id));
		}

		return $this->content[$inquiry_id];
	}

}

class PiInquiryPath {

	private $inquiry_nid;
	private $phase_nid;
	private $activity_nid;

	function __construct() {
		$path = get_inquiry_details_from_current_path();
		$this->activity_nid = $path->activity_id;
		$this->phase_nid = $path->phase_id;
		$this->inquiry_nid = $path->inquiry_id;
	}

}

class PiInquiryInfo {

	private $inquiry;
	private $phases;
	private $phases_activities;
	private $activity_phase;
	private $activities;
	private $use_groups;
	private $collaboration_level;

	function __construct($inquiry_id) {
		$this->inquiry = node_load($inquiry_id);

		$this->phases = array();
		$this->activity_phase = array();
		$this->phases_activities = array();
		$this->activities = array();

		$this->collaboration_level = 'individual';

		$phase_nids = pi_phase_load_nids_for_inquiry($this->inquiry['nid']);
		foreach ($phase_nids as $phase_nid) {
			$phase = node_load($phase_nid);
			$this->phases[$phase_nid] = $phase;

			if ($phase->collaboration === 'group') {
				$this->use_groups = TRUE;
				if ($this->collaboration_level === 'individual') {
					$this->collaboration_level = 'group';
				}
			} else if ($phase->collaboration === 'all') {
				$this->collaboration_level = 'all';
			}

			$this->phases_activities[$phase_nid] = array();
			$activity_nids = pi_get_activities_for_phase($phase_nid);
			foreach ($activity_nids as $activity_nid) {
				$activity = node_load($activity_nid);
				$this->activities[$activity_nid] = $activity;
				$this->phases_activities[$phase_nid][$activity_nid] = $activity;
				$this->activity_phase[$activity_nid] = $phase_nid;
			}
		}
	}

	public function getSubgroup($uid) {
		return pi_inquiry_groupings_get_user_subgroup($this->inquiry, $uid);
	}

	public function getActivity($activity_nid) {
		return $this->activities[$activity_nid];
	}

	public function getPhaseActor($phase_nid, $uid) {
		$phase = $this->getPhase($phase_nid);

		$group = $this->getSubgroup($uid);
		$inquiry_actor = $this->inquiry->actor_gid;

		switch ($phase->collaboration) {
			case 'individual':
				$actor = $uid;
				break;
			case 'group':
				$actor = $group;
				break;
			default:
				$actor = $inquiry_actor;
				break;
		}
		return array(
				'collaboration' => $phase->collaboration,
				'actor' => $actor,
		);
	}

	public function getActivityActor($activity_nid, $uid) {
		return $this->getPhaseActor($this->activity_phase[$activity_nid], $uid);
	}

	public function getActivityCollaboration($activity_nid) {
		return $this->phases[$this->activity_phase[$activity_nid]]->collaboration;
	}

}

class PiInquiryContent {

	private $inquiry_info;

	public function __construct($inquiry_info) {
		$this->inquiry_info = $inquiry_info;
	}

	public function getContent($activity_nid, $uid, $try_to_create) {

		$activity = $this->inquiry_info->getActivity($activity_nid);
		if ($activity->referenced_node) {
			return $activity->referenced_node;
		}

		$actor_data = $this->inquiry_info->getActivityActor($activity_nid, $uid);
		$query = "SELECT nid FROM {pi_activity_node} WHERE activity_id=%d AND collaboration='%s' AND actor=%d";


		$row = db_fetch_array(db_query($query, $actor_data['collaboration'], $actor_data['actor']));
		if ($row) {
			$nid = $row['nid'];
			return $nid;
		} else if ($try_to_create) {
			$new_node = $this->createContentOnVisit($activity_nid, $uid);
			if ($new_node) {
				return $new_node->nid;
			}
		}

		return FALSE;
	}

	private function createContentOnVisit($activity_nid, $uid) {
		$activity = $this->inquiry_info->getActivity($activity_nid);

		$types = array(
				'pi_methodology',
				'pi_data_spreadsheet',
				'pi_sort_data',
				'pi_sort_key_answers',
				'pi_sort_key_questions',
				'pi_sort_result_presentations',
		);
		if (in_array($activity->activity_type, $types)) {
			$node = new stdClass();
			$node->uid = $uid;
			$node->type = $activity->activity_type;
			$node->body = '';
			node_save($node);

			$this->linkContentWithActivity($node, $activity_nid, $uid);
		}
	}

	public function linkContentWithActivity($content_node, $activity_nid, $uid) {
		$actor_data = $this->inquiry_info->getActivityActor($activity_nid, $uid);

		$activity_node = array(
				'activity_id' => $activity_nid,
				'nid' => $content_node->nid,
				'collaboration' => $actor_data['collaboration'],
				'actor' => $actor_data['actor'],
		);

		drupal_write_record('pi_activity_node', $activity_node);

		$activity = $this->inquiry_info->getActivity($activity_nid);
		if (!$activity->script_activity) {
			$query = "UPDATE pi_activity SET referenced_node=%d WHERE nid=%d";
			db_query($query, $content_node->nid, $activity_nid);
		}
	}

	public function getChildrenContentForContentNid($content_nid) {
		return $this->getChildrenContentForContentNode(node_load($content_nid));
	}

	public function getChildrenContentForContentNode($content_node) {
		$activity_nid = $this->getActivityForContent($content_node->nid);
		$actor_data = $this->inquiry_info->getActivityActor($activity_nid, $content_node->uid);
		return $this->getChildrenContentForActivityAndActor($activity_nid, $actor_data);
	}

	private function getChildrenContentForActivityAndActor($activity_nid, $actor_data) {
		$query = "SELECT {pi_activity_node}.nid FROM {node} LEFT JOIN {pi_activity} ON {node}.vid={pi_activity}.vid "
						. "LEFT JOIN {pi_activity_node} ON {pi_activity}.nid={pi_activity_node}.activity_id "
						. "WHERE {pi_activity}.parent_activity=%d AND {pi_activity_node}.collaboration='%s' AND {pi_activity_node}.actor=%d";

		$nids = array();
		$result = db_query($query, $activity_nid, $actor_data['collaboration'], $actor_data['actor_nid']);
		while ($row = db_fetch_array($result)) {
			$nids[] = $row['nid'];
		}

		return $nids;
	}

	public function getContentFromFor($activity_nid, $using_activity_id, $using_uid) {
		$using_actor_data = $this->inquiry_info->getActivityActor($using_activity_id, $using_uid);
		return $this->getSharedContentForActivityAndActorData($activity_nid, $using_actor_data);
	}

	public function getSharedContent($activity_nid, $uid) {
		$viewer_actor_data = $this->inquiry_info->getActivityActor($activity_nid, $uid);
		return $this->getSharedContentForActivityAndActorData($activity_nid, $viewer_actor_data);
	}

	public function getContributionContent($contribution_node, $activity_nid) {
		$actor_data = array('collaboration' => $contribution_node->collaboration, 'actor' => $contribution_node->actor);
		return $this->getSharedContentForActivityAndActorData($activity_nid, $actor_data);
	}

	private function getActivityForContent($content_nid) {
		$query = "SELECT `activity_id` FROM {pi_activity_node} WHERE `nid` = %d";
		$result = db_fetch_object(db_query($query, $content_nid));
		if ($result != false) {
			if (isset($result->activity_id)) {
				return $result->activity_id;
			}
		}
		return FALSE;
	}

	private function getSharedContentForActivityAndActorData($activity_nid, $viewer_actor_data) {
		$activity_collaboration = $this->inquiry_info->getActivityCollaboration($activity_nid);
		$viewer_collaboration = $viewer_actor_data['collaboration'];

		if ($viewer_collaboration === $activity_collaboration) {
			$mode = 'single_actor';
			$single_actor = $viewer_actor_data['actor'];
		} else if ($activity_collaboration === 'all') {
			$mode = 'single_actor';
			$single_actor = $this->inquiry_info->getInquiryActorGid();
		} else if ($viewer_collaboration === 'individual' && $activity_collaboration === 'group') {
			$mode = 'single_actor';
			$single_actor = $this->inquiry_info->getSubgroup($viewer_actor_data['actor']);
		} else if ($viewer_collaboration === 'group' && $activity_collaboration === 'individual') {
			$mode = 'shared_within_group';
			$group = $this->inquiry_info->getSubgroup($viewer_actor_data['actor']);
			$group_members = nquire_group_get_member_uids($group, TRUE);
		} else if ($viewer_collaboration === 'all' && $activity_collaboration === 'individual') {
			$mode = 'shared_with_inquiry';
			$all_share_requisite = 'individual_all';
		} else if ($viewer_collaboration === 'all' && $activity_collaboration === 'group') {
			$mode = 'shared_with_inquiry';
			$all_share_requisite = 'group_all';
		}


		$sharing_actors = array();
		switch ($mode) {
			case 'single_actor';
				$sharing_actors[] = $single_actor;
				break;
			case 'shared_within_group';
				$share_query = "SELECT share FROM pi_activity_share WHERE activity_id=%d AND actor=%d";
				foreach ($group_members as $uid) {
					$row = db_fetch_array(db_query($share_query, $activity_nid, $uid));
					$share = !$row || $row['share'] !== 'individual_no';
					if ($share) {
						$sharing_actors[] = $uid;
					}
				}
				break;
			case 'shared_with_inquiry':
				$share_query = "SELECT actor FROM pi_activity_share WHERE activity_id=%d AND share='%s'";
				$result = db_query($share_query, $activity_nid, $all_share_requisite);
				while ($row = db_fetch_array($result)) {
					$sharing_actors[] = $row['actor'];
				}
		}

		$nids = array();

		$query = "SELECT filtered_pi_activity_node.nid FROM "
						. "(SELECT * FROM pi_activity_node WHERE actor=%d) AS filtered_pi_activity_node "
						. "INNER JOIN (SELECT node.nid as activity_node_nid FROM node LEFT JOIN pi_activity ON node.vid=pi_activity.vid WHERE pi_activity.nid=%d) AS activity_node_table "
						. "ON filtered_pi_activity_node.activity_id=activity_node_table.activity_node_nid ";
		foreach ($sharing_actors as $actor) {
			$result = db_query($query, $actor, $activity_nid);
			while ($row = db_fetch_array($result)) {
				$nids[] = $row['nid'];
			}
		}

		return $nids;
	}

}

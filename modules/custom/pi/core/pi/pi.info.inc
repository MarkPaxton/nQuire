<?php

class PiInfo {
	private $path;
	private $inquiries;
	
	function __construct() {
		$this->path = new PiInquiryPath();
		$this->inquiries = array();
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
			foreach($activity_nids as $activity_nid) {
				$activity = node_load($activity_nid);
				$this->activities[$activity_nid] = $activity;
				$this->phases_activities[$phase_nid][$activity_nid] = $activity;
				$this->activity_phase[$activity_nid] =  $phase_nid;
			}
		}		
	}	
	
	
}

class PiInquiryContent {
	private $inquiry_info;
	
	public function __construct($inquiry_info) {
		$this->inquiry_info = $inquiry_info;
	}
	
	public function getContent($activity_nid, $uid) {
		
	}
	
	public function getContentFrom($activity_nid, $using_activity_id, $using_uid) {
		
	}
	
	public function getSharedContent($activity_nid) {
		
	}
	
	public function getContributionContent($activity_nid) {
		
	}
}

/*
	private $current_path;
	private $current_stage;
	private $all_stages_structure;
	private $access_values;
	private $uid;
	private $inquiry;
	private $use_groups;
	private $subgroup;
	private $subgroup_node;
	private $roles;
	private $phases;
	private $activities;
	private $share_policy;

	function __construct() {
		$this->access_values = array(
				FALSE => 0,
				'noaccess' => 0,
				'view' => 1,
				'comment' => 2,
				'edit' => 3,
		);

		$this->load_current_path(get_inquiry_details_from_current_path());
	}
	
	public function set_current_inquiry($inquiry_nid) {
		$path->phase_id = NULL;
		$path->inquiry_id = $inquiry_nid;
		$path->activity_id = NULL;
		
		$this->load_current_path($path);
	}
	
	public function set_current_activity($activity_nid) {
		$node = node_load($activity_nid);
		$phase = node_load($node->phase_nid);

		$path = new stdClass();
		$path->phase_id = $node->phase_nid;
		$path->inquiry_id = $phase->inquiry_nid;
		$path->activity_id = $node->nid;

		$this->load_current_path($path);
	}

	private function load_current_path($path) {
		$this->current_path = $path;

		if ($this->current_path->inquiry_id) {
			global $user;
			$this->uid = $user->uid;

			$this->inquiry = array();
			$this->inquiry['nid'] = $this->current_path->inquiry_id;
			$this->inquiry['node'] = node_load($this->current_path->inquiry_id);
			$this->inquiry['access'] = $this->inquiry['node']->uid == $user->uid || nquire_group_get_user_status_in_group($this->inquiry['node']->actor_gid, $this->uid) === 'member';
			$this->inquiry['roles'] = nquire_commons_decode_data($this->inquiry['node']->roles);

			$this->subgroup = pi_inquiry_groupings_get_user_subgroup($this->inquiry['node'], $this->uid);
			$this->subgroup_node = node_load($this->subgroup);

			$this->roles = array(
					'inquiry' => nquire_group_get_user_roles($this->inquiry['node']->actor_gid, $this->uid),
					'subgroup' => $this->subgroup ? nquire_group_get_user_roles($this->subgroup, $this->uid) : FALSE,
			);


			$this->load_stage();
			$this->load_phases();
			$this->load_activities();
		}
	}

	private function load_stage() {
		$query = "SELECT stage_id FROM {pi_stage_current} WHERE inquiry_id=%d";
		$row = db_fetch_array(db_query($query, $this->inquiry['nid']));
		if ($row) {
			$this->current_stage = node_load($row['stage_id']);
		} else {
			$query = "SELECT {node}.nid FROM {node} LEFT JOIN {pi_stage} ON {node.vid}={pi_stage}.vid WHERE {pi_stage}.inquiry_nid=%d ORDER BY {pi_stage}.weight ASC LIMIT 1";
			$row = db_fetch_array(db_query($query, $this->inquiry['nid']));
			if ($row) {
				$this->current_stage = node_load($row['nid']);
			}
		}
	}

	private function load_phases() {
		$this->phases = array();
		$this->use_groups = FALSE;

		$phase_nids = pi_phase_load_nids_for_inquiry($this->inquiry['nid']);
		foreach ($phase_nids as $nid) {
			$access = 'edit';
			if ($this->inquiry['access'] && $this->current_stage && isset($this->current_stage->phases[$nid])) {
				$access = $this->current_stage->phases[$nid] === 'hidden' ? FALSE : $this->current_stage->phases[$nid];
			}

			$item = array();
			$item['node'] = node_load($nid);
			$item['access'] = $access;
			$item['activities'] = array();
			$this->phases[$nid] = $item;

			if ($item['node']->collaboration === 'group') {
				$this->use_groups = TRUE;
			}
		}
	}

	private function load_activities() {
		$this->activities = array();

		foreach ($this->phases as $phase_nid => $data) {
			$activity_nids = pi_get_activities_for_phase($phase_nid);
			foreach ($activity_nids as $activity_nid) {
				$item = array();
				$item['node'] = node_load($activity_nid);
				$item['collaboration'] = $data['node']->collaboration;
				$item['phase_nid'] = $data['node']->nid;
				$item['script_activity'] = TRUE;

				if ($this->inquiry['access']) {
					switch ($item['collaboration']) {
						case 'individual':
							$item['access'] = $data['access'];
							break;
						case 'group':
							$item['access'] = $this->subgroup ? $this->max_access_for_user_roles($this->roles['subgroup'], $activity_nid, $data['access']) : FALSE;
							break;
						default:
							$item['access'] = $this->max_access_for_user_roles($this->roles['inquiry'], $activity_nid, $data['access']);
							break;
					}
				} else {
					$item['access'] = FALSE;
				}
				$this->phases[$phase_nid]['activities'][$activity_nid] = $item;
				$this->activities[$activity_nid] = $item;
			}
		}
	}

	private function max_access_for_user_roles($user_roles, $activity_nid, $base_access = 'edit') {
		if (count($this->inquiry['roles']) === 0) {
			return $base_access;
		} else {
			$access = FALSE;
			foreach ($this->inquiry['roles'] as $role) {
				if (in_array($role['id'], $user_roles)) {
					if (isset($role['activities'][$activity_nid])) {
						$value = $role['activities'][$activity_nid];
						if ($this->access_values[$value] > $this->access_values[$access]) {
							$access = $value;
						}
					}
				}
			}

			if ($this->access_values[$access] > $this->access_values[$base_access]) {
				$access = $base_access;
			}

			return $access;
		}
	}

	function get_inquiry_node() {
		return $this->inquiry['node'];
	}

	function get_inquiry_nid() {
		return $this->inquiry['nid'];
	}

	function has_inquiry_access() {
		return $this->inquiry['access'];
	}

	function has_phase_access($phase_nid) {
		return $this->phases[$phase_nid]['access'];
	}

	function get_phases() {
		return $this->phases;
	}

	function get_phase($nid) {
		return $this->phases[$nid];
	}

	function get_activity($nid) {
		if (!isset($this->activities[$nid])) {
			$parent_nid = pi_activity_get_parent_activity($nid);
			if (isset($this->activities[$parent_nid])) {
				$parent_data = $this->activities[$parent_nid];
				$activity_node = node_load($nid);
				$child_data = array(
						'node' => $activity_node,
						'collaboration' => $parent_data['collaboration'],
						'phase_nid' => $parent_data['phase_nid'],
						'access' => $parent_data['access'],
						'script_activity' => $activity_node->script_activity,
				);
				$this->activities[$nid] = $child_data;
			}
		}

		return $this->activities[$nid];
	}

	function get_use_groups() {
		return $this->use_groups;
	}

	function get_subgroup() {
		return $this->subgroup;
	}

	function get_uid() {
		return $this->uid;
	}

	function get_current_phase_nid() {
		return $this->current_path->phase_id;
	}

	function get_current_activity_nid() {
		return $this->current_path->activity_id;
	}

	function get_activity_by_type($type) {
		foreach ($this->phases as $phase_data) {
			foreach ($phase_data['activities'] as $activity_data) {
				if ($activity_data['node']->activity_type === $type) {
					return $activity_data;
				}
			}
		}

		return NULL;
	}

	function get_activities_by_type($type) {
		$activities = array();
		if ($this->phases) {
			foreach ($this->phases as $phase_data) {
				foreach ($phase_data['activities'] as $activity_data) {
					if ($activity_data['node']->activity_type === $type) {
						$activities[] = $activity_data;
					}
				}
			}
		}

		return $activities;
	}

	function get_user_share_policy() {
		if (!isset($this->share_policy)) {
			$this->share_policy = array('structure' => array(), 'values' => array());
			$query = "SELECT share FROM pi_activity_share WHERE activity_id=%d AND actor=%d";

			foreach ($this->phases as $pnid => $phase_data) {
				$phase_policy = array(
						'id' => $pnid,
						'title' => check_plain($phase_data['node']->title) . ' <small>(' . $phase_data['node']->collaboration . ')</small>',
						'activities' => array(),
				);

				$valid = TRUE;
				switch ($phase_data['node']->collaboration) {
					case 'individual':
						$default = $this->use_groups ? 'individual_group' : 'individual_no';
						$actor = $this->uid;
						$not_editable = FALSE;
						break;
					case 'group':
						if ($this->subgroup) {
							$default = 'group_no';
							$actor = $this->subgroup;
							$not_editable = !$this->user_is_subgroup_owner();
						} else {
							$valid = FALSE;
						}
						break;
					default:
						$default = 'all_no';
						$actor = $this->inquiry['node']->actor_gid;
						$not_editable = !$this->user_is_inquiry_owner();
						break;
				}

				foreach ($phase_data['activities'] as $anid => $activity_data) {
					if ($valid) {
						$row = db_fetch_array(db_query($query, $anid, $actor));
						$this->share_policy['values'][$anid] = $row ? $row['share'] : $default;
					}

					$activity_policy = array(
							'id' => $anid,
							'title' => check_plain($activity_data['node']->title),
							'attributes' => array(
									'not_editable' => $not_editable || !$valid,
									'collaboration' => $phase_data['node']->collaboration,
									'use_groups' => $this->use_groups ? '1' : '0',
							),
					);

					$phase_policy['activities'][] = $activity_policy;
				}

				$this->share_policy['structure'][] = $phase_policy;
			}
		}

		return $this->share_policy;
	}

	function user_is_subgroup_owner() {
		return $this->subgroup_node && $this->subgroup_node->uid === $this->uid;
	}

	function user_is_inquiry_owner() {
		return $this->inquiry && $this->inquiry['node']->uid === $this->uid;
	}

	function get_all_stages_structure() {
		if (!isset($this->all_stages_structure)) {
			$this->all_stages_structure = array('structure' => array(), 'values' => array());
			$query = "SELECT {node}.nid FROM {node} LEFT JOIN {pi_stage} ON {node}.vid={pi_stage}.vid WHERE inquiry_nid=%d";
			$result = db_query($query, $this->inquiry['nid']);
			while ($row = db_fetch_array($result)) {
				$stage = node_load($row['nid']);
				$this->all_stages_structure['values'][] = array(
						'id' => $stage->nid,
						'title' => check_plain($stage->title),
						'values' => $stage->phases,
				);
			}

			foreach ($this->phases as $pnid => $phase_data) {
				$phase_policy = array(
						'id' => $pnid,
						'title' => check_plain($phase_data['node']->title),
						'activities' => array(),
				);

				foreach ($phase_data['activities'] as $anid => $activity_data) {
					$activity_policy = array(
							'id' => $anid,
							'title' => check_plain($activity_data['node']->title),
					);

					$phase_policy['activities'][] = $activity_policy;
				}
				$this->all_stages_structure['structure'][] = $phase_policy;
			}
		}

		return $this->all_stages_structure;
	}

	function get_current_stage() {
		return $this->current_stage;
	}

}
*/
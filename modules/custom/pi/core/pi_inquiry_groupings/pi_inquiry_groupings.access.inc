<?php

class PiInquiryGroupingAccess {

	private $current_path;
	private $access_values;
	private $uid;
	private $inquiry;
	private $subgroup;
	private $roles;
	private $phases;
	private $activities;

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

	public function set_current_activity($activity_nid) {
		$node = node_load($activity_nid);
		$phase = node_load($node->phase_nid);

		$path = new stdClass();
		$path->phase_id = $node->phase_nid;
		$path->inquiry_id = $phase->inquiry_nid;
		$path->activity_id = $node->nid;
		$path->stage_id = $node->activity_phases_stages['stages']['stage_select'];

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
			$this->inquiry['access'] = $this->inquiry['node']->uid || nquire_group_get_user_status_in_group($this->inquiry['node']->actor_gid, $this->uid) === 'member';
			$this->inquiry['roles'] = nquire_commons_decode_data($this->inquiry['node']->roles);

			$this->subgroup = pi_inquiry_groupings_get_user_subgroup($this->inquiry['node'], $this->uid);
			$this->roles = array(
					'inquiry' => nquire_group_get_user_roles($this->inquiry['node']->actor_gid, $this->uid),
					'subgroup' => $this->subgroup ? nquire_group_get_user_roles($this->subgroup, $this->uid) : FALSE,
			);


			$this->load_phases();
			$this->load_activities();
		}
	}

	private function load_phases() {
		$this->phases = array();

		$phase_nids = pi_phase_load_nids_for_inquiry($this->inquiry['nid']);
		foreach ($phase_nids as $nid) {
			$item = array();
			$item['node'] = node_load($nid);
			$item['access'] = $this->inquiry['access'];
			$item['activities'] = array();
			$this->phases[$nid] = $item;
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
							$item['access'] = 'edit';
							break;
						case 'group':
							$item['access'] = $this->subgroup ? $this->max_access_for_user_roles($this->roles['subgroup'], $activity_nid) : FALSE;
							break;
						default:
							$item['access'] = $this->max_access_for_user_roles($this->roles['inquiry'], $activity_nid);
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

	private function max_access_for_user_roles($user_roles, $activity_nid) {
		if (count($this->inquiry['roles']) === 0) {
			return 'edit';
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

}

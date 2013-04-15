<?php

class PiInquiryGroupingAccess {

	private $current_path;
	private $access_values;
	private $uid;
	private $inquiry;
	private $subgroup;
	private $roles;
	private $phases;

	function __construct() {
		global $user;

		$this->access_values = array(
				'noaccess' => 0,
				'view' => 1,
				'comment' => 2,
				'edit' => 3,
		);

		$this->current_path = get_inquiry_details_from_current_path();

		if ($this->current_path->inquiry_id) {
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
		foreach ($this->phases as $phase_nid => $data) {
			$activity_nids = pi_get_activities_for_phase($phase_nid);
			foreach ($activity_nids as $activity_nid) {
				$item = array();
				$item['node'] = node_load($activity_nid);
				$item['collaboration'] = $data['node']->collaboration;
				$item['phase_nid'] = $data['node']->nid;

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
			}
		}
	}

	private function max_access_for_user_roles($user_roles, $activity_nid) {
		if (count($this->inquiry['roles']) === 0) {
			return 'edit';
		} else {
			$access = 'noaccess';
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
		return $this->inquiry_node;
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

}

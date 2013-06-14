<?php

function pi_activities_page($node, $mode, $child_node = NULL) {
	global $user;



	switch ($mode) {
		case 'phase':
			$ok = $node->type === 'pi_phase';
			break;
		case 'edit':
		case 'child_add':
		case 'child_edit':
		case 'child_delete':
			$ok = $node->type === 'pi_activity';
			break;
	}

	if ($ok) {
		include_once(drupal_get_path('module', 'node') . '/node.pages.inc');

		$inquiry_info = pi_info()->getCurrentInquiryInfo();
		$inquiry_access = pi_info()->getAccessManager($inquiry_info->getInquiryNid(), $user->uid);

		$phase_node = $mode === 'phase' ? $node : $inquiry_info->getPhase($node->phase_nid);
		$activity_node = $mode === 'phase' ? NULL : $node;

		$phase_count = count($inquiry_info->getPhases());
		$phase_index = $inquiry_info->getPhaseIndex($phase_node->nid);
		$keys = _ag_phase_keys($phase_count);
		$phase_key = $keys[$phase_index % strlen($keys)];

		if ($mode === 'child_edit' || 'child_add') {
			$separate_f = "{$activity_node->activity_type}_edit_child_in_separate_page";
			$separate_page = function_exists($separate_f) && $separate_f($activity_node);
		}

		$data = array(
				'in_phase' => !$separate_page,
				'phase' => array(
						'node' => $phase_node,
						'phase_key' => $phase_key,
						'title' => check_plain($phase_node->title),
						'description' => nl2br(check_plain($phase_node->body)),
				),
				'activities' => array(),
		);



		if ($data['in_phase']) {
//		$link = l(t('here'), 'phase/' . $phase_node->nid . '/shared');
			switch ($phase_node->collaboration) {
				case 'individual':
					$data['phase']['sharing'] .= t('Activities in this phase are carried out individually. ') /* 	. t('Click !link to see what other participants are doing.', array('!link' => $link)) */;
					break;
				case 'group':
					$data['phase']['sharing'] .= t('Activities in this phase are carried out in groups.') /* . t('Click !link to see what other groups are doing.', array('!link' => $link)) */;
					break;
				default:
					$data['phase']['sharing'] .= t('Activities in this phase are done by all inquiry participants working together.');
					break;
			}
		}


		$activities = $mode === $separate_page ? array($activity_node->nid => $activity_node) : $inquiry_info->getActivitiesForPhase($phase_node->nid);

		foreach ($activities as $a_nid => $a_node) {
			$active_activity = $a_nid == $node->nid;
			$data['activities'][$a_nid] = _pi_activities_get_activity_data($inquiry_info, $inquiry_access, $user, $phase_key, $separate_page, $a_node, $active_activity, $mode, $child_node);
		}

		$output = theme('pi_activities_view_phase', $data);

		return $output;
	} else {
		return '';
	}
}

function _pi_activities_get_activity_data($inquiry_info, $inquiry_access, $user, $phase_key, $separate_page, $activity_node, $active_activity, $mode, $child_node) {
	$access = $inquiry_access->getAccessToItem($activity_node);
	$reason = $inquiry_access->getAccessReason($activity_node);
	$inquiry_nid = $inquiry_info->getInquiryNid();
	$exaplanation = pi_activity_access_get_explanation_text($access, $reason, $inquiry_nid);
	$can_view = $active_activity ? pi_activity_tab_edit_access_callback($activity_node) : pi_activity_tab_view_access_callback($activity_node);

	if ($can_view) {
		$content_manager = pi_info()->getContentManager();
		$content_nid = $content_manager->getContent($activity_node->nid, $user->uid, TRUE);
		$content_node = node_load($content_nid);
	}

	$data = array(
			'node' => $activity_node,
			'title' => check_plain($activity_node->title),
			'description' => nl2br(check_plain($activity_node->body)),
			'links' => $access === 'edit' ? pi_activity_create_edit_links($activity_node, $active_activity && $mode === 'edit') : NULL,
			'access' => $access,
			'reason' => $reason,
			'access_explanation' => $exaplanation,
			'phase_key' => $phase_key,
			'can_view' => $can_view,
	);

	if ($can_view) {
		$options = array(
				'edit' => $active_activity && $mode === 'edit',
				'child_edit' => $active_activity && $mode === 'child_edit' ? $child_node->nid : NULL,
				'child_delete' => $active_activity && $mode === 'child_delete' ? $child_node->nid : NULL,
				'add_child' => $active_activity && $mode === 'child_add',
				'editable' => $access === 'edit',
				'activity_node' => $activity_node,
				'separate_page' => $separate_page,
		);


		if ($mode === 'edit' && !$content_node) {
			global $user;
			$content_node = new stdClass();
			$content_node->type = $activity_node->activity_type;
			$content_node->uid = $user->uid;

			$options['new'] = TRUE;
		}

		$view_f = "{$activity_node->activity_type}_display_content_in_activity";
		$data['content'] = $view_f($content_node, $options);
	}

	return $data;
}

function theme_pi_activities_view_phase($data) {
	$output = '';
	return $output;
}

function theme_pi_activities_view_activity($activity_data) {
	$output = '';
	return $output;
}

function theme_pi_activities_view_contribution_content($phases_data) {
	$output = '';
	return $output;
}

function theme_pi_activities_view_shared_activity($activity_data) {
	$output = '';
	return $output;
}


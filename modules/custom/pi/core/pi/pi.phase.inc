<?php

/** @file This file contains all the functions that display the phase level view.
 * This is the page where the activities will be listed
 */

/**
 * Produces the view of the activities, etc within a particular phase in an inquiry
 * @param node $phase A node of type pi_phase
 * @return String HTML output of the phase
 */
function pi_inquiry_phase_view($phase) {
	global $user;
	$inquiry_info = pi_info()->getCurrentInquiryInfo();
	$inquiry_access = pi_info()->getAccessManager($inquiry_info->getInquiryNid(), $user->uid);

	$output = _pi_inquiry_phase_view($phase, $inquiry_info, $inquiry_access, $user);
	return $output;

	$output .= '</small>';
}

function _pi_inquiry_phase_view($phase, $inquiry_info, $inquiry_access, $user) {

	$phase_count = count($inquiry_info->getPhases());
	$phase_index = $inquiry_info->getPhaseIndex($phase->nid);
	$keys = _ag_phase_keys($phase_count);
	$phase_key = $keys[$phase_index % strlen($keys)];

	$phase_data = array(
			'phase_key' => $phase_key,
			'title' => check_plain($phase->title),
			'description' => nl2br(check_plain($phase->body)),
	);

	$link = l(t('here'), 'phase/' . $phase->nid . '/shared');

	switch ($phase->collaboration) {
		case 'individual':
			$phase_data['sharing'] .= t('Activities in this phase are carried out individually. ')
							. t('Click !link to see what other participants are doing.', array('!link' => $link));
			break;
		case 'group':
			$phase_data['sharing'] .= t('Activities in this phase are carried out in groups.')
							. t('Click !link to see what other groups are doing.', array('!link' => $link));
			break;
		default:
			$phase_data['sharing'] .= t('Activities in this phase are done by all inquiry participants working together.');
			break;
	}


	$activities = $inquiry_info->getActivitiesForPhase($phase->nid);
	$activity_data = array();
	foreach ($activities as $activity_node) {
		$activity_data[] = pi_activity_display_data($activity_node, $phase_key, $inquiry_info, $inquiry_access, $user, 'view');
	}

	$output .= theme('pi_inquiry_phase_view', $phase_data, $activity_data);

	return $output;
}


function theme_pi_inquiry_phase_activity_view($activity_data) {
	$activity_text = '<div style="margin-bottom:12px;">'
					. '<span style="font-size:16px;font-weight: bold;padding-right: 20px;">' . check_plain($activity_data['title']) . '</span>'
					. $activity_data['links']
					. '</div>';

	$activity_text .= '<div style="margin-bottom:12px;">'
					. '<div>' . $activity_data['description'] . '</div>'
					. '<div>' . $activity_data['access_explanation'] . '</div>'
					. '</div>';

	if ($activity_data['can_view']) {

		$content_wrapper_function = "{$activity_data['node']->activity_type}_wrap_content_node_view";
		if (!function_exists($content_wrapper_function) || $content_wrapper_function($activity_data['node'])) {
			$activity_text .= '<div style="padding: 5px; background: #eee; border: 1px dotted #aaa;">'
							. $activity_data['content']
							. '</div>';
		} else {
			$activity_text .= $activity_data['content'];
		}
	}

	return $activity_text;
}

function pi_inquiry_phase_shared_view($phase) {
	global $user;
	$inquiry_info = pi_info()->getCurrentInquiryInfo();
	$inquiry_access = pi_info()->getAccessManager($inquiry_info->getInquiryNid(), $user->uid);

	drupal_set_title($phase->title);

	$output = nl2br(check_plain($phase->body));

	$output .= '<br><small>';

	$link = l(t('here'), 'phase/' . $phase->nid);

	switch ($phase->collaboration) {
		case 'individual':
			$output .= t('This is the content shared by other participats in this phase. ')
							. t('Click !link to go to your activities.', array('!link' => $link));
			break;
		case 'group':
			$output .= t('This is the content shared by other groups')
							. t('Click !link to go to your group activities.', array('!link' => $link));
			break;
		default:
			$output .= t('Click !link to go to your activities.', array('!link' => $link));
			break;
	}
	$output .= '</small>';

	$activities = $inquiry_info->getActivitiesForPhase($phase->nid);

	if (count($activities) > 0) {
		$list = array();

		foreach ($activities as $activity_node) {
			$activity_title = check_plain($activity_node->title);
			if ($inquiry_access->getAccessToItem($activity_node) !== 'hidden') {
				$activity_content = pi_activity_shared_content_list($activity_node);
			} else {
				$activity_content = t('You do not have access to view this activity.');
			}

			$list[] = array('data' => array($activity_title, $activity_content), 'class' => 'activity');
		}

		$attributes = array('class' => 'activity');

		$header = array(t('Activity'), t('Description'));
		$output .= theme_table($header, $list, $attributes);
	} else {
		drupal_set_message(t('No activities are available in this phase at this stage.'));
	}
	return $output;
}


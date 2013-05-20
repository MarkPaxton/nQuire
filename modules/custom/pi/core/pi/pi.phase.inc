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


	drupal_set_title($phase->title);

	$output = nl2br(check_plain($phase->body));

	$output .= '<br><small>';

	$link = l(t('here'), 'phase/' . $phase->nid . '/shared');

	switch ($phase->collaboration) {
		case 'individual':
			$output .= t('Activities in this phase are carried out individually. ')
							. t('Click !link to see what other participants are doing.', array('!link' => $link));
			$work_header = t('My work');
			break;
		case 'group':
			$output .= t('Activities in this phase are carried out in groups.')
							. t('Click !link to see what other groups are doing.', array('!link' => $link));
			$work_header = t('My group\' work');
			break;
		default:
			$output .= t('Activities in this phase are done by all inquiry participants working together.');
			$work_header = t('Our work');
			break;
	}
	$output .= '</small>';

	$inquiry_info = pi_info()->getCurrentInquiryInfo();
	$inquiry_access = pi_info()->getAccessManager($inquiry_info->getInquiryNid(), $user->uid);
	$activities = $inquiry_info->getActivitiesForPhase($phase->nid);

	if (count($activities) > 0) {
		$list = array();

		foreach ($activities as $activity_node) {
			$activity_text = '<div style="margin-bottom:12px;">'
							. '<span style="font-size:16px;font-weight: bold;padding-right: 20px;">' . check_plain($activity_node->title) . '</span>'
							. pi_activity_create_phase_view_activity_links($activity_node)
							. '</div>';
			$access = $inquiry_access->getAccessToItem($activity_node);
			$reason = $inquiry_access->getAccessReason($activity_node);

			$activity_text .= '<div style="margin-bottom:12px;">';
			if ($activity_node->body) {
				$activity_text .= '<div>' . nl2br(check_plain($activity_node->body)) . '</div>';
			}
			$activity_text .= theme('pi_activity_access_explanation', $access, $reason, $inquiry_info->getInquiryNid())
							. '</div>';


			$content_manager = pi_info()->getContentManager();
			$content_nid = $content_manager->getContent($activity_node->nid, $user->uid, TRUE);
			$content_node = node_load($content_nid);


			$can_view = pi_activity_tab_view_access_callback($activity_node) && ($access === 'edit' || $access === 'view');


			if ($can_view && $content_node) {
				include_once(drupal_get_path('module', 'node') . '/node.pages.inc');

				$node_view = node_view($content_node, FALSE, TRUE, TRUE);

				$content_wrapper_function = "{$activity_node->activity_type}_wrap_content_node_view";
				if (!function_exists($content_wrapper_function) || $content_wrapper_function($activity_node)) {
					$activity_text .= '<div style="padding: 5px; background: #eee; border: 1px dotted #aaa;">'
									. $node_view . '</div>';
				} else {
					$activity_text .= $node_view;
				}
			}

			$list[] = array('data' => array($activity_text), 'class' => 'activity');
		}

		$attributes = array('class' => 'activity');

		$header = array(t('Activity'));
		$output .= theme_table($header, $list, $attributes);
	} else {
		drupal_set_message(t('No activities are available in this phase at this stage.'));
	}
	return $output;
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


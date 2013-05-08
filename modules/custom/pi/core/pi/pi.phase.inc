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
			break;
		case 'group':
			$output .= t('Activities in this phase are carried out in groups.')
							. t('Click !link to see what other groups are doing.', array('!link' => $link));
			break;
		default:
			$output .= t('Activities in this phase are done by all inquiry participants working together.');
			break;
	}
	$output .= '</small>';

	$inquiry_info = pi_info()->getCurrentInquiryInfo();
	$inquiry_access = pi_info()->getAccess($inquiry_info->getInquiryNid(), $user->uid);
	$activities = $inquiry_info->getActivitiesForPhase($phase->nid);

	if (count($activities) > 0) {
		$list = array();

		foreach ($activities as $activity_nid => $activity_node) {
			$activity_link = pi_activity_build_link($activity_nid, $user->uid);

			if ($inquiry_access->getAccess($activity_nid) !== 'hidden') {
				$activity_desc = pi_activity_teaser($activity_node);
			} else {
				$activity_desc = t('You do not have access to view or edit this activity.');
			}

			$list[] = array('data' => array($activity_link, $activity_desc), 'class' => 'activity');
		}

		$attributes = array('class' => 'activity');

		$header = array(t('Activity'), t('Description'));
		$output .= theme_table($header, $list, $attributes);
	} else {
		drupal_set_message(t('No activities are available in this phase at this stage.'));
	}
	return $output;
}

function pi_inquiry_phase_shared_view($phase) {
	global $user;
	$inquiry_info = pi_info()->getCurrentInquiryInfo();
	$inquiry_access = pi_info()->getAccess($inquiry_info->getInquiryNid(), $user->uid);
	
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

		foreach ($activities as $activity_nid => $activity_node) {
			$activity_title = check_plain($activity_node->title);
			if ($inquiry_access->getAccess($activity_nid) !== 'hidden') {
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


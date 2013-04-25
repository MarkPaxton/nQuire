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

	$access_data = pi_inquiry_groupings_get_access_data();
	$uid = $access_data->get_uid();

	drupal_set_title($phase->title);

	$output = nl2br(check_plain($phase->body));

	$output .= '<br><small>';

	$link = l(t('here'), 'phase/' . $phase->nid . '/sharedcontent');

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

	$phase_data = $access_data->get_phase($phase->nid);
	$activities = $phase_data['activities'];

	if (count($activities) > 0) {
		$list = array();

		foreach ($activities as $activity_nid => $activity_data) {
			$activity = $activity_data['node'];

			$activity_link = pi_activity_build_link($activity_nid, $uid);

			if ($activity_data['access']) {
				$activity_desc = pi_activity_teaser($activity);
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


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
	switch ($phase->collaboration) {
		case 'individual':
			$output .= t('Activities in this phase are carried out individually.');
			break;
		case 'group':
			$output .= t('Activities in this phase are carried out in groups.');
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
				$nid = pi_activity_get_activity_content_nid_for_activity_and_user($activity_nid, $uid);
				$activity_content_node = node_load($nid);
				// If no content node is associated, then check for child nodes... 
				if (!$activity_content_node) {
					$activity_content_node = pi_activity_create_content_on_first_visit($activity, $uid);
				}
				if (!$activity_content_node) {
					$activity_content_node = $activity;
				}


				$loaded_activity_status = pi_activity_get_activity_status_for_user($activity->nid, $uid);
				if (count($loaded_activity_status) == NULL) {
					$loaded_activity_status['visible_status'] = $activity->visible_status;
					$loaded_activity_status['node_function'] = $activity->node_function;
				}

				// (By default) always open an existing node in view
				// if node_function for link is edit or view then set node function to view
				$link_node_function = $loaded_activity_status['node_function'];
				if (($link_node_function == 'edit') || ($link_node_function == 'view'))
					$link_node_function = 'view';

				$activity_desc = pi_activity_teaser($activity_content_node, $activity, $link_node_function);
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
?>


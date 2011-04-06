<?php
/** @file This file contains all the functions that display the phase level view.
 * This is the page where the activities will be listed
 */


/**
 * Produces the view of the activities, etc within a particular phase in an inquiry
 * @param $phase A node of type pi_phase
 * @return sting HTML output of the phase
 */
function pi_inquiry_phase_view($phase)
{
	global $user;

	$details = get_inquiry_details_from_current_path();
	drupal_set_title($phase->title);

	$activities = pi_get_activities_for_phase_and_user($phase->nid, $user->uid);

	$output = "";
	$output .= t("<p>" . $phase->body . "</p>");

	$hidden_created_activity_types = get_hidden_summary_activities($phase->inquiry_nid);

	if ($activities)
	{
		$list = array();
		$active_activity_flag = 0;
		foreach($activities as $activity_nid)
		{
			$activity = node_load($activity_nid);

			// only display activities that are not in the hidden list
			if(!(in_array($activity->node_type, $hidden_created_activity_types)))
			{
				//Changed the order of the code execution because this function can possibly generate
				//the missing node instance (pi_fooddiary_data).
				//Therefore activity status and build activity link has to be executed later.
				$nid = pi_get_activity_content_nid_for_activity_and_user($activity->nid, $user->uid); 
				//get_nid_for_inquiry_activity_and_user($phase->inquiry_nid, $activity->activity_id, $user->uid);
				$loaded_actvity_node = node_load($nid);			
				$loaded_activity_status = pi_activity_load_status($activity->nid, $user->uid);
				
				// so that (by default) always open an existing node in view
				// if node_function for link is edit or view then set node function to view
				$link_node_function = $loaded_activity_status['node_function'];

				if(($link_node_function == 'edit') || ($link_node_function == 'view'))
				{
					$link_node_function = 'view';
				}

				$activity_link = pi_activity_build_link($activity_nid, $user->uid);
				if (($active_activity_flag == 0) && $activity)
				{
					$active_activity_flag = 1;
				}

				$activity_desc = pi_activity_teaser($loaded_actvity_node, $activity, $link_node_function);
					
				$list[] = array('data' => $activity_link . $activity_desc, 'class' => 'activity');
			}
		}

		$title = NULL;
		$type = 'ul';
		$attributes = array('class' => 'activity');

		$output .= theme_item_list($list, $title, $type, $attributes);

		if ($active_activity_flag == 0)
		{
			drupal_set_message(t('None of the above activities can be accessed at this stage.'));
		}
	}
	else
	{
		drupal_set_message(t('No activities are available in this phase at this stage.'));
	}

	return $output;
}


?>


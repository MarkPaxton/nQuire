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

	//stage is set to NULL to show all activities of the phase
	$activities = get_activities_of_stage_phase_and_user($phase->inquiry_nid, NULL, $phase->nid, $user->uid);

	$output = "";
	$output .= t("<p>" . $phase->body . "</p>");


	$hidden_created_activity_types = get_hidden_summary_activities($phase->inquiry_nid);

	if ($activities)
	{
		$list = array();

		$active_activity_flag = 0;

		foreach($activities as $activity)
		{
			$loaded_activity = load_activity($activity->activity_id);
			//$loaded_activity_status = load_activity_status($activity->activity_id, $user->uid);

			// only display activities that are not in the hidden list
			if(!(in_array($loaded_activity->node_type, $hidden_created_activity_types)))
			{
				//Changed the order of the code execution because this function can possibly generate
				//the missing node instance (pi_fooddiary_data).
				//Therefore activity status and build activity link has to be executed later.
				$nid = get_nid_for_inquiry_activity_and_user($phase->inquiry_nid, $activity->activity_id, $user->uid);
				
				$loaded_actvity_node = node_load($nid);			
				
				$loaded_activity_status = load_activity_status($activity->activity_id, $user->uid);

				// so that (by default) always open an exitsing node in view
				// if node_function for link is edit or view then set node function to view
				$link_node_function = $loaded_activity_status->node_function;

				if(($link_node_function == 'edit') || ($link_node_function == 'view'))
				{
					$link_node_function = 'view';
				}

				$activity_link = build_activity_link($details->inquiry_id, $details->stage_id, $details->phase_id, $activity->activity_id, $user->uid, 'heading');
				if (($active_activity_flag == 0) && $loaded_activity)
				{
					$active_activity_flag = 1;
				}

				$activity_desc = pi_activity_teaser($loaded_actvity_node, $loaded_activity, $link_node_function);
					
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


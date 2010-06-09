<?php
/** @file This file contains all the functions that display the phase level view.
 * This is the page where the activities will be listed 
 */


/**
 * Produces the view of the activities, etc within a particular phase in an inquiry
 * @param $phase A node of type pi_phase
 * @return sting HTML output of the phase
 */
function pi_inquiry_phase_view($phase) {

	global $user;
	$details = get_inquiry_details_from_current_path();
	drupal_set_title($phase->title);

	//stage is set to NULL to show all activities of the phase
	$activities = get_activities_of_stage_phase_and_user($phase->inquiry_nid, NULL, $phase->nid, $user->uid);

	$output = "";
	$output .= "<div style='display: block; clear: both;'>";
	$output .= t("<p>" . $phase->body . "</p>");
	$output .= "</div>";
	

	$hidden_created_activity_types = array('pi_key_questions', 'pi_measure_filter', 'pi_available_measures', 'pi_result_presentations', 'pi_data', 'pi_key_answers');
	//$hidden_created_activity_types = array('pi_measure_filter');
	
	
	//$hidden_created_activity_types = array();
	if ($activities)
	{
		// $headings = array('Activity', 'You can...', 'Description');
		$headings = array('Activity', 'Description');
		$table = array();

		$active_activity_flag = 0;

		foreach($activities as $activity)
		{
			$loaded_activity = load_activity($activity->activity_id);
			$loaded_activity_status = load_activity_status($phase->inquiry_nid, $activity->activity_id, $user->uid);
				
			// only display activities that are not in the hidden list
			if(!(in_array($loaded_activity->node_type, $hidden_created_activity_types)))
			{
				switch ($loaded_activity->destination)
				{
					case "self":
						$destination = t("destination=" . t("activity/" . $activity->activity_id . "/view"));
						break;
					case "phase":
						$destination = t("destination=" . t("phase/" . $phase->nid));
						break;
					default:
						$destination = t("destination=" . t("inquiry/" . $phase->inquiry_nid));
						break;
				};
					
				// so that (by default) always open an exitsing node in view
				// if node_function for link is edit or view then set node function to view
				$link_node_function = $loaded_activity_status->node_function;
				
				if (($link_node_function == 'edit') OR ($link_node_function == 'view')) {
					$link_node_function = 'view';
				};
					
				/*
				$activity_path = "activity/" . $activity->activity_id . "/" . $link_node_function;
				if ($loaded_activity_status->visible_status == 'unavailable') {
						
					$activity_link = check_plain($loaded_activity->name) . '&nbsp;' . add_activity_icon($loaded_activity_status->visible_status);
				}
				else {
					$image_path = drupal_get_path( 'module', 'pi' ) . '/images/';
						
						
					//new activity lightbulb
					if (in_array($activity->activity_id, new_activity_list($details->inquiry_id, $details->phase_id, $user->uid))) {
						$new_icon = "<img width='12' src='" . $image_path . "new_activity.png'/>";
					}
					else {
						$new_icon = "";
					};

					$activity_link = l(check_plain($loaded_activity->name) . '&nbsp;' . add_activity_icon($loaded_activity_status->visible_status) . '&nbsp;' . $new_icon, $activity_path, array('html' => TRUE, 'query' => $destination));
				};
*/
				$activity_link = build_activity_link($details->inquiry_id, $details->stage_id, $details->phase_id, $activity->activity_id, $user->uid);
				if (($active_activity_flag == 0) AND $loaded_activity)
				{
					$active_activity_flag = 1;
				}

				// $table[] = array($activity_link , ucwords(t($loaded_activity_status->visible_status)), t($loaded_activity->description));
				$nid = get_nid_for_inquiry_activity_and_user($phase->inquiry_nid, $activity->activity_id, $user->uid);
				$loaded_actvity_node = node_load($nid);
				$activity_desc = pi_activity_teaser($loaded_actvity_node, $loaded_activity, $link_node_function);
				$table[] = array(
						array('data' => '<div class="teaser_table_cell">'. $activity_link['data'].'</div>', 'valign' => 'top'),
						array('data' => $activity_desc, 'valign' => 'top')
						//array('data' => $activity_link, 'valign' => 'top'),
						//t($activity_desc)
					);
				//$table[] = array($activity_link , t($loaded_activity->description));
			}
		}

		$output .= theme('table', $headings, $table);
		
		if ($active_activity_flag == 0) {
			drupal_set_message(t('None of the above activities can be accessed at this stage.'));
		};
	}
	else {
		drupal_set_message(t('No activities are available in this phase at this stage.'));
	};
	return $output;
}


?>


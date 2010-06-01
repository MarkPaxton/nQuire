<?php
// $Id: pi.activity.inc, 2010/05/17 MCP $

/**
 * @file Provides view functions for pages under the url activity/* 
 * 
 */

/**
 * Generate the page for a pi activity, provides a custom header and footer for the content
 *  and also works out permissions and displayed the view for the correct activity function
 *  (add/view/edit) for the node/node_type the activity references
 * @param uint $activity_id The id of the pi_activity to work with  
 * @param $arg_node_function The function to perform on the activity
 */
function pi_activity_view($activity, $arg_node_function) {

	global $user;
	$uid = $user->uid;
	
	$output = '';
	drupal_set_title('');


	// Check that the node function given int the URL is allowed by comparing it with the 
	//   node function in the database for this inquiry and user
	//$node_function = get_node_function_of_activity($details->inquiry_id, $activity_id, $uid);
	// this is now handled with the access callback pi_activity_access
	switch($arg_node_function)
	{
		case 'view':
			$details = get_inquiry_details_from_current_path();
			//get nid
			$nid = get_nid_for_inquiry_activity_and_user($details->inquiry_id, $activity->activity_id, $uid);
			if(isset($nid))
			{
				$loaded_node = node_load($nid);
				//build page
				if(!function_exists("node_view"))
				{
					include_once(drupal_get_path('module', 'node') . '/node.pages.inc');
				}
				$output .= node_view($loaded_node, FALSE, TRUE, TRUE);
			}
			else
			{
				drupal_set_message(t("Can't find the content this activity."), 'error');
			}
			//$output .= previous_next_buttons($details->inquiry_id, $details->stage_id, $details->phase_id, $activity_id, $uid);
			//$output .= previous_next_buttons($details->inquiry_id, $activity_id, $uid);
			//$output .= t(next_button());
			break;
		case 'add': 
			//get node type
			$node_type = $activity->node_type;
			if(!function_exists("node_add"))
			{
				include_once(drupal_get_path('module', 'node') . '/node.pages.inc');
			}
			$output .= pi_activity_actor_audience_info($details->inquiry_id, $activity->activity_id);
			$output .= node_add($node_type);
			break;
		case 'edit':
			//get nid
			$nid = get_nid_for_inquiry_activity_and_user($details->inquiry_id, $activity->activity_id, $uid);
			//build form
			$loaded_node = node_load($nid);
			if( !function_exists("node_page_edit")) {
				include_once(drupal_get_path('module', 'node') . '/node.pages.inc');
			}
			$output .= pi_activity_actor_audience_info($details->inquiry_id, $activity->activity_id);
			$output .= node_page_edit($loaded_node);
			break;
	}
	return $output;
}

/**
 * Generate the "To be done by me, will be seen by my group" type text for a pi_activity
 * @param $inquiry_id the id of the inquiry the activity belongs to
 * @param $activity_id the id of the activity to generate the text for
 */
function pi_activity_actor_audience_info($inquiry_id, $activity_id) {
	global $user;
	$output = "";
	
	if ($activity_id && $inquiry_id)
	{
		$loaded_activity = load_activity($activity_id);
		$audience = $loaded_activity->audience;
		$loaded_activity_actor = load_activity_actor($inquiry_id, $activity_id);
		$activity_uid = $activity_actor[0]["uid"];
		$activity_gid = $activity_actor[0]["gid"];
		$activity_gid_type = get_type_of_group($gid);
		 
		$seen_by = $audience;
		if (($activity_uid && !$activity_gid) || $audience == 'individual')
		{
			$done_by = 'individual';
		}
		elseif ($activity_gid_type == 'class' && $audience == 'class')
		{
			$done_by = 'class';
		}
		else
		{
			$done_by = 'group';
		}
		
		$output .= '<div class="submitted">';
		$output .= t('Being done by ') . pi_actor_audience_translate($done_by) . '. ';
		$output .= t('Will be seen by ') . pi_actor_audience_translate($seen_by) . '. ';
		$output .= "</div>\n";
	}
	return $output;
}

/**
 * Turns "PI language" text individual/group/class into friendly text me/my group/my class
 * @param string $actor either individual/group/class
 * @return a string turning actor into friendly text, or $actor if it's not possible.
 */
function pi_actor_audience_translate($actor) {
	$translation = array();
	$translation['individual'] = t('me');
	$translation['group'] =  t('my group');
	$translation['class'] = t('my class');
	if(array_key_exists($actor, $translation))
	{
		return $translation[$actor];
	}
	else
	{
		return t($actor);
	}
}

?>
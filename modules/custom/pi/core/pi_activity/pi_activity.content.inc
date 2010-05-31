<?php
// $Id: pi.activity.inc, 2010/05/17 MCP $

/**
 * @file Provides functions to view nodes created for an activity under the url /activity/content 
 */

/**
 * Generate the page for a pi activity, provides a custom header and footer for the content
 *  and also works out permissions and displayed the view for the correct activity function
 *  (add/view/edit) for the node/node_type the activity references
 * @param uint $activity_id The id of the pi_activity to work with  
 * @param $arg_node_function The function to perform on the activity
 */
function pi_activity_content($activity) {
	global $user;
	$uid = $user->uid;
	$details = get_inquiry_details_from_current_path();

	$output = '';
	drupal_set_title(t('Content for ' . $activity->name));
	$content_nids = pi_activity_get_activity_content_nids($activity->activity_id, $details->inquiry_id);
	$title_row = array(t('Title'), t('Author'));
	$rows=array();
/*	ob_start();
	var_dump($content_nids);
	drupal_set_message('<pre>' . ob_get_clean() . '</pre>');
*/
	foreach($content_nids as $nid)
	{
		$row=array();
		$node = node_load($nid);
		if(isset($node) && ($nid>0))
		{
			$row[] = $node->title;
			$row[] = user_load($node->uid)->name;
		}
		else
		{
			$row[] = $nid;
			$row[] = t('No data found');
		}
		$rows[] = array('data' => $row);
	}
	$output .= theme('table', $title_row, $rows);
	return $output;
}

?>
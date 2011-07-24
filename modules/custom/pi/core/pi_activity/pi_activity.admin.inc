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
	drupal_set_title(t('Content for ' . $activity->title));
	$content_nids = pi_activity_get_activity_content_nids($activity->nid, $details->inquiry_id);
	$title_row = array(t('Title'), t('Author')/*, t('Groups')*/);
	$rows=array();
	/*	ob_start();
	 var_dump($content_nids);
	 drupal_set_message('<pre>' . ob_get_clean() . '</pre>');
	 */
	foreach($content_nids as $nid)
	{
		$row=array();

		if( is_numeric( $nid ) && $nid > 0 )
		{
			$node = node_load($nid);
			if(isset($node))
			{
				//	$node_groups = pi_activity_get_activity_content_group_access($node->nid);
				$row[] = l($node->title, 'activity/' . $activity->nid . '/admin/view/' . $node->nid);
				$row[] = user_load($node->uid)->name;
				/*	$group_row = array();
				 foreach($node_groups as $group)
				 {
				 $group_row[] = check_plain($group->title);
				 }
				 $row[] = implode(', ', $group_row);*/
			}
			else
			{
				$row[] = $nid;
				$row[] = t('No data found');
			}
			$rows[] = array('data' => $row);
		}
	}
	$output .= theme('table', $title_row, $rows);
	return $output;
}

function pi_activity_edit($activity)
{
	$output = "";

	if( !function_exists("node_page_edit")) {
		include_once(drupal_get_path('module', 'node') . '/node.pages.inc');
	}

	if( is_object( $activity ) ) {
		$output .= node_page_edit($activity);
	}

	//return "This page has been intentionally left blank.";
	return $output;
}

function pi_activity_admin_view_node($activity, $node)
{
	$output = '';
	if(!function_exists("node_view"))
	{
		include_once(drupal_get_path('module', 'node') . '/node.pages.inc');
	}
	$output .= theme('pi_activity_admin_view_node', $activity, node_view($node, FALSE, TRUE, TRUE));
	drupal_set_title('Content for activity "' . check_plain($activity->title) . '"');
	return $output;
}

function theme_pi_activity_admin_view_node($activity, $node_view_page)
{
	$output = '';

	$link = l("here", 'activity/' . $activity->nid . '/admin/content/');
	$output = "<p>Click $link to return to the activity content list.</p>";
	$output .= "<div style='background:#CCCCCC;padding:6px;'>";
	$output .= $node_view_page;
	$output .= "</div>";
	return $output;
}

function pi_activity_get_activity_content_group_access($activity_content_nid)
{
	$query = "SELECT * FROM {node_access} "
	. "LEFT JOIN {og} ON {node_access}.`gid` = {og}.`nid` "
	. "LEFT JOIN {node} ON {node_access}.`nid` = {node}.`nid` "
	. "WHERE {node_access}.`nid` = %d";
	$result = db_query($query, $activity_content_nid);
	$data = array();
	while($row=db_fetch_object($result))
	{
		$data[] = $row;
	}
	return $data;
}
?>
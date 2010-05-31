<?php 
/**
 * @file This file contains the page callback functions and any necessary others for 
 * the items under the menu inquiry/add 
 */


/**
 * This page is the starting point for creating content for an inquiry
 * @param $inquiry
 */
function pi_authoring_inquiry_add()
{
	$output = '';
	if( !function_exists("node_add"))
	{
		include_once(drupal_get_path('module', 'node') . '/node.pages.inc');
	}
	$output .= node_add('pi_inquiry');
	return $output;
}

?>
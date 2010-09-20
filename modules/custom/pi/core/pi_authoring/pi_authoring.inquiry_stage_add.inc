<?php
/**
 * @file This file contains the page callback functions and any necessary others for 
 * the items under the menu inquiry/%inquiry/stage/add 
 */


/**
 * This page is the starting point for creating content for an inquiry
 */
function pi_authoring_inquiry_stage_add()
{
	$output = '';
	if( !function_exists("node_add"))
	{
		include_once(drupal_get_path('module', 'node') . '/node.pages.inc');
	}
	$output .= node_add('pi_stage');
	return $output;
}

?>
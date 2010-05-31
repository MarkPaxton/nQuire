<?php
/**
 * @file This file contains the page callback functions and any necessary others for 
 * the items under the menu inquiry/%inquiry/phase/add 
 */


/**
 * This page generates an add phase form for pi_phase
 * @param object(node->pi_inquiry) $inquiry
 * @return string HTML output
 */
function pi_authoring_inquiry_phase_add($inquiry)
{
	$output = '';
	if( !function_exists("node_add"))
	{
		include_once(drupal_get_path('module', 'node') . '/node.pages.inc');
	}
	$output .= node_add('pi_phase');
	return $output;
}

?>
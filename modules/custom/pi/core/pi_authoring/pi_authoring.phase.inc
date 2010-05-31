<?php

/** @file This file deals with all the pi_authoring content under the url /phase/
 *  In particular this holds a clever callback to redirect the browser to the inquiry page
 */

/**
 * this function provides a redirevt back to the associated inquiry when a phase is 
 * redirected here from a form edit see pi_authoring_from_pi_phase_form_alter() 
 * @param object(node->pi_phase) $phase
 */
function pi_authoring_phase_inquiry($phase)
{
	drupal_goto('inquiry/' . $phase->inquiry_nid . '/edit/phases');
}

/**
 * This page provides editing of phases that redirects back to the inquiry authoring pages
 * @param $inquiry
 */
function pi_authoring_phase_edit($phase)
{
	$output = '';
	if( !function_exists("node_page_edit"))
	{
		include_once(drupal_get_path('module', 'node') . '/node.pages.inc');
	}
	$output .= node_page_edit($phase);
	// Set the title here to override the title being set in node_page_edit
	drupal_set_title("Editing " .  check_plain($phase->title));
	return $output;
}



?>
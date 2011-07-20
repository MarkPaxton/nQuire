<?php

/** @file This file deals with all the pi_authoring content under the url /phase/
 *  In particular this holds a clever callback to redirect the browser to the inquiry page
 */

/**
 * this function provides a redirect back to the associated phase when an actibvity is 
 * redirected here from a form edit see pi_authoring_from_pi_activity_form_alter() 
 * @param object(node->pi_stage) $stage
 */
function pi_authoring_activity_inquiry($phase)
{
	drupal_goto('phase/' . $phase->inquiry_nid . '/edit/activities');
}

/**
 * This page provides editing of activities that redirects back to the inquiry authoring pages
 * @param $inquiry
 */
function pi_authoring_activity_edit($activity)
{
	$output = '';
	if( !function_exists("node_page_edit"))
	{
		include_once(drupal_get_path('module', 'node') . '/node.pages.inc');
	}
	$output .= node_page_edit($activity);
	// Set the title here to override the title being set in node_page_edit
	drupal_set_title(t("Editing @title", array('@title' => $activity->title) ));
	return $output;
}

?>
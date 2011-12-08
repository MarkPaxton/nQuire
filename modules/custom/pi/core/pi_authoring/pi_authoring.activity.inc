<?php

/** @file This file deals with all the pi_authoring content under the url /phase/
 *  In particular this holds a clever callback to redirect the browser to the inquiry page
 */



/**
 * this function provides a redirect back to the associated phase when an actibvity is
 * redirected here from a form edit see pi_authoring_from_pi_activity_form_alter()
 * @param object(node:pi_activity) $activity
 */
function pi_authoring_activity_inquiry($activity)
{
	if($activity->phase_nid !== false)
	{
		$phase = node_load($activity->phase_nid);
		if($phase->inquiry_nid!==false)
		{
			$inquiry = node_load($phase->inquiry_nid);
			if($inquiry!==false)
			{
				drupal_goto('inquiry/' . $inquiry->nid . '/edit/phases');
			}
			else
			{
				pi_authoring_activity_phase($activity);
			}
		}
		else
		{
			pi_authoring_activity_phase($activity);
		}
	}
}


/**
 * this function provides a redirect back to the associated phase when an actibvity is
 * redirected here from a form edit see pi_authoring_from_pi_activity_form_alter()
 * @param object(node->pi_phase) $phase
 */
function pi_authoring_activity_phase($activity)
{
	drupal_goto('phase/' . $activity->phase_nid . '/edit/activities');
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
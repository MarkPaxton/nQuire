<?php
/** @file This file contains all the functions to display the nQuire home view; this is the
 * page with all the inquiries listed.
 */ 


/**
 * pi_home_view - Home page for PI that lists all the inquiries
 */
function pi_home_view() {
	drupal_set_title("My inquiries");
	$output = "<p>Choose an inquiry.</p>";

	$list = array();

	//list all inquiries for the user
	global $user;
	$inquiries_for_user = get_inquiries_of_user($user->uid);
	
	foreach($inquiries_for_user as $inquiry)
	{
		$list[] = array('data' => l(check_plain($inquiry->title), "inquiry/" . $inquiry->nid, array('attributes' => array('class' => 'heading'))) . node_view($inquiry, TRUE, TRUE), 'class' => 'inquiry');		
	}

	$type = 'ul';
	$attributes = array('class' => 'inquiry');
	
	$output .= theme_item_list($list, $title, $type, $attributes);	
	// $output .= theme('table', $header, $rows);
	return $output;
}


?>
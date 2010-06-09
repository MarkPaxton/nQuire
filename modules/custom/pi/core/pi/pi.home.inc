<?php
/** @file This file contains all the functions to display the nQuire home view; this is the
 * page with all the inquiries listed.
 */ 


/**
 * pi_home_view - Home page for PI that lists all the inquiries
 */
function pi_home_view() {
	global $user;
	drupal_set_title("My inquiries");
	$output = "<p>Choose an inquiry.</p>";

	// Header
	$header = array(
		t('Inquiry'),
		t('Description'),
	);

	$rows = array();

	//list all inquiries for the user
	$inquiries_for_user = get_inquiries_of_user($user->uid);
	foreach($inquiries_for_user as $inquiry)
	{
		$row = array();
		$row[] = array('data' => l(check_plain($inquiry->title), "inquiry/" . $inquiry->nid), 'style' => 'font-size:130%;font-weight:bold;white-space:nowrap');
		$row[] = $inquiry->body;
		$rows[] = array('data' => $row);
	}

	$output .= theme('table', $header, $rows);
	return $output;
}


?>
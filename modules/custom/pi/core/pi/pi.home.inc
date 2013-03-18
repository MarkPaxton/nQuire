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

	global $user;

	$list = array();

	//list all inquiries for the user
	global $user;
	$inquiries_for_user = pi_inquiry_groupings_get_inquiries_for_user($user->uid);

	$header = array(t('Inquiry'), t('Description'));

	foreach ($inquiries_for_user as $status => $inquiries) {
		if (count($inquiries) > 0) {
			switch ($status) {
				case 'joined':
					$output .= '<p>' . t('My inquiries:') . '</p>';
					break;
				case 'awaiting':
					$output .= '<p>' . t('Inquiries awaiting decision on membership:') . '</p>';
					break;
				case 'available':
					$output .= '<p>' . t('Available inquiries:') . '</p>';
					break;
			}
			$rows = array();

			foreach ($inquiries as $inquiry_nid) {
				$inquiry = node_load($inquiry_nid);
				$status = theme('pi_inquiry_manager_participants_group_status_and_options', $inquiry->actor_gid, $user->uid);

				$rows[] = array(l($inquiry->title, "inquiry/{$inquiry->nid}") . '<br/>' . $status, $inquiry->description);
			}

			$output .= theme_table($header, $rows);
		}
	}

	return $output;
	foreach ($inquiries_for_user as $inquiry_nid) {
		$inquiry = node_load($inquiry_nid);
		$list[] = array('data' => l(check_plain($inquiry->title), "inquiry/" . $inquiry->nid, array('attributes' => array('class' => 'heading'))) . node_view($inquiry, TRUE, TRUE), 'class' => 'inquiry');
	}

	$type = 'ul';
	$attributes = array('class' => 'inquiry');

	$output .= theme_item_list($list, $title, $type, $attributes);
	// $output .= theme('table', $header, $rows);
	return $output;
}

?>
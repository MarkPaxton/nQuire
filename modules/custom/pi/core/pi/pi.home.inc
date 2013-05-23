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

	//$header = array(t('Inquiry'), t('Description'));

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
				$status = theme('pi_inquiry_manager_participants_group_status_and_options', $inquiry, $user->uid);
				
				$image = $inquiry->image ? ('<img src="' . file_create_url($inquiry->image) . '" style="float:left;width: 200px;"></img>') : '';
				$title = l('<h3>' . check_plain($inquiry->title) . '</h3>', "inquiry/{$inquiry->nid}", array('html' => TRUE)) 
								. '<p><b>' . nl2br(check_plain($inquiry->body)) . '</b></p>'
								. '<p><small>' . t('Created by !user', array('!user' => nquire_commons_user_name_or_me($inquiry->uid))) . '<br/>'
								. t('Number of participants: !n', array('!n' => count(nquire_group_get_member_uids($inquiry->actor_gid, TRUE)))) . '</small></p>';
				
				
				$rows[] = array(array('data' => $image, 'style' => 'width: 200px'), $title);
			}

			$output .= theme_table(NULL, $rows);
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
	return $output;
}


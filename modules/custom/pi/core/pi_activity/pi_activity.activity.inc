<?php

// $Id: pi.activity.inc, 2010/05/17 MCP $

/**
 * @file Provides view functions for pages under the url activity/*
 *
 */

/**
 * Generate the page for a pi activity, provides a custom header and footer for the content
 *  and also works out permissions and displayed the view for the correct activity function
 *  (add/view/edit) for the node/node_type the activity references
 *  If the user is the admin user, then redirect to the activity content admin page instead as admin
 *  can't create Activity Content (due to being in all og groups)
 * @param uint $activity_id The id of the pi_activity to work with
 * @param $arg_node_function The function to perform on the activity
 */
function pi_activity_content_view($activity_node, $arg_node_function, $teaser = FALSE) {

	$access_data = pi_inquiry_groupings_get_access_data();
	$activity_data = $access_data->get_activity($activity_node->nid);
	$content_node = node_load(pi_activity_get_content_nid_for_activity($activity_node->nid));

	if (!$content_node && !$teaser && $arg_node_function === 'view' && $activity_data['access'] === 'edit') {
		$activity_link = 'activity/' . $activity_node->nid . '/edit';
		unset($_REQUEST['destination']);
		unset($_GET['destination']);
		drupal_goto($activity_link);
	} else {
		include_once(drupal_get_path('module', 'node') . '/node.pages.inc');

		$output = "";

		if (!$teaser) {
			$output .= '<p><small>';
			switch ($activity_data['collaboration']) {
				case 'individual':
					$output .= t('This activity is carried out individually.');
					break;
				case 'group':
					$output .= t('This activity is carried out in groups.');
					break;
				default:
					$output .= t('This activity is done by all inquiry participants working together.');
					break;
			}
			$output .= '</small></p>';
		}

		$output .= '<p><b>' . nl2br(check_plain($activity_node->body)) . '</b></p>';


		if ($arg_node_function === 'view') {
			$content_output .= $content_node ? node_view($content_node, $teaser, TRUE, TRUE) : FALSE;
		} else if ($arg_node_function === 'edit') {
			$lternative_edit_page_function = "{$activity_node->activity_type}_edit_page";
			if (function_exists($lternative_edit_page_function)) {
				$content_output = $lternative_edit_page_function($content_node);
			} else {
				if (!$content_node) {
					$content_node = new stdClass;
					$content_node->uid = $access_data->get_uid();
					$content_node->type = $activity_node->activity_type;
				}
				$content_output .= node_page_edit($content_node);
			}
		}

		if ($content_output) {
			$output .= '<div style="padding: 5px; background: #eee; border: 1px dotted #aaa;">'
							. $content_output . '</div>';
		}

		if (!$teaser) {
			drupal_set_title(check_plain($activity_node->title));
		}
	}

	return $output;
}


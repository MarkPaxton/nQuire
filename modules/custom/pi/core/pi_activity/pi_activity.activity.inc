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

function pi_activity_content_delete($activity_node) {
	$content_nid = pi_activity_get_content_nid_for_activity($activity_node->nid);
	$node = node_load($content_nid);
	if ($node) {
		drupal_set_title(t("Delete @title", array('@title' => $node->title)));
		$output = drupal_get_form('pi_activity_content_delete_form', $activity_node, $node);
		return $output;
	}
}

function pi_activity_content_delete_form(&$form_state, $activity_node, $node) {
	$form = array();

	$info = t('<h2 class="title">Are you sure to delete <i>@title</i>?</h2>', array('@title' => $node->title))
					. '<div>' . t('This action cannot be undone.') . '</div>';

	$form['info'] = array(
			'#value' => $info,
	);

	nquire_commont_create_form_delete_cancel_buttons($form, 'pi_activity_content_delete_form_submit', 'phase/' . $activity_node->phase_nid);

	return $form;
}

function pi_activity_content_delete_form_submit($form, $form_state) {
	$activity_nid = arg(1);
	$content_nid = pi_activity_get_content_nid_for_activity($activity_nid);
	node_delete($content_nid);
	
	nquire_commons_form_close($form, $form_state);
}

function pi_activity_shared_content_list_view($activity_node) {


	$content_list = pi_activity_get_shared_content_for_activity_nid($activity_node->nid);
	$output = '';

	foreach ($content_list as $list_type => $nids) {
		switch ($list_type) {
			case 'groupmates':
				$intro = t('Content shared by members of my group:');
				break;
			case 'participants':
				$intro = t('Content shared by inquiry participants:');
				break;
			case 'groups':
				$intro = t('Content shared by other groups:');
				break;
		}

		$output .= '<p><b>' . $intro . '</b></p>';

		foreach ($nids as $actor_id => $nid) {
			$output .= pi_activity_display_shared_content_node($activity_node, $actor_id, node_load($nid), TRUE);
		}
	}

	return $output;
}

function pi_activity_display_shared_content_node($activity_node, $actor_id, $node, $teaser) {
	$access_data = pi_inquiry_groupings_get_access_data();
	$activity_data = $access_data->get_activity($activity_node->nid);
	if ($activity_data['collaboration'] === 'individual') {
		$user = user_load($actor_id);
		$actor_title = $user->name;
	} else {
		$group = node_load($actor_id);
		$actor_title = $group->title;
	}

	include_once(drupal_get_path('module', 'node') . '/node.pages.inc');

	if ($teaser) {
		$actor_text = l($actor_title, 'activity/' . $activity_node->nid . '/shared/' . $actor_id);
	} else {
		$actor_text = t('Content shared by @actor:', array('@actor' => $actor_title));
	}

	$output = $actor_text
					. '<div style="padding: 5px; background: #eee; border: 1px dotted #aaa;">'
					. ($node ? node_view($node, $teaser, TRUE, TRUE) : '')
					. '</div>';

	return $output;
}

function pi_activity_shared_content_view($activity_node, $actor_id, $teaser = FALSE) {
	$node = node_load(pi_activity_get_content_nid_for_activity_and_actor($activity_node->nid, $actor_id));
	return pi_activity_display_shared_content_node($activity_node, $actor_id, $node, $teaser);
}

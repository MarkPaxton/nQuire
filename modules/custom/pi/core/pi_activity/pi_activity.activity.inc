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
			$content_wrapper_function = "{$activity_node->activity_type}_wrap_content_node_view";
			if (!function_exists($content_wrapper_function) || $content_wrapper_function($activity_node)) {
				$output .= '<div style="padding: 5px; background: #eee; border: 1px dotted #aaa;">'
								. $content_output . '</div>';
			} else {
				$output .= $content_output;
			}
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
	global $user;
	$inquiry_info = pi_info()->getCurrentInquiryInfo();
	$inquiry_content = pi_info()->getContentManager($inquiry_info->getInquiryNid());

	$content_list = $inquiry_content->getActivitySharedContent($activity_node->nid, $user->uid);
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
			$output .= theme('pi_activity_display_shared_content_node', $inquiry_info, $activity_node, $actor_id, $nid, TRUE);
		}
	}

	return $output;
}

function pi_activity_shared_content_view($activity_node, $actor_id, $teaser = FALSE) {
	$inquiry_info = pi_info()->getCurrentInquiryInfo();
	$inquiry_content = pi_info()->getContentManager($inquiry_info->getInquiryNid());

	$content_nid = $inquiry_content->getContentForActor($activity_node->nid, $actor_id);
	return theme('pi_activity_display_shared_content_node', $inquiry_info, $activity_node, $actor_id, $content_nid, $teaser);
}

/**
 * 
 * @param \PiInquiryInfo $inquiry_info
 * @param type $activity_node
 * @param type $content_nid
 * @param type $user_link
 * @return string
 */
function theme_pi_activity_display_shared_content_node($inquiry_info, $activity_node, $actor_id, $content_nid, $teaser) {
	$node = node_load($content_nid);
	$collaboration = $inquiry_info->getActivityCollaboration($activity_node->nid);

	if ($collaboration === 'individual') {
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

	if ($node) {
		$shared_content_display_function = "{$node->type}_create_shared_view";
		if (function_exists($shared_content_display_function)) {
			$content_view = $shared_content_display_function($node);
		} else {
			$content_view = node_view($node, FALSE, TRUE, TRUE);
		}
	}

	$output = $actor_text
					. '<div style="padding: 5px; background: #eee; border: 1px dotted #aaa;">'
					. ($content_view ? $content_view : '')
					. '</div>';

	return $output;
}

function theme_pi_activity_display_contribution_content_node($activity_collaboration, $content_node, $content_actor, $viewer_collaboration, $contribution_data) {
	$output = '';

	if ($content_node) {

		if ($activity_collaboration !== $viewer_collaboration) {
			if ($activity_collaboration === 'individual') {
				$user = user_load($content_actor);
				$actor_title = $user->name;
			} else if ($activity_collaboration === 'group') {
				$group = node_load($content_actor);
				$actor_title = $group->title;
			}

			if ($actor_title) {
				$output .= t('Content created by @actor:', array('@actor' => $actor_title));
			}
		}

		include_once(drupal_get_path('module', 'node') . '/node.pages.inc');

		$contribution_content_display_function = "{$content_node->type}_create_contribution_view";
		if (function_exists($contribution_content_display_function)) {
			$content_view = $contribution_content_display_function($content_node, $contribution_data);
		} else {
			$content_view = node_view($content_node, FALSE, TRUE, TRUE);
		}

		$output = '<div style="padding: 5px; background: #eee; border: 1px dotted #aaa;">'
						. $content_view
						. '</div>';
	}
	return $output;
}

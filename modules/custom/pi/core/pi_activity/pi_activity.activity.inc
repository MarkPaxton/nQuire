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
	$output = "";

	global $user;
	$inquiry_info = pi_info()->getInquiryInfoForActivity($activity_node->nid);
	if ($inquiry_info) {
		$phase_node = $inquiry_info->getPhase($activity_node->phase_nid);
		$content_manager = pi_info()->getContentManager();
		$content_nid = $content_manager->getContent($activity_node->nid, $user->uid, TRUE);
		$content_node = node_load($content_nid);


		include_once(drupal_get_path('module', 'node') . '/node.pages.inc');


		if (!$teaser) {
			$output .= '<p><small>';
			switch ($phase_node->collaboration) {
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
		
		if ($arg_node_function === 'edit' && pi_activity_edit_as_view_mode($activity_node)) {
			$arg_node_function = 'view';
		}

		if ($arg_node_function === 'view') {
			$content_output .= $content_node ? node_view($content_node, $teaser, TRUE, TRUE) : FALSE;
		} else if ($arg_node_function === 'edit') {
			$lternative_edit_page_function = "{$activity_node->activity_type}_edit_page";
			if (function_exists($lternative_edit_page_function)) {
				$content_output = $lternative_edit_page_function($content_node);
			} else {
				if (!$content_node) {
					$content_node = new stdClass;
					$content_node->uid = $user->uid;
					$content_node->type = $activity_node->activity_type;
				}
				$content_output .= node_page_edit($content_node);
			}
		}

		if ($content_output) {
			if (!$teaser) {
				if ($activity_node->body) {
					$output .= '<p>' . nl2br(check_plain($activity_node->body)) . '</p>';
				}
				$output .= pi_activity_create_activity_view_activity_links($activity_node);
			}

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
	global $user;
	$content_nid = pi_info()->getContentManager()->getContent($activity_node->nid, $user->uid, FALSE);

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

	global $user;
	$content_nid = pi_info()->getContentManager()->getContent($activity_nid, $user->uid, FALSE);

	node_delete($content_nid);

	nquire_commons_form_close($form, $form_state);
}

function pi_activity_shared_content_list_view($activity_node) {
	global $user;
	$inquiry_info = pi_info()->getCurrentInquiryInfo();
	$inquiry_content = pi_info()->getContentManager();

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
	$inquiry_content = pi_info()->getContentManager();

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

function theme_pi_activity_access_explanation($access, $reason, $inquiry_nid) {

	switch ($reason) {
		case 'no_inquiry_access':
			$text = $reason;
			break;
		case 'inquiry_roles':
			switch ($access) {
				case 'edit':
					$text = t('Given your roles in this inquiry, you can edit this activity.');
					break;
				case 'view':
					$text = t('Given your roles in this inquiry, you can view this activity, but not edit it.');
					break;
				default:
					$text = t('Given your roles in this inquiry, you cannot access this activity.');
					break;
			}
			break;
		case 'group_roles':
			switch ($access) {
				case 'edit':
					$text = t('Given your roles in your group, you can edit this activity.');
					break;
				case 'view':
					$text = t('Given your roles in your group, you can view this activity, but not edit it.');
					break;
				default:
					$text = t('Given your roles in your group, you cannot access this activity.');
					break;
			}
			break;
		case 'no_group':
			$url = url('inquiry/' . $inquiry_nid . '/participants');
			$text = t('This is a group activity, but you do not belong to any group. Click <a href="!url">here</a> to create or join a group.', array('!url' => $url));
			break;
		case 'stage':
			switch ($access) {
				case 'edit':
					$text = t('At this stage, you can edit this activity.');
					break;
				case 'view':
					$text = t('At this stage, you can view this activity, but not edit it.');
					break;
				default:
					$text = t('At this stage, you cannot access this activity.');
					break;
			}
			break;
		case 'none':
			switch ($access) {
				case 'edit':
					$text = t('You can edit this activity.');
					break;
				case 'view':
					$text = t('You can view this activity.');
					break;
				default:
					$text = t('You cannot access this activity.');
					break;
			}
			break;
	}
	$output = "<p>$text</p>";
	return $output;
}

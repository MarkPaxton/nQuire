<?php

// $Id: pi.activity.inc, 2010/05/17 MCP $

/**
 * @file Provides view functions for pages under the url activity/*
 *
 */



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


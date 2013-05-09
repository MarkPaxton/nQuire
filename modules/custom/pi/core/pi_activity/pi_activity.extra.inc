<?php

/** @file pi_activity_extra.inc
 *  This is an include file that contains all the extra functions for pi_actitivy, this
 *  has been work in progress as some of the functions are old and to be removed - this is now
 *  mostly done, but some are still out of date/lacking checks
 */

/**
 * Returns the activity nid for a content item, if it has one.
 * Assumes only one activivity nid per item
 * @param uint $content_nid
 * @returns uint of an node id (unverified if it exits though) or NULL if no activity node found
 */
/* TOREMOVE */
function pi_activity_get_parent_activity($child_nid) {
	$query = "SELECT `parent_activity` FROM {pi_activity} WHERE `nid`=%d";
	$row = db_fetch_array(db_query($query, $child_nid));
	return $row ? $row['parent_activity'] : FALSE;
}

function pi_activity_build_link_for_child($parent_activity_nid, $child_activity_nid) {
	global $user;
	$content_manager = pi_info()->getContentManager();
	$access_manager = pi_info()->getAccess(pi_info()->getInquiryInfoForActivity($parent_activity_nid)->getInquiryNid(), $user->uid);
	
	$child_activity_node = node_load($child_activity_nid);

	$access = $access_manager->getAccess($parent_activity_nid);
	if ($access === 'hidden') {
		$image = 'unavailable.png';
	} else if ($content_manager->getContent($child_activity_nid, $user->uid, FALSE)) {
		$image = $access . 'able.png';
	} else {
		$image = 'creatable.png';
	}

	global $base_path;
	$image_path = $base_path . '/' . drupal_get_path('module', 'pi') . '/images/' . $image;

	$text = check_plain($child_activity_node->title) . '&nbsp;' . "<img width='12' src='" . $image_path . "'/>";

	if ($access !== 'hidden') {
		$node_function = $access;

		if ($node_function === 'edit') {
			if (pi_activity_tab_view_access_callback($child_activity_node)) {
				$node_function = 'view';
			}
		}
		$url = "activity/" . $child_activity_nid . "/" . $node_function;
		return l($text, $url, array('html' => TRUE));
	} else {
		return $text;
	}
}

function pi_activity_build_link($activity_nid) {
	global $user;
	$content_manager = pi_info()->getContentManager();
	$inquiry_info = pi_info()->getInquiryInfoForActivity($activity_nid);
	$inquiry_access = pi_info()->getAccess($inquiry_info->getInquiryNid(), $user->uid);
	$activity_node = $inquiry_info->getActivity($activity_nid);
	
	$content_exists = $content_manager->getContent($activity_nid, $user->uid, FALSE);
	
	$access = $inquiry_access->getAccess($activity_nid);
	switch ($access) {
		case 'edit':
			$image = 'editable.png';
			break;
		case 'view':
			$image = $content_exists ? 'viewable.png' : 'unavailable.png';
			break;
		default:
			$image = 'unavailable.png';
			break;
	}

	$image_path = url(drupal_get_path('module', 'pi') . '/images/' . $image);

	$text = check_plain($activity_node->title) . '&nbsp;' . "<img width='12' src='" . $image_path . "'/>";

	if ($access !== 'hidden') {
		$node_function = $access;

		if ($node_function === 'edit') {
			if (pi_activity_tab_view_access_callback($activity_node)) {
				$node_function = 'view';
			}
		}
		$url = "activity/" . $activity_nid . "/" . $node_function;
		return l($text, $url, array('html' => TRUE));
	} else {
		return $text;
	}
}


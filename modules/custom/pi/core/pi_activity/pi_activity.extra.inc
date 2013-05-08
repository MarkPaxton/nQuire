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



function pi_activity_get_parent_activity($child_nid) {
	$query = "SELECT `parent_activity` FROM {pi_activity} WHERE `nid`=%d";
	$row = db_fetch_array(db_query($query, $child_nid));
	return $row ? $row['parent_activity'] : FALSE;
}

function pi_activity_build_link_for_child($parent_activity_nid, $child_activity_nid) {
	$access_data = pi_inquiry_groupings_get_access_data();
	$activity_data = $access_data->get_activity($parent_activity_nid);
	$child_activity_node = node_load($child_activity_nid);


	if (!$activity_data['access']) {
		$image = 'unavailable.png';
	} else if (pi_activity_get_content_nid_for_activity($child_activity_nid)) {
		$image = $activity_data['access'] . 'able.png';
	} else {
		$image = 'creatable.png';
	}

	global $base_path;
	$image_path = $base_path . '/' . drupal_get_path('module', 'pi') . '/images/' . $image;

	$text = check_plain($child_activity_node->title) . '&nbsp;' . "<img width='12' src='" . $image_path . "'/>";

	if ($activity_data['access']) {
		$node_function = $activity_data['access'];

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
	$access_data = pi_inquiry_groupings_get_access_data();
	$activity_data = $access_data->get_activity($activity_nid);
	$activity_node = $activity_data['node'];

	$content_exists = pi_activity_get_content_nid_for_activity($activity_nid);

	switch ($activity_data['access']) {
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

	global $base_path;
	$image_path = $base_path . '/' . drupal_get_path('module', 'pi') . '/images/' . $image;

	$text = check_plain($activity_node->title) . '&nbsp;' . "<img width='12' src='" . $image_path . "'/>";

	if ($activity_data['access']) {
		$node_function = $activity_data['access'];

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

/**
 * Generates add activity link
 */
function make_add_activity_link($activity_id) {
	$access_data = pi_inquiry_groupings_get_access_data();
	$activity_data = $access_data->get_activity($activity_id);

	if ($activity_data['access'] === 'edit') {

		$add_activity_id = pi_activity_get_add_activity_for_sort_activity($activity_id);

		if ($add_activity_id) {
			$activity_link = pi_activity_build_link_for_child($activity_id, $add_activity_id);
			$add_activity_link = '<div class="add_activity_link">' . $activity_link . '</div>';
		}

		return $add_activity_link;
	} else {
		return '';
	}
}
?>
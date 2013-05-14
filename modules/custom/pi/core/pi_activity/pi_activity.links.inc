<?php

function pi_activity_create_phase_view_activity_links($activity_node) {
	$inquiry_info = pi_info()->getCurrentInquiryInfo();
	if ($inquiry_info) {
		global $user;
		$access_data = pi_info()->getAccessManager($inquiry_info->getInquiryNid(), $user->uid);
		$content_manager = pi_info()->getContentManager();

		$has_content = $content_manager->getContent($activity_node->nid, $user->uid, FALSE);
		$activity_access = $access_data->getAccessToItem($activity_node);
		$can_view = pi_activity_tab_view_access_callback($activity_node) && (($activity_access === 'edit' && !pi_activity_edit_as_view_mode($activity_node)) || $activity_access === 'view');
		$can_edit = pi_activity_tab_edit_access_callback($activity_node) && $activity_access === 'edit';

		$links = array();
		if ($can_view && $has_content) {
			$links[] = array('url' => url("activity/{$activity_node->nid}/view"), 'label' => t('View'), 'image' => 'viewable.png');
		}

		if ($can_edit) {
			if ($has_content) {
				$links[] = array('url' => url("activity/{$activity_node->nid}/edit"), 'label' => t('Edit'), 'image' => 'editable.png');
			} else {
				$type = node_get_types('type', $activity_node->activity_type);
				$node_function = isset($type->add_node_type) ? 'view' : 'edit';
				$links[] = array('url' => url("activity/{$activity_node->nid}/$node_function"), 'label' => t('Create'), 'image' => 'editable.png');
			}
		}
	}

	return theme('pi_activity_links', $links);
}

function pi_activity_create_activity_view_activity_links($activity_node) {
	$inquiry_info = pi_info()->getCurrentInquiryInfo();
	if ($inquiry_info) {
		global $user;
		$access_data = pi_info()->getAccessManager($inquiry_info->getInquiryNid(), $user->uid);

		$activity_access = $access_data->getAccessToItem($activity_node);
		$links = array();

		$add_activity_nid = pi_activity_get_add_activity_for_sort_activity($activity_node->nid);
		if ($add_activity_nid && $activity_access === 'edit') {
			$add_activity = node_load($add_activity_nid);
			$links[] = array('url' => url("activity/$add_activity_nid/edit"), 'label' => check_plain($add_activity->title), 'image' => 'add.png');
		}
	}

	return theme('pi_activity_links', $links);
}

function _pi_activity_links_format_link($link) {
	$image_path = url(drupal_get_path('module', 'pi') . '/images/' . $link['image']);
	return "<a href='{$link['url']}'><b>{$link['label']}</b>&nbsp<img width='12' src='$image_path'/></a>";
}

function theme_pi_activity_links($links) {
	if ($links && count($links) > 0) {
		$output = '<p>'
						. implode('&nbsp;&nbsp;&nbsp;', array_map(_pi_activity_links_format_link, $links))
						. '</p>';
	} else {
		$output = '';
	}
	return $output;
}
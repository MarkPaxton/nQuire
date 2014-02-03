<?php

function pi_activity_create_edit_links($activity_node, $edit_view) {
	$inquiry_info = pi_info()->getCurrentInquiryInfo();
	if ($inquiry_info) {
		global $user;
		$access_data = pi_info()->getAccessManager($inquiry_info->getInquiryNid(), $user->uid);
		$content_manager = pi_info()->getContentManager();

		$content_node = node_load($content_manager->getContent($activity_node->nid, $user->uid, FALSE));
		
		$activity_access = $access_data->getAccessToItem($activity_node);
		$can_edit = pi_activity_tab_edit_access_callback($activity_node) && $activity_access === 'edit';

		$links = array();



		if ($can_edit) {      
			if ($content_node) {
				$add_content_link_f = "{$activity_node->activity_type}_add_content_label";
				$add_content_link_label = function_exists($add_content_link_f) ? $add_content_link_f($content_node) : FALSE;

				if ($add_content_link_label) {
					$links[] = array('url' => url("activity/{$activity_node->nid}/add", array('fragment' => $activity_node->nid)), 'label' => $add_content_link_label, 'image' => 'addable.png');
				}

				if (!$edit_view) {
					$edit_content_link_f = "{$activity_node->activity_type}_edit_content_label";
					$edit_content_link_label = function_exists($edit_content_link_f) ? $edit_content_link_f($content_node) : FALSE;
					if ($edit_content_link_label) {
						$links[] = array('url' => url("activity/{$activity_node->nid}", array('fragment' => $activity_node->nid)), 'label' => $edit_content_link_label, 'image' => 'editable.png');
					}
				}
			} else {
				if (!$edit_view) {
					$links[] = array('url' => url("activity/{$activity_node->nid}", array('fragment' => $activity_node->nid)), 'label' => t('Create'), 'image' => 'editable.png');
				}
			}
		}
	}

	return theme('pi_activity_links', $links);
}

function _pi_activity_links_format_link($link) {
	$image_path = url(drupal_get_path('module', 'pi') . '/images/' . $link['image']);
	return "<a class='nquire_button_link nquire_button_link_activity_work' href='{$link['url']}'><b>{$link['label']}</b>&nbsp<img width='12' src='$image_path'/></a>";
}

function theme_pi_activity_links($links) {
	if ($links && count($links) > 0) {
		$output = implode('&nbsp;&nbsp;&nbsp;', array_map(_pi_activity_links_format_link, $links));
	} else {
		$output = '';
	}
	return $output;
}
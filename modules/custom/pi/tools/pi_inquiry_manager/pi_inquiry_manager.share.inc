<?php

function pi_inquiry_manager_share_content() {
	$access_data = pi_inquiry_groupings_get_access_data();
	if ($access_data->get_inquiry_node()) {
		$output = drupal_get_form('pi_inquiry_manager_share_content_form');
		return $output;
	}
}

function pi_inquiry_manager_share_content_form(&$form_state) {
	$form = array();

	$structure = nquire_structure_widget_access_data_2_structure();
	$access_data = pi_inquiry_groupings_get_access_data();
	$share_policy = $access_data->get_user_share_policy();

	nquire_structure_widget_share_content_table_create_form_item($form, 'share_content', $share_policy['structure'], json_encode($share_policy['structure']['values']));

	nquire_commons_create_form_save_cancel_buttons($form, 'pi_inquiry_manager_share_content_form_submit', drupal_get_destination());
}

function pi_inquiry_manager_share_content_form_submit(&$form_state, $inquiry_node) {
	
}

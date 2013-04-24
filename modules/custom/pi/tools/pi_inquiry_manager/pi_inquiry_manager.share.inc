<?php

function pi_inquiry_manager_share() {
	$access_data = pi_inquiry_groupings_get_access_data();
	if ($access_data->get_inquiry_node()) {
		$output = drupal_get_form('pi_inquiry_manager_share_content_form');
		return $output;
	}
}

function pi_inquiry_manager_share_content_form(&$form_state) {
	$form = array();

	$access_data = pi_inquiry_groupings_get_access_data();
	$share_policy = $access_data->get_user_share_policy();

	nquire_structure_widget_share_content_table_create_form_item($form, 'sharecontent', json_encode($share_policy['structure']), json_encode(array(array('id' => 'share', 'title' => t('Share with'), 'values' => $share_policy['values']))));

	nquire_commons_create_form_save_cancel_buttons($form, 'pi_inquiry_manager_share_content_form_submit', 'inquiry/' . $access_data->get_inquiry_nid() . '/share');

	return $form;
}

function pi_inquiry_manager_share_content_form_submit($form, $form_state) {
	$sc = nquire_commons_decode_data($form_state['values']['sharecontent']);
	if (isset($sc[0]['values'])) {
		$access_data = pi_inquiry_groupings_get_access_data();
		$query = "INSERT INTO pi_activity_share (activity_id, actor, share) VALUES (%d, %d, '%s') ON DUPLICATE KEY UPDATE share='%s'";
		foreach ($sc[0]['values'] as $anid => $value) {
			$activity_data = $access_data->get_activity($anid);
			if ($activity_data) {
				switch ($activity_data['collaboration']) {
					case 'individual':
						db_query($query, $anid, $access_data->get_uid(), $value, $value);
						break;
					case 'group':
						if ($access_data->user_is_subgroup_owner()) {
							db_query($query, $anid, $access_data->get_subgroup(), $value, $value);
						}
						break;
					default:
						if ($access_data->user_is_inquiry_owner()) {
							db_query($query, $anid, $access_data->get_inquiry_node()->actor_gid, $value, $value);
						}
						break;
				}
			}
		}
	}
	return;
}

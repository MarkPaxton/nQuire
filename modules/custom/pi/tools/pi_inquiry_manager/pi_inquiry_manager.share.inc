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

	$form['introinquiry'] = array(
			'#value' => '<h3>' . t('Share content with inquiry participants') . '</h3>',
	);

	nquire_structure_widget_share_content_table_create_form_item($form, 'sharecontent', json_encode($share_policy['structure']), json_encode(array(array('id' => 'share', 'title' => t('Share with'), 'values' => $share_policy['values']))));

	$form['introsystem'] = array(
			'#value' => '<h3>' . t('Publish investigation results in nQuire') . '</h3>' . theme('pi_journal_share_information', $access_data->get_inquiry_node()),
	);

	if (pi_journal_edit_share_access($access_data->get_inquiry_node())) {
		$form['publishresults'] = array(
				'#type' => 'checkbox',
				'#title' => t('Publish investigation'),
				'#description' => t('Select whether you want your inquiry results to be public in the !link.', array('!link' => l(t('nQuire Journal'), 'journal'))),
				'#required' => FALSE,
				'#default_value' => pi_journal_is_shared_for_user($access_data->get_inquiry_node(), $access_data->get_uid()),
				'#attributes' => array('class' => 'structure_additional_form_item'),
		);
	}


	nquire_commons_create_form_save_cancel_buttons($form, 'pi_inquiry_manager_share_content_form_submit', 'inquiry/' . $access_data->get_inquiry_nid() . '/share');

	return $form;
}

function pi_inquiry_manager_share_content_form_submit($form, $form_state) {
		$access_data = pi_inquiry_groupings_get_access_data();
	
		$sc = nquire_commons_decode_data($form_state['values']['sharecontent']);
	if (isset($sc[0]['values'])) {
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
	
	if (isset($form_state['values']['publishresults']) && pi_journal_edit_share_access($access_data->get_inquiry_node())) {
		pi_journal_edit_share_investigation($access_data->get_inquiry_node(), $access_data->get_uid(), $form_state['values']['publishresults']);
	}
	
	return;
}

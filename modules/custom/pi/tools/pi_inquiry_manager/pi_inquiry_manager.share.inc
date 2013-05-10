<?php

function pi_inquiry_manager_share() {
	if (pi_info()->getCurrentPath()->inquiry_nid) {
		$output = drupal_get_form('pi_inquiry_manager_share_content_form');
		return $output;
	}
}

function pi_inquiry_manager_share_content_form(&$form_state) {
	$form = array();

	$inquiry_info = pi_info()->getCurrentInquiryInfo();
	if ($inquiry_info) {
		global $user;

		$share_policy = $inquiry_info->getUserSharePolicy($user->uid);

		$form['introinquiry'] = array(
				'#value' => '<h3>' . t('Share content with inquiry participants') . '</h3>',
		);

		nquire_structure_widget_share_content_table_create_form_item($form, 'sharecontent', json_encode($share_policy['structure']), json_encode(array(array('id' => 'share', 'title' => t('Share with'), 'values' => $share_policy['values']))));

		$form['introsystem'] = array(
				'#value' => '<h3>' . t('Publish investigation results in nQuire') . '</h3>' . theme('pi_journal_share_information', $inquiry_info->getInquiryNode()),
		);

		if (pi_journal_edit_share_access($inquiry_info->getInquiryNode())) {
			$form['publishresults'] = array(
					'#type' => 'checkbox',
					'#title' => t('Publish investigation'),
					'#description' => t('Select whether you want your inquiry results to be public in the !link.', array('!link' => l(t('nQuire Journal'), 'journal'))),
					'#required' => FALSE,
					'#default_value' => pi_journal_is_shared_for_user($inquiry_info->getInquiryNode(), $user->uid),
					'#attributes' => array('class' => 'structure_additional_form_item'),
			);
		}


		nquire_commons_create_form_save_cancel_buttons($form, 'pi_inquiry_manager_share_content_form_submit', 'inquiry/' . $inquiry_info->getInquiryNid() . '/share');
	}
	return $form;
}

function pi_inquiry_manager_share_content_form_submit($form, $form_state) {
	global $user;
	$inquiry_info = pi_info()->getCurrentInquiryInfo();
	$access_manager = pi_info()->getAccessManager($inquiry_info->getInquiryNid(), $user->uid);

	$sc = nquire_commons_decode_data($form_state['values']['sharecontent']);
	if (isset($sc[0]['values'])) {
		$query = "INSERT INTO pi_activity_share (activity_id, actor, share) VALUES (%d, %d, '%s') ON DUPLICATE KEY UPDATE share='%s'";
		foreach ($sc[0]['values'] as $anid => $value) {
			$activity = $inquiry_info->getActivity($anid);
			if ($activity) {
				switch ($inquiry_info->getActivityCollaboration($anid)) {
					case 'individual':
						db_query($query, $anid, $user->uid, $value, $value);
						break;
					case 'group':
						if ($access_manager->isGroupOwner()) {
							db_query($query, $anid, $access_manager->getSubgroup(), $value, $value);
						}
						break;
					default:
						if ($access_manager->isInquiryOwner()) {
							db_query($query, $anid, $inquiry_info->getInquiryActorGid(), $value, $value);
						}
						break;
				}
			}
		}
	}

	if (isset($form_state['values']['publishresults']) && pi_journal_edit_share_access($inquiry_info->getInquiryNode())) {
		pi_journal_edit_share_investigation($inquiry_info->getInquiryNode(), $user->uid, $form_state['values']['publishresults']);
	}

	return;
}

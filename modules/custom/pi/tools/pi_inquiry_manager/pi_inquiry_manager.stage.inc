<?php

function pi_inquiry_manager_stage_selector() {
	global $user;
	$inquiry_info = pi_info()->getCurrentInquiryInfo();
	if ($inquiry_info) {
		$access = pi_info()->getAccessManager($inquiry_info->getInquiryNid(), $user->uid);
		if ($access->isInquiryOwner()) {
			$output = drupal_get_form('pi_inquiry_manager_stage_selector_form');
		} else {
			$output = pi_inquiry_manager_stage_selector_view();
		}
	}
	return $output;
}

function pi_inquiry_manager_stage_selector_view() {
	$output = '';

	$inquiry_info = pi_info()->getCurrentInquiryInfo();
	if ($inquiry_info) {
		$all_stages_structure = $inquiry_info->getStageStructure();
		$current_stage = $inquiry_info->getCurrentStage()->nid;

		_pi_inquiry_manager_stage_selector_load_javascript($all_stages_structure['values'], $current_stage);

		$output .= theme('nquire_structure_widget_stage_view', 'stageselector', json_encode($all_stages_structure['structure']));
	}
	return $output;
}

function pi_inquiry_manager_stage_selector_form(&$form_state) {
	$form = array();

	$inquiry_info = pi_info()->getCurrentInquiryInfo();
	if ($inquiry_info) {
		$all_stages_structure = $inquiry_info->getStageStructure();
		$current_stage = $inquiry_info->getCurrentStage()->nid;

		nquire_structure_widget_stage_selector_create_form_item($form, 'stageselector', json_encode($all_stages_structure['structure']), $current_stage);

		_pi_inquiry_manager_stage_selector_load_javascript($all_stages_structure['values']);

		nquire_commons_create_form_save_cancel_buttons($form, 'pi_inquiry_manager_stage_selector_form_submit', 'inquiry/' . $inquiry_info->getInquiryNid() . '/share');
	}

	return $form;
}

function _pi_inquiry_manager_stage_selector_load_javascript($stage_phase_values, $highlight_column = NULL) {
	$stage_phase_str = json_encode($stage_phase_values);
	$module = "{"
					. "  init: function() {"
					. "    this.stages = $stage_phase_str;"
					. "    this.highlightColumn = '$highlight_column';"
					. "  }"
					. "}";
	nquire_commons_inject_inline_javascript_module('InquiryStagePhaseValues', $module);
}

function pi_inquiry_manager_stage_selector_form_submit($form, $form_state) {
	$inquiry_info = pi_info()->getCurrentInquiryInfo();
	if ($inquiry_info) {
		global $user;
		$access_data = pi_info()->getAccessManager($inquiry_info->getInquiryNid(), $user->uid);
		if ($access_data->isInquiryOwner()) {
			$value = $form_state['values']['stageselector'];
			$query = "INSERT INTO pi_stage_current (inquiry_id, stage_id) VALUES (%d, %d) ON DUPLICATE KEY UPDATE stage_id=%d";
			db_query($query, $inquiry_info->getInquiryNid(), $value, $value);
		}
	}
}

<?php

function inquiry_basic_activities_update_toolform() {
  $new_form = nquire_commons_rebuild_ahah_form();

  $choice_form = $new_form['data']['tool_form'];
  $output = drupal_render($choice_form);
  
  drupal_json(array('status' => TRUE, 'data' => $output));
}

function _inquiry_basic_activities_tool_form(&$form, $form_state, $activity_data, $title, $description) {
 $tool_value = nquire_commons_form_data_default_value($activity_data, 'tool.type', $form_state, 'tool_type', 'none');

  $form['data']['tool_type'] = array(
      '#type' => 'select',
      '#title' => $title,
      '#prefix' => '<div>',
      '#suffix' => '</div>',
      '#description' => $description,
      '#options' => array(
          'none' => t('- No instrument'),
          'vm' => t('Virtual microscope'),
          'treezilla' => t('Treezilla'),
          'ispot' => t('iSpot'),
          'pirate' => t('Pirate remote telescope'),
          'fits' => t('Astronomy image processing software'),
      ),
      '#default_value' => $tool_value,
      '#ahah' => array(
          'path' => 'creator/toolform/',
          'wrapper' => 'instrument-form-container',
          'method' => 'replace',
          'effect' => 'fade',
      ),
  );

  $form['data']['tool_form'] = array();
  
  $form['data']['tool_form'][] = array(
      '#value' => '<div id="instrument-form-container">',
  );
  
  if ($tool_value !== 'none') {
    inquiry_available_tools_invoke('form', $tool_value, $activity_data['tool'], $form['data']['tool_form'], $form_state);
  }
  
  $form['data']['tool_form'][] = array(
      '#value' => '</div>',
  );
}


function _inquiry_basic_activities_tool_form_process(&$activity, $design, $form_values) {
  $type = $form_values['tool_type'];
  
  $data = array();
  $data['tool'] = array();
  $data['tool']['type'] = $type;
  
  if ($type !== 'none') {
    inquiry_available_tools_invoke('form_process', $type, $data['tool'], $form_values);
  }

  $activity->data = json_encode($data);
}
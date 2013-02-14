<?php

function _inquiry_basic_activities_designer_tool_form(&$form, $form_state, $activity_data, $title, $description, $measures_form = FALSE, $base_path = NULL) {
  $tool_value = nquire_commons_form_data_default_value($activity_data, 'tool.type', $form_state, 'tool_type', 'none');
  
  $options = array('none' => t('-- No instrument')) + inquiry_available_features_list('tool');
  
  $form['data']['tool_type'] = array(
      '#type' => 'select',
      '#title' => $title,
      '#description' => $description,
      '#options' => $options,
      '#default_value' => $tool_value,
      '#ahah' => array(
          'path' => ahah_helper_path(array('data', 'tool_form')),
          'wrapper' => 'instrument-dependent-section-container',
          'method' => 'replace',
          'effect' => 'fade',
      ),
  );

  $form['data']['tool_form'] = array();

  $form['data']['tool_form'][] = array(
      '#value' => '<div id="instrument-dependent-section-container">',
  );

  if ($tool_value !== 'none') {
    inquiry_available_features_invoke('tool', 'form', $tool_value, $activity_data['tool'], $form['data']['tool_form'], $form_state);
  }

  if ($measures_form) {
    inquiry_creator_add_measures_form_links($activity_data['measures'], $tool_value, $form['data']['tool_form'], $base_path);
  }

  $form['data']['tool_form'][] = array(
      '#value' => '</div>',
  );

  return $tool_value;
}

function _inquiry_basic_activities_designer_tool_form_process(&$activity, $design, $form_values) {
  $type = $form_values['tool_type'];

  $data = array();
  $data['tool'] = array();
  $data['tool']['type'] = $type;

  if ($type !== 'none') {
    inquiry_available_features_invoke('tool', 'form_process', $type, $data['tool'], $form_values);
  }

  $old_data = nquire_commons_decode_data($activity->data);
  if (isset($old_data['measures'])) {
    $data['measures'] = $old_data['measures'];
  }

  $new_data = json_encode($data);
  if ($activity->data !== $new_data) {
    $activity->data = $new_data;
    return TRUE;
  } else {
    return FALSE;
  }
}


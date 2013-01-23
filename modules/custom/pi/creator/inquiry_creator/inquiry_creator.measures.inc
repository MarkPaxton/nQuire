<?php

function _inquiry_creator_add_measures_form_links($measures_data, &$form, $form_state, $current_tool) {
  $value = '<div>';

  if (count($measures_data) > 0) {
    $value .= '<ul>';
    foreach ($measures_data as $measure) {
      $value .= '<li>' . $measure['title'] . '</li>';
    }
    $value .= '</ul>';
  } else {
    $value .= '<p><i>' . t('There are no measures in this activity. Click <b>Add new measure</b> to start creating measures.') . '</i></p>';
  }

  $value .= l(t('Add new measure'), $_GET['q'] . '/measure/add');

  $form['measures'] = array(
      '#type' => 'fieldset',
      '#title' => t('Available measures'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#value' => $value,
  );
}

function inquiry_creator_measure_add($design_nid, $activity_nid) {
  $design = node_load($design_nid);
  $node = node_load($activity_nid);

  $output = theme('inquiry_creator_inquiry_main', $design, 'activities');

  $form = drupal_get_form('inquiry_creator_measure_form', $design, $node, NULL);

  $output .= theme('inquiry_creator_side_form', $design, $activity_nid, 'measure', $form);
  return $output;
}

function inquiry_creator_measure_edit($design_nid, $activity_nid, $measure_nid) {
  
}

function inquiry_creator_measure_form(&$form_state, $design, $activity, $measure_nid) {
  $form = array();

  module_load_include('inc', 'inquiry_creator', 'inquiry_creator.edit');
  module_load_include('inc', 'inquiry_creator', 'inquiry_creator.activities');
  _inquiry_creator_create_inquiry_id_form_item($design, $form);
  _inquiry_creator_create_activity_id_form_item($activity, $form);

  $data = nquire_commons_decode_data($activity->data);

  $form['title'] = array(
      '#type' => 'textfield',
      '#title' => t('Measure name'),
      '#description' => t('Write here the name that will identify the measure.'),
      '#required' => TRUE,
      '#default_value' => nquire_commons_form_data_default_value($data, "measures.$measure_nid.title", $form_state, 'title', ''),
  );

  //Show a simple textarea, no input selector required....
  $form['description'] = array(
      '#type' => 'textarea',
      '#title' => t('Measure description'),
      '#description' => t('Write here the description of the measure. This could include, for instance, an explanation of how to use this measure.'),
      '#default_value' => nquire_commons_form_data_default_value($data, "measures.$measure_nid.description", $form_state, 'description', ''),
      '#required' => TRUE
  );
  
  return $form;
}

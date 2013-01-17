<?php

function inquiry_creator_create_phase_stub($title) {
  $node = new stdClass();
  $node->type = 'inquiry_phase';
  $node->title = $title;

  node_save($node);
  return $node;
}

function inquiry_creator_phase_edit($design_nid, $phase_nid) {
  $design = node_load($design_nid);
  $node = node_load($phase_nid);

  $output = theme('inquiry_creator_inquiry_main', $design, 'activities');

  $form = drupal_get_form('inquiry_creator_phase_form', $design, $node);

  $output .= theme('inquiry_creator_side_form', $design, $phase_nid, 'phase', $form);
  return $output;
}

function inquiry_creator_phase_form(&$form_state, $design, $node) {
  $form = array();

  _inquiry_creator_create_inquiry_id_form_item($design, $form);
  $form['phase_id'] = array(
      '#type' => 'hidden',
      '#default_value' => $node->nid,
      '#required' => TRUE,
  );

  $type = node_get_types('type', $node);

  $form['title'] = array(
      '#type' => 'textfield',
      '#title' => check_plain($type->title_label),
      '#description' => t('Write here the name for the phase.'),
      '#required' => TRUE,
      '#default_value' => $node->title,
  );

  //Show a simple textarea, no input selector required....
  $form['body_filter']['body'] = array(
      '#type' => 'textarea',
      '#title' => check_plain($type->body_label),
      '#description' => t('A brief description of the phase objectives. This will help the inquiry participants understand their tasks.'),
      '#default_value' => $node->body,
      '#required' => TRUE
  );

  _inquiry_creator_create_form_buttons('inquiry_creator_phase_form_submit', $form, $design);

  return $form;
}

function inquiry_creator_phase_form_submit($form, $form_state) {
  $design = node_load($form_state['values']['inquiry_design']);
  $phase = node_load($form_state['values']['phase_id']);
  $title = $form_state['values']['title'];

  if ($design && $phase && inquiry_design_set_item_title($design, $phase->nid, $title)) {
    $phase->title = $form_state['values']['title'];
    $phase->body = $form_state['values']['body'];

    node_save($phase);
    node_save($design);
  }

  inquiry_creator_form_cancel($form, $form_state);
}
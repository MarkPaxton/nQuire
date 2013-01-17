<?php


function inquiry_creator_activity_edit($design_nid, $activity_nid) {
  $design = node_load($design_nid);
  $node = node_load($activity_nid);

  $output = theme('inquiry_creator_inquiry_main', $design, 'activities');
  
  $form = drupal_get_form('inquiry_creator_activity_form', $design, $node);
  
  $output .= theme('inquiry_creator_side_form', $design, $activity_nid, 'activity', $form);
  return $output;
}


function inquiry_creator_create_activity_stub($type, $title) {
  $node = new stdClass();
  $node->type = 'inquiry_activity';
  $node->activity_type = $type;
  $node->title = $title;

  node_save($node);
  return $node;
}

function inquiry_creator_activity_form(&$form_state, $design, $node) {
  $form = array();

  _inquiry_creator_create_inquiry_id_form_item($design, $form);
  $form['activity_id'] = array(
      '#type' => 'hidden',
      '#default_value' => $node->nid,
      '#required' => TRUE,
  );

  $type = node_get_types('type', $node);

  $form['title'] = array(
      '#type' => 'textfield',
      '#title' => check_plain($type->title_label),
      '#description' => t('Write here the activity title.'),
      '#required' => TRUE,
      '#default_value' => $node->title,
  );

  //Show a simple textarea, no input selector required....
  $form['body_filter']['body'] = array(
      '#type' => 'textarea',
      '#title' => check_plain($type->body_label),
      '#description' => t('Write here the instructions for the inquiry participants. This should describe their tasks in this activity.'),
      '#default_value' => $node->body,
      '#required' => TRUE
  );
  
  inquiry_available_activities_invoke('form', $node, $design, $form);

  _inquiry_creator_create_form_buttons('inquiry_creator_activity_form_submit', $form, $design);

  return $form;
}

function inquiry_creator_activity_form_submit($form, $form_state) {
  $design = node_load($form_state['values']['inquiry_design']);
  $activity = node_load($form_state['values']['activity_id']);
  $title = $form_state['values']['title'];

  if ($design && $activity && inquiry_design_set_item_title($design, $activity->nid, $title)) {
    $activity->title = $form_state['values']['title'];
    $activity->body = $form_state['values']['body'];
    
    inquiry_available_activities_invoke('form_process', $activity, $design, $form_state['values']['data']);
    
    node_save($activity);
    node_save($design);
  }

  inquiry_creator_form_cancel($form, $form_state);
}

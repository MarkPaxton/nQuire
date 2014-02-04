<?php

function inquiry_creator_phase_edit($design_nid, $phase_nid) {
  $design = node_load($design_nid);
  $node = node_load($phase_nid);

  $form = drupal_get_form('inquiry_creator_phase_form', $design, $node);
  $content = theme('inquiry_creator_side_form', $design, $phase_nid, 'phase', $form);

  $output = theme('inquiry_creator_inquiry_main', $design, 'activities', $content);
  return $output;
}

function _inquiry_creator_create_phase_id_form_item($phase, &$form) {
  $form['phase_id'] = array(
      '#type' => 'hidden',
      '#default_value' => $phase->nid,
      '#required' => TRUE,
  );
}

function inquiry_creator_phase_form(&$form_state, $design, $node) {
  $form = array();

  nquire_commons_form_set_default_link_action($form, 'submit');

  _inquiry_creator_create_inquiry_id_form_item($design, $form);
  _inquiry_creator_create_phase_id_form_item($node, $form);

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
      '#required' => FALSE,
  );

  nquire_commons_create_form_save_cancel_buttons($form, 'inquiry_creator_phase_form_submit', "creator/{$design->nid}/activities");

  return $form;
}

function inquiry_creator_phase_form_submit($form, $form_state) {
  global $user;
  $phase = node_load($form_state['values']['phase_id']);

  $inquiry_info = pi_info()->getInquiryInfo($phase->inquiry_nid);
  if ($inquiry_info->userIsInquiryAdmin($user->uid)) {
    $changed = FALSE;

    if ($phase->title !== $form_state['values']['title']) {
      $phase->title = $form_state['values']['title'];
      $changed = TRUE;
    }

    if ($phase->body !== $form_state['values']['body']) {
      $phase->body = $form_state['values']['body'];
      $changed = TRUE;
    }

    if ($changed) {

      $phase->revision = TRUE;
      node_save($phase);
    }
  }

  nquire_commons_form_close($form, $form_state);
}

function inquiry_creator_phase_delete($design_nid, $phase_nid) {
  $design = node_load($design_nid);
  $node = node_load($phase_nid);


  $form = drupal_get_form('inquiry_creator_phase_delete_form', $design, $node);
  $content = theme('inquiry_creator_side_form', $design, $phase_nid, 'phase', $form);

  $output = theme('inquiry_creator_inquiry_main', $design, 'activities', $content);
  return $output;
}

function inquiry_creator_phase_delete_form(&$form_state, $design, $node) {
  $form = array();

  _inquiry_creator_create_inquiry_id_form_item($design, $form);
  _inquiry_creator_create_phase_id_form_item($node, $form);

  $info = t('<h1 class="title">Are you sure to delete phase <i>@title</i>?</h1>', array('@title' => $node->title));

  $inquiry_info = pi_info()->getInquiryInfo($design->nid);
  $activities = $inquiry_info->getActivitiesForPhase($node->nid);
  if ($activities && count($activities > 0)) {
    $info .= '<p>' . t('The following activities will also be deleted:') . '<ul>';
    foreach ($activities as $activity) {
      $info .= '<li><b>' . check_plain($activity->title) . '</b></li>';
    }
    $info .= '</ul></p>';
  }

  $info .= '<div>' . t('This action cannot be undone.') . '</div>';


  $form['info'] = array(
      '#value' => $info,
  );

  nquire_commont_create_form_delete_cancel_buttons($form, 'inquiry_creator_phase_delete_form_submit', "creator/{$design->nid}/activities");

  return $form;
}

function inquiry_creator_phase_delete_form_submit($form, $form_state) {
  global $user;

  $design = node_load($form_state['values']['inquiry_design']);
  $phase = node_load($form_state['values']['phase_id']);

  $inquiry_info = pi_info()->getInquiryInfo($phase->inquiry_nid);
  if ($inquiry_info->userIsInquiryAdmin($user->uid)) {

    $phase_nid = $phase->nid;

    $inquiry_info = pi_info()->getInquiryInfo($design->nid);
    $activities = $inquiry_info->getActivitiesForPhase($phase_nid);
    foreach ($activities as $activity) {
      module_load_include('inc', 'inquiry_creator', 'inquiry_creator.activities');
      _inquiry_creator_activity_delete_node($activity->nid);
    }

    node_delete($phase_nid);
  }

  nquire_commons_form_close($form, $form_state);
}

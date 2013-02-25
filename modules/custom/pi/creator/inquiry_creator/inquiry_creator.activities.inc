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
  global $user;

  $node = new stdClass();
  $node->type = 'inquiry_activity';
  $node->activity_type = $type;
  $node->title = $title;
  $node->uid = $user->uid;

  node_save($node);
  return $node;
}

function _inquiry_creator_create_activity_id_form_item($activity, &$form) {
  $form['activity_id'] = array(
      '#type' => 'hidden',
      '#default_value' => $activity->nid,
      '#required' => TRUE,
  );
}

function _inquiry_creator_activity_form(&$form_state, $design, $node) {
  $form = array();

  ahah_helper_register($form, $form_state);

  nquire_commons_form_set_default_link_action($form, 'submit');

  module_load_include('inc', 'inquiry_creator', 'inquiry_creator.edit');
  _inquiry_creator_create_inquiry_id_form_item($design, $form);
  _inquiry_creator_create_activity_id_form_item($node, $form);

  $type = node_get_types('type', $node);

  $form['title'] = array(
      '#type' => 'textfield',
      '#title' => check_plain($type->title_label),
      '#description' => t('Write here the activity title.'),
      '#required' => TRUE,
      '#default_value' => nquire_commons_form_node_default_value($node, 'title', $form_state, 'title', ''),
  );

  //Show a simple textarea, no input selector required....
  $form['body_filter']['body'] = array(
      '#type' => 'textarea',
      '#title' => check_plain($type->body_label),
      '#description' => t('Write here the instructions for the inquiry participants. This should describe their tasks in this activity.'),
      '#default_value' => nquire_commons_form_node_default_value($node, 'body', $form_state, 'body', ''),
      '#required' => FALSE,
  );

  inquiry_available_features_invoke('activity', 'form', $node, $design, $form, $form_state);

  inquiry_creator_create_form_save_cancel_buttons($form, 'inquiry_creator_activity_form_submit', "creator/{$design->nid}/");

  return $form;
}

function inquiry_creator_activity_form_submit($form, $form_state) {
  $design = node_load($form_state['values']['inquiry_design']);
  $activity = node_load($form_state['values']['activity_id']);
  $title = $form_state['values']['title'];

  if ($design && $activity) {
    $changed = FALSE;
    $design_changed = FALSE;

    if ($activity->title !== $title) {
      if (inquiry_design_set_item_title($design, $activity->nid, $title)) {
        $design_changed = TRUE;
      }
      $activity->title = $title;
      $changed = TRUE;
    }

    if ($activity->body !== $form_state['values']['body']) {
      $activity->body = $form_state['values']['body'];
      $changed = TRUE;
    }

    $data_changed = inquiry_available_features_invoke('activity', 'form_process', $activity, $design, $form_state['values']);

    if ($changed || $data_changed) {
      $activity->revision = TRUE;
      node_save($activity);
      
      inquiry_deployer_update_activity($design, inquiry_design_load_phase_for_activity($design, $activity->nid), $activity);
    }

    if ($design_changed) {
      $design->revision = TRUE;
      node_save($design);
    }
  }

  inquiry_creator_form_close($form, $form_state);
}

function inquiry_creator_activity_delete($design_nid, $activity_nid) {
  $design = node_load($design_nid);
  $node = node_load($activity_nid);

  $output = theme('inquiry_creator_inquiry_main', $design, 'activities');

  $form = drupal_get_form('inquiry_creator_activity_delete_form', $design, $node);

  $output .= theme('inquiry_creator_side_form', $design, $activity_nid, 'activity', $form);
  return $output;
}

function inquiry_creator_activity_delete_form(&$form_state, $design, $node) {
  $form = array();

  _inquiry_creator_create_inquiry_id_form_item($design, $form);
  _inquiry_creator_create_activity_id_form_item($node, $form);

  $info = t('<h1 class="title">Are you sure to delete activity <i>@title</i>?</h1>', array('@title' => $node->title))
          . '<div>' . t('This action cannot be undone.') . '</div>';

  $form['info'] = array(
      '#value' => $info,
  );

  inquiry_creator_create_form_delete_cancel_buttons($form, 'inquiry_creator_activity_delete_form_submit', "creator/{$design->nid}");

  return $form;
}

function inquiry_creator_activity_delete_form_submit($form, $form_state) {
  global $user;

  $activity_nid = $form_state['values']['activity_id'];

  if (_inquiry_creator_activity_delete_node($activity_nid)) {
    $design = node_load($form_state['values']['inquiry_design']);
    if ($design && $design->type === 'inquiry_design' && $design->uid == $user->uid) {
      if (inquiry_design_remove_structure_item($design, $activity_nid)) {
        $design->revision = TRUE;
        node_save($design);
      }
    }
  }

  inquiry_creator_form_close($form, $form_state);
}

function _inquiry_creator_activity_delete_node($activity_nid) {
  global $user;

  $activity = node_load($activity_nid);

  if ($activity && $activity->type === 'inquiry_activity' && $activity->uid == $user->uid) {
    inquiry_available_features_invoke('activity', 'delete', $activity);
    node_delete($activity_nid);

    return TRUE;
  }

  return FALSE;
}

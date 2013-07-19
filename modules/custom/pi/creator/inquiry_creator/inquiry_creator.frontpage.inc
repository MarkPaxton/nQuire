<?php

function inquiry_creator_frontpage($nid) {
  $node = node_load($nid);

  $output = '';
  $output .= drupal_get_form('inquiry_creator_frontpage_form', $node);

  return $output;
}

function inquiry_creator_frontpage_form(&$form_state, $node) {
  $form = array();

  nquire_commons_form_set_default_link_action($form, 'submit');

  _inquiry_creator_create_inquiry_id_form_item($node, $form);

  //$form['#attributes'] = array('enctype' => "multipart/form-data");

  $form['editor']['frontpage'] = array(
      '#type' => 'textarea',
      '#title' => t('Frontpage'),
      '#description' => t('Edit here the inquiry front page.'),
      '#default_value' => nquire_commons_form_node_default_value($node, 'frontpage', $form_state, 'frontpage', ''),
      '#required' => FALSE,
  );
  $form['editor']['format'] = filter_form(2);

  nquire_commons_create_form_save_cancel_buttons($form, 'inquiry_creator_frontpage_form_submit', "creator/{$node->nid}");

  return $form;
}

function inquiry_creator_frontpage_form_submit($form, $form_state) {
  global $user;
  $node_nid = $form_state['values']['inquiry_design'];

  $node = node_load($node_nid);
  $inquiry_info = pi_info()->getInquiryInfo($node_nid);
  if ($inquiry_info->userIsInquiryAdmin($user->uid)) {
    $save = FALSE;

    if ($node->frontpage !== $form_state['values']['frontpage']) {
      $node->frontpage = $form_state['values']['frontpage'];
      $save = TRUE;
    }

    if ($save) {
      $node->revision = TRUE;
      node_save($node);
    }
  }

  drupal_goto('creator/' . $node->nid);
}

<?php

function pi_inquiry_manager_publish() {
  if (pi_info()->getCurrentPath()->inquiry_nid) {
    $output = drupal_get_form('pi_inquiry_manager_publish_form');
    return $output;
  }
}

function pi_inquiry_manager_publish_form(&$form_state) {
  $form = array();

  $inquiry_info = pi_info()->getCurrentInquiryInfo();
  if ($inquiry_info) {
    global $user;

    drupal_set_title(t('Publish investigation results in nQuire'));

    $form['intro'] = array(
        '#value' => theme('pi_journal_share_information', $inquiry_info->getInquiryNode()),
    );

    $contribution_node = pi_journal_contribution_find_contribution_node($inquiry_info->getInquiryNode(), $user->uid);

    if (pi_journal_edit_share_access($inquiry_info->getInquiryNode())) {
      $current_state = $contribution_node && $contribution_node->shared;

      $form['current_publish_state'] = array(
          '#type' => 'hidden',
          '#required' => FALSE,
          '#default_value' => $current_state,
      );

      $form['title'] = array(
          '#type' => 'textfield',
          '#title' => t('Title'),
          '#description' => t('Enter the title for your inquiry results.'),
          '#required' => FALSE,
          '#default_value' => nquire_commons_form_node_default_value($contribution_node, 'title', $form_state, 'title', ''),
      );

      $form['summary'] = array(
          '#type' => 'textarea',
          '#title' => t('Summary'),
          '#description' => t('Enter the summary of your investigation here.'),
          '#required' => FALSE,
          '#default_value' => nquire_commons_form_node_default_value($contribution_node, 'body', $form_state, 'summary', ''),
      );

      $cancel_path = 'inquiry/' . $inquiry_info->getInquiryNid() . '/publish';
      $submit_callback = 'pi_inquiry_manager_pubish_form_publish_submit';

      $buttons = $current_state ? array(
          'submit' => array(
              'type' => 'button',
              'button_type' => 'publish',
              'label' => t('Save'),
              'callback' => $submit_callback,
          ),
          'unpublish' => array(
              'type' => 'button',
              'button_type' => 'unpubish',
              'label' => t('Unpublish results'),
              'callback' => $submit_callback,
          ),
          'cancel' => array(
              'type' => 'link',
              'label' => t('Cancel'),
              'callback' => $cancel_path,
          ),
              ) : array(
          'submit' => array(
              'type' => 'button',
              'button_type' => 'publish',
              'label' => t('Publish my results'),
              'callback' => $submit_callback,
          ),
          'cancel' => array(
              'type' => 'link',
              'label' => t('Cancel'),
              'callback' => $cancel_path,
          ),
      );

      nquire_commons_create_form_buttons($form, $buttons);
      nquire_commons_create_form_destination($form, $cancel_path);
    }
  }
  return $form;
}

function pi_inquiry_manager_publish_form_validate($form, $form_state) {
  if ($form_state['clicked_button']['#button_type'] === 'publish' && !$form_state['values']['title']) {
    form_set_error('title', t('A title is needed to publish your results.'));
  }
}

function pi_inquiry_manager_pubish_form_publish_submit($form, $form_state) {
  global $user;
  $inquiry_info = pi_info()->getCurrentInquiryInfo();

  if (pi_journal_edit_share_access($inquiry_info->getInquiryNode())) {
    $share = $form_state['clicked_button']['#button_type'] === 'publish';
    $title = $form_state['values']['title'];
    $summary = $form_state['values']['summary'];
    pi_journal_edit_share_investigation($inquiry_info->getInquiryNode(), $user->uid, $share, $title, $summary);
  }

  return;
}

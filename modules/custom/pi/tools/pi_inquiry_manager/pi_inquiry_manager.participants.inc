<?php

function pi_inquiry_manager_participants($nid) {

  $inquiry = node_load($nid);
  $output = theme('pi_inquiry_manager_participants_view_inquiry_group', $inquiry);
  return $output;
}

function theme_pi_inquiry_manager_participants_view_inquiry_group($inquiry_node) {

  $output = '';

  $group = NULL;
  $inquiry_selection = pi_inquiry_groupings_selection_type($inquiry_node);
  switch ($inquiry_selection) {
    case 'justme':
      $output .= t('This inquiry is just for myself.');
      break;
    case 'group':
      $cohort = pi_inquiry_groupings_get_cohort_group($inquiry_node);
      $output .= t('This inquiry is for members of group: <b>@group</b>.', array('@group' => $cohort->title));
      $group = pi_inquiry_groupings_get_inquiry_group($inquiry_node);
      break;
    case 'anyone':
      $output .= t('This inquiry is for anyone.');
      $group = pi_inquiry_groupings_get_inquiry_group($inquiry_node);
      break;
  }

  if ($group) {
    $output .= '<br/>';

    $group_selection = pi_inquiry_groupins_get_group_selection($group);
    switch ($group_selection) {
      case 0:
        $output .= $inquiry_selection === 'group' ? t('Access to the inquiry is open for group members.') : t('Access to the inquiry is open.');
        break;
      case 1:
        $output .= t('Access needs to be approved by the inquiry owner.');
        break;
      case 2:
        $output .= t('Access is only through invitation.');
        break;
    }
  }
  
  if (pi_inquiry_manager_participants_group_edit_access($inquiry_node->nid)) {
    $output .= '<br/>' . l(t('Edit'), "inquiry/{$inquiry_node->nid}/participants/access");
  }
  return $output;
}


function pi_inquiry_manager_participants_edit_inquiry_group($inquiry_nid) {
  return "!";
}

/*
function pi_inquiry_manager_participants_form(&$form_state, $inquiry_node) {
  $form = array();

  ahah_helper_register($form, $form_state);

  nquire_commons_create_form_save_cancel_buttons($form, '...', "inquiry/{$inquiry_node->nid}/participants");
  
  return $form;
}
*/
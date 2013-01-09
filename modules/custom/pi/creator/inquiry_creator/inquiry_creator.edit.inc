<?php

function inquiry_creator_new() {
  include_once(drupal_get_path('module', 'node') . '/node.pages.inc');

  $output = '';
  $output .= node_add('inquiry_design');

  return $output;
}

function inquiry_creator_edit($nid) {
  include_once(drupal_get_path('module', 'node') . '/node.pages.inc');

  $node = node_load($nid);

  $output = '';
  $output .= node_page_edit($node);

  return $output;
}

function inquiry_creator_main($nid, $op = '', $item = 0) {
  $design = node_load($nid);
  drupal_set_title($design->title);

  $output = theme('inquiry_creator_inquiry_main', $design, $op, $item);

  switch ($op) {
    case '': /* main view, nothing is being edited */
      break;
    case 'activities': /* activity structure is being edited */
      $output .= drupal_get_form('inquiry_creator_activities_form', $design);
      break;
    case 'stages':
      $output .= drupal_get_form('inquiry_creator_stages_form', $design);
      break;
    default: /* other things */
      break;
  }

  return $output;
}

function theme_inquiry_creator_inquiry_metadata($node) {
  $output = '';
  $output .= '<b>' . $node->title . '</b>: ' . $node->body;
  $output .= '&nbsp;&nbsp;<small>' . l(t('edit'), "creator/{$node->nid}/edit") . '</small>';

  return $output;
}

function theme_inquiry_creator_inquiry_main($node, $current_op = '') {
  $output = '';

  $output .= theme('inquiry_creator_inquiry_metadata', $node);
  $output .= theme('inquiry_creator_edit_menu', $node, $current_op);

  return $output;
}

function theme_inquiry_creator_edit_menu($node, $current_op) {
  $ops = array(
      'activities' => t('Edit structure'),
      'stages' => t('Edit temporal organization'),
      'groups' => t('Edit collaboration organization'),
      'roles' => t('Edit roles'),
  );

  $use_links = strlen($current_op) === 0;

  $output = '<div>';

  foreach ($ops as $op => $text) {
    if ($use_links) {
      $output .= l($text, "creator/{$node->nid}/$op");
    } else if ($op === $current_op) {
      $output .= "<b>$text</b>";
    } else {
      $output .= $text;
    }

    $output .= '&nbsp;&nbsp;';
  }

  $output .= '</div>';

  return $output;
}

/* forms */

function _inquiry_creator_create_inquiry_id_form_item($node, &$form) {
  $form['inquiry_design'] = array(
      '#type' => 'hidden',
      '#default_value' => $node->nid,
      '#required' => TRUE,
  );
}

function _inquiry_creator_create_form_buttons($callback, &$form) {
  $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save'),
      '#submit' => array($callback),
  );
  $form['cancel'] = array(
      '#type' => 'submit',
      '#value' => t('Cancel'),
      '#submit' => array('inquiry_creator_form_cancel'),
  );
}

function inquiry_creator_form_cancel($form, $form_state) {
  drupal_goto("creator/{$form_state['values']['inquiry_design']}");
}

function inquiry_creator_activities_form(&$form_state, $node) {
  $form = array();

  _inquiry_creator_create_inquiry_id_form_item($node, $form);
  inquiry_structure_widget_create_form_item($form, 'structure', $node->structure);
  _inquiry_creator_create_form_buttons('inquiry_creator_activities_form_submit', $form);

  return $form;
}

function inquiry_creator_activities_form_submit($form, $form_state) {
  global $user;
  $node = node_load($form_state['values']['inquiry_design']);
  if ($node && $node->type === 'inquiry_design' && $node->uid === $user->uid) {
    $phases = json_decode($form_state['values']['structure']);
    $id = 0;
    foreach ($phases as $phase) {
      if (is_numeric($phase->id)) {
        $id = max($id, $phase->id);
      }
      foreach ($phase->activities as $activity) {
        if (is_numeric($activity->id)) {
          $id = max($id, $activity->id);
        }
      }
    }

    foreach ($phases as $phase) {
      if (!is_numeric($phase->id)) {
        $phase->id = ++$id;
      }
      foreach ($phase->activities as $activity) {
        if (!is_numeric($activity->id)) {
          $activity->id = ++$id;
        }
      }
    }

    $node->structure = json_encode($phases);
    $node->revision = TRUE;
    node_save($node);
  }
  inquiry_creator_form_cancel($form, $form_state);
}

function inquiry_creator_stages_form(&$form_state, $node) {
  $form = array();

  _inquiry_creator_create_inquiry_id_form_item($node, $form);
  inquiry_structure_stages_table_create_form_item($form, 'stages', $node->structure, $node->stages);
  _inquiry_creator_create_form_buttons('inquiry_creator_stages_form_submit', $form);

  return $form;
}

function inquiry_creator_roles_form($node) {
  
}

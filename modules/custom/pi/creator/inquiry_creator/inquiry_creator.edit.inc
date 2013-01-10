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
  $output .= drupal_get_form('inquiry_creator_metadata_form', $node);

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
    case 'groups':
      $output .= drupal_get_form('inquiry_creator_groups_form', $design);
      break;
    case 'roles':
      $output .= drupal_get_form('inquiry_creator_roles_form', $design);
      break;
    default: /* other things */
      break;
  }

  return $output;
}

function theme_inquiry_creator_inquiry_metadata($node, $editable) {
  $output = '<p>';
  $output .= t('Title:') . '&nbsp<b>' . $node->title . '</b>';
  $output .= '<br/>' . t('Description:') . '&nbsp<b>' . $node->body . '</b>';
  if ($editable) {
    $output .= '<br/><small>' . l(t('edit'), "creator/{$node->nid}/edit") . '</small>';
  }
  $output .= '<br/>';

  return $output;
}

function theme_inquiry_creator_inquiry_main($node, $current_op = '') {
  $output = '';

  $output .= theme('inquiry_creator_inquiry_metadata', $node, $current_op === '');
  $output .= theme('inquiry_creator_edit_menu', $node, $current_op);

  return $output;
}

function theme_inquiry_creator_edit_menu($node, $current_op) {
  drupal_add_css(drupal_get_path('module', 'inquiry_creator') . '/css/editMenu.css');
  
  $ops = array(
      'activities' => t('Edit structure'),
      'stages' => t('Edit temporal organization'),
      'groups' => t('Edit collaboration organization'),
      'roles' => t('Edit roles'),
  );

  $use_links = strlen($current_op) === 0;

  $output = '<div>';

  foreach ($ops as $op => $text) {
    $output .= '<div class="edit-link-container' . ($use_links ? ' active' : ($op === $current_op ? ' selected' : '')) . '">';
    if ($use_links) {
      $output .= l($text, "creator/{$node->nid}/$op");
    } else if ($op === $current_op) {
      $output .= "<b>$text</b>";
    } else {
      $output .= $text;
    }

    $output .= '</div>';
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

function _inquiry_creator_create_form_buttons($callback, &$form, $node) {
  $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save'),
      '#submit' => array($callback),
  );
  $form['cancel'] = array(
      '#type' => 'markup',
      '#value' => l(t('Cancel'), "creator/{$node->nid}/"),
  ); /*
    $form['cancel'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel'),
    '#validate' => array(),
    '#submit' => array('inquiry_creator_form_cancel'),
    ); */
}

function inquiry_creator_form_cancel($form, $form_state) {
  drupal_goto("creator/{$form_state['values']['inquiry_design']}");
}

function inquiry_creator_activities_form(&$form_state, $node) {
  $form = array();

  _inquiry_creator_create_inquiry_id_form_item($node, $form);
  inquiry_structure_widget_create_form_item($form, 'structure', $node->structure);
  _inquiry_creator_create_form_buttons('inquiry_creator_activities_form_submit', $form, $node);

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

function inquiry_creator_groups_form(&$form_state, $node) {
  $form = array();

  _inquiry_creator_create_inquiry_id_form_item($node, $form);
  inquiry_structure_groups_table_create_form_item($form, 'groups', $node->structure, $node->groups);
  _inquiry_creator_create_form_buttons('inquiry_creator_groups_form_submit', $form, $node);

  return $form;
}

function inquiry_creator_groups_form_submit($form, $form_state) {
  global $user;
  $node = node_load($form_state['values']['inquiry_design']);
  if ($node && $node->type === 'inquiry_design' && $node->uid === $user->uid) {

    $node->groups = $form_state['values']['groups'];
    $node->revision = TRUE;
    node_save($node);
  }
  inquiry_creator_form_cancel($form, $form_state);
}

function inquiry_creator_roles_form(&$form_state, $node) {
  $form = array();

  _inquiry_creator_create_inquiry_id_form_item($node, $form);
  inquiry_structure_roles_table_create_form_item($form, 'roles', $node->structure, $node->roles);
  _inquiry_creator_create_form_buttons('inquiry_creator_roles_form_submit', $form, $node);

  return $form;
}

function inquiry_creator_roles_form_submit($form, $form_state) {
  global $user;
  $node = node_load($form_state['values']['inquiry_design']);
  if ($node && $node->type === 'inquiry_design' && $node->uid === $user->uid) {
    $roles = json_decode($form_state['values']['roles']);
    $id = 0;
    foreach ($roles as $role) {
      if (is_numeric($role->id)) {
        $id = max($id, $role->id);
      }
    }

    foreach ($roles as $role) {
      if (!is_numeric($role->id)) {
        $role->id = ++$id;
      }
    }

    $node->roles = json_encode($roles);
    $node->revision = TRUE;
    node_save($node);
  }
  inquiry_creator_form_cancel($form, $form_state);
}

function inquiry_creator_stages_form(&$form_state, $node) {
  $form = array();

  _inquiry_creator_create_inquiry_id_form_item($node, $form);
  inquiry_structure_stages_table_create_form_item($form, 'stages', $node->structure, $node->stages);
  _inquiry_creator_create_form_buttons('inquiry_creator_stages_form_submit', $form, $node);

  return $form;
}

function inquiry_creator_stages_form_submit($form, $form_state) {
  global $user;
  $node = node_load($form_state['values']['inquiry_design']);
  if ($node && $node->type === 'inquiry_design' && $node->uid === $user->uid) {
    $stages = json_decode($form_state['values']['stages']);
    $id = 0;
    foreach ($stages as $stage) {
      if (is_numeric($stage->id)) {
        $id = max($id, $stage->id);
      }
    }

    foreach ($stages as $stage) {
      if (!is_numeric($stage->id)) {
        $stage->id = ++$id;
      }
    }

    $node->stages = json_encode($stages);
    $node->revision = TRUE;
    node_save($node);
  }
  inquiry_creator_form_cancel($form, $form_state);
}

function inquiry_creator_metadata_form(&$form_state, $node) {
  $form = array();

  _inquiry_creator_create_inquiry_id_form_item($node, $form);

  $type = node_get_types('type', $node);

  $form['title'] = array(
      '#type' => 'textfield',
      '#title' => check_plain($type->title_label),
      '#description' => t('Write here the title for the inquiry.'),
      '#required' => TRUE,
      '#default_value' => $node->title,
  );

  //Show a simple textarea, no input selector required....
  $form['body_filter']['body'] = array(
      '#type' => 'textarea',
      '#title' => check_plain($type->body_label),
      '#description' => t('An introduction to the inquiry, e.g. about the goals of the inquiry, the scientific field etc.'),
      '#default_value' => $node->body,
      '#required' => TRUE
  );

  _inquiry_creator_create_form_buttons('inquiry_creator_metadata_form_submit', $form, $node);

  return $form;
}

function inquiry_creator_metadata_form_submit($form, $form_state) {
  global $user;
  $node = node_load($form_state['values']['inquiry_design']);
  if ($node && $node->type === 'inquiry_design' && $node->uid === $user->uid) {
    $node->title = $form_state['values']['title'];
    $node->body = $form_state['values']['body'];
    $node->revision = TRUE;
    // node_save($node);
  }
  inquiry_creator_form_cancel($form, $form_state);
}


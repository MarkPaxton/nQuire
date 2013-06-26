<?php

function inquiry_creator_new() {
  include_once(drupal_get_path('module', 'node') . '/node.pages.inc');

  $output = '';
  $output .= drupal_get_form('inquiry_creator_metadata_form', NULL);

  return $output;
}

function inquiry_creator_edit($nid) {
  include_once(drupal_get_path('module', 'node') . '/node.pages.inc');

  $node = node_load($nid);

  $output = '';
  $output .= drupal_get_form('inquiry_creator_metadata_form', $node);

  return $output;
}

function inquiry_creator_inquiry_delete($nid) {
  include_once(drupal_get_path('module', 'node') . '/node.pages.inc');

  $node = node_load($nid);

  $output = '';
  $output .= drupal_get_form('inquiry_creator_inquiry_delete_form', $node);

  return $output;
}

function inquiry_creator_inquiry_delete_form(&$form_state, $design) {
  $form = array();

  _inquiry_creator_create_inquiry_id_form_item($design, $form);

  $info = t('<h1 class="title">Are you sure to delete inquiry <i>@title</i>?</h1>', array('@title' => $design->title));
  $info .= '<p>' . t('All phases, activities, stages, and groups that compose this inquiry will be delete!') . '</p>';
  $info .= '<p>' . t('This action cannot be undone.') . '</p>';


  $form['info'] = array(
      '#value' => $info,
  );

  nquire_commont_create_form_delete_cancel_buttons($form, 'inquiry_creator_inquiry_delete_form_submit', "creator/{$design->nid}");

  return $form;
}

function inquiry_creator_inquiry_delete_form_submit($form, $form_state) {
  global $user;

  $design = node_load($form_state['values']['inquiry_design']);

  if ($design && $design->type === 'pi_inquiry' && $design->uid == $user->uid) {
    node_delete($design->nid);

    /*    $phase_nid = $phase->nid;

      $inquiry_info = pi_info()->getInquiryInfo($design->nid);
      $activities = $inquiry_info->getActivitiesForPhase($phase_nid);
      foreach ($activities as $activity) {
      module_load_include('inc', 'inquiry_creator', 'inquiry_creator.activities');
      _inquiry_creator_activity_delete_node($activity->nid);
      }

      node_delete($phase_nid); */
    drupal_goto('creator');
  }

  nquire_commons_form_close($form, $form_state);
}

function inquiry_creator_main($nid, $op = '', $item = '') {
  $design = node_load($nid);
  drupal_set_title(check_plain($design->title));

  switch ($op) {
    case '':
      $content = drupal_get_form('inquiry_creator_activities_form', $design);
      break;
    case 'stages':
      $content = drupal_get_form('inquiry_creator_stages_form', $design);
      break;
    case 'groups':
      $content = drupal_get_form('inquiry_creator_groups_form', $design);
      break;
    case 'roles':
      $content = '<p>' .
              t('Here you can create roles and assign them different permission for each activity. Later, users will be assigned to concrete roles. In this way you can control what each user can do in the inquiry activities.')
              . '</p>'
              . theme('inquiry_creator_show_default_roles', $design)
              . drupal_get_form('inquiry_creator_roles_form', $design);
      break;
    case 'defaultroles':
      $content = '<p>' . t('Here you can select the roles that are assigned by default to new inquiry participants.') . '</p>'
              . drupal_get_form('inquiry_creator_defaultroles_form', $design);
      break;
    default: /* other things */
      break;
  }

  $output = theme('inquiry_creator_inquiry_main', $design, $op, $content);

  return $output;
}

function theme_inquiry_creator_inquiry_metadata_form($form) {
  drupal_add_css(drupal_get_path('module', 'inquiry_creator') . '/css/inquiry_creator_metadata.css');

  return drupal_render($form);
}

function theme_inquiry_creator_inquiry_metadata($node, $editable = FALSE) {
  drupal_add_css(drupal_get_path('module', 'inquiry_creator') . '/css/inquiry_creator_metadata.css');

  $title = t('Inquiry description');
  $content_cells = array(
      array(
          array('data' => t('Title:'), 'class' => 'inquiry_creator_metadata_table_header'),
          array('data' => check_plain($node->title), 'class' => 'inquiry_creator_metadata_table_value'),
      ),
      array(
          array('data' => t('Description:'), 'class' => 'inquiry_creator_metadata_table_header'),
          array('data' => nl2br(check_plain($node->body)), 'class' => 'inquiry_creator_metadata_table_value'),
      ),
  );
  $content = nquire_commons_create_not_expanding_table_block(theme('table', NULL, $content_cells, array('id' => 'inquiry_creator_metadata_table')));

  if ($editable) {
    $content .= '<small>'
            . nquire_commons_form_submit_link(t('Edit'), "creator/{$node->nid}/edit")
            . '&nbsp;&nbsp;&nbsp;'
            . nquire_commons_form_submit_link(t('Delete inquiry'), "creator/{$node->nid}/delete")
            . '</small>';

    $output .= nquire_commons_create_page_section($content, $title);
  }

  return $output;
}

function theme_inquiry_creator_inquiry_main($node, $current_op = '', $content = '') {
  $output = '';

  $output .= theme('inquiry_creator_inquiry_metadata', $node, TRUE);
  $output .= theme('inquiry_creator_edit_menu', $node, $current_op, $content);

  return $output;
}

function theme_inquiry_creator_edit_menu($node, $current_op, $content) {
  drupal_add_css(drupal_get_path('module', 'inquiry_creator') . '/css/editMenu.css');
  nquire_commons_load_submit_form_links();

  $ops = array(
      '' => t('Edit phases and activities'),
      'stages' => t('Edit temporal organization'),
      'groups' => t('Edit collaboration organization'),
      'roles' => t('Edit roles'),
  );

  $output = '<div class="edit-link-menu-container">';

  $output .= '<table class="edit-link-menu"><tr>';
  foreach ($ops as $key => $title) {
    $output .= '<td class="edit-link-container' . ($key === $current_op ? ' selected' : ' active') . '">';

    if ($key === $current_op) {
      $output .= "<b>$title</b>";
    } else {
      $output .= nquire_commons_form_submit_link($title, "creator/{$node->nid}/{$key}");
    }

    $output .= '</td>';
  }

  $output .= '<td class="edit-link-container padding"></td>';

  $table .= '</tr></table>';
  $output .= $table;

  $output .= '</div>';

  $output .= '<div class="edit-link-menu-content">'
          . $content
          . '</div>';

  return $output;
}

function theme_inquiry_creator_side_form($design, $item, $type, $form) {
  nquire_commons_load_javascript_support();
  nquire_commons_load_ahah_fix();

  drupal_add_js(drupal_get_path('module', 'inquiry_creator') . '/js/editFormPosition.js');
  drupal_add_css(drupal_get_path('module', 'inquiry_creator') . '/css/sideForm.css');

  $output = '<table class="item-edit-table"><tr><td>'
          . theme('nquire_structure_widget_view', $design, $item)
          . '</td><td><div class="item-edit-form-container item-edit-form-container-' . $type . '">'
          . $form
          . '</div></td></tr></table>';

  return $output;
}

function theme_inquiry_creator_show_default_roles($design) {
  $output = '';
  $roles = nquire_commons_decode_data($design->roles, '[]');
  if (count($roles) > 0) {
    $default = array();
    foreach ($roles as $role) {
      if ($role['default']) {
        $default[] = $role['title'];
      }
    }
    $output = '<p>';
    if (count($default) > 0) {
      $output .= t('The following roles are assigned to participants by default: ') . '<b>' . implode(', ', $default) . '</b>';
    } else {
      $output .= t('No roles are assigned to participants by default.');
    }

    $output .= '<br/>' . nquire_commons_form_submit_link(t('Edit'), 'creator/' . arg(1) . '/defaultroles') . '</p>';
  }
  return $output;
}

/* forms */

function inquiry_creator_activities_form(&$form_state, $node) {
  $form = array();

  nquire_commons_form_set_default_link_action($form, 'submit');

  _inquiry_creator_create_inquiry_id_form_item($node, $form);
  $structure = json_encode(inquiry_creator_get_inquiry_structure($node->nid));
  nquire_structure_widget_create_form_item($form, 'structure', $structure);
  nquire_commons_create_form_save_cancel_buttons($form, 'inquiry_creator_activities_form_submit', "creator/{$node->nid}/");

  return $form;
}

function inquiry_creator_activities_form_submit($form, $form_state) {

  global $user;
  $node = node_load($form_state['values']['inquiry_design']);

  $new_ids = array('phase' => array(), 'activity' => array());

  if ($node && $node->type === 'pi_inquiry' && $node->uid === $user->uid) {
    $current_phases = inquiry_creator_get_inquiry_structure($node->nid);

    $activity_phases = array();
    $weights = array();

    foreach ($current_phases as $i => $phase) {
      $weights[$phase->id] = $i;
      foreach ($phase->activities as $j => $activity) {
        $weights[$activity->id] = $j;
        $activity_phases[$activity->id] = $phase->id;
      }
    }

    $new_phases = json_decode($form_state['values']['structure']);

    foreach ($new_phases as $i => $phase) {
      if (isset($weights[$phase->id])) {
        $phase_node = node_load($phase->id);
        if ($i != $weights[$phase->id]) {
          $phase_node->weight = $i;
          $phase_node->revision = TRUE;
          node_save($phase_node);
        }
      } else {
        $phase_node = inquiry_creator_create_phase_stub($node->nid, $phase->title, $i);
        $new_ids['phase'][$phase->id] = $phase_node->nid;
      }

      foreach ($phase->activities as $j => $activity) {
        if (isset($weights[$activity->id])) {
          $activity_node = node_load($activity->id);
          $change = FALSE;
          if ($j != $weights[$activity->id]) {
            $activity_node->weight = $j;
            $change = TRUE;
          }

          if ($activity_node->phase_nid != $phase_node->nid) {
            $activity_node->phase_nid = $phase_node->nid;
            $change = TRUE;
          }

          if ($change) {
            $activity_node->revision = TRUE;
            node_save($activity_node);
          }
        } else {
          $activity_node = inquiry_creator_create_activity_stub($phase_node->nid, $activity->type, $activity->title, $j);
          $new_ids['activity'][$activity->id] = $activity_node->nid;
        }
      }
    }
  }

  $destination_parts = explode('/', $form_state['values']['form_destination']);
  if (count($destination_parts) >= 4 && isset($new_ids[$destination_parts[2]][$destination_parts[3]])) {
    $destination_parts[3] = $new_ids[$destination_parts[2]][$destination_parts[3]];
    $form_state['values']['form_destination'] = implode('/', $destination_parts);
  }

  nquire_commons_form_close($form, $form_state);
}

function inquiry_creator_groups_form(&$form_state, $node) {
  $form = array();
  nquire_commons_form_set_default_link_action($form, 'submit');

  $structure = json_encode(inquiry_creator_get_inquiry_structure($node->nid));
  $groups_obj = array(
      array(
          'id' => 0,
          'title' => t('Grouping:'),
          'values' => array(),
      ),
  );
  $inquiry_info = pi_info()->getInquiryInfo($node->nid);
  foreach ($inquiry_info->getPhases() as $phase_nid => $phase_node) {
    $groups_obj[0]['values'][$phase_nid] = $phase_node->collaboration;
  }
  $groups = json_encode($groups_obj);

  _inquiry_creator_create_inquiry_id_form_item($node, $form);
  nquire_structure_groups_table_create_form_item($form, 'groups', $structure, $groups);
  nquire_commons_create_form_save_cancel_buttons($form, 'inquiry_creator_groups_form_submit', "creator/{$node->nid}/");

  return $form;
}

function inquiry_creator_groups_form_submit($form, $form_state) {
  global $user;
  $node = node_load($form_state['values']['inquiry_design']);
  if ($node && $node->type === 'pi_inquiry' && $node->uid === $user->uid) {
    $groups = nquire_commons_decode_data($form_state['values']['groups']);
    if (isset($groups[0]['values'])) {
      foreach ($groups[0]['values'] as $phase_nid => $collaboration) {
        $phase_node = node_load($phase_nid);

        if ($phase_node && $phase_node->type === 'pi_phase' && $phase_node->uid === $user->uid && $phase_node->collaboration != $collaboration) {
          $phase_node->collaboration = $collaboration;
          $phase_node->revision = TRUE;
          node_save($phase_node);
        }
      }
    }
  }
  nquire_commons_form_close($form, $form_state);
}

function inquiry_creator_roles_form(&$form_state, $node) {
  $form = array();

  nquire_commons_form_set_default_link_action($form, 'submit');

  $structure = json_encode(inquiry_creator_get_inquiry_structure($node->nid));
  _inquiry_creator_create_inquiry_id_form_item($node, $form);
  nquire_structure_roles_table_create_form_item($form, 'roles', $structure, $node->roles);
  nquire_commons_create_form_save_cancel_buttons($form, 'inquiry_creator_roles_form_submit', "creator/{$node->nid}/");

  return $form;
}

function inquiry_creator_roles_form_submit($form, $form_state) {
  global $user;
  $node = node_load($form_state['values']['inquiry_design']);
  if ($node && $node->type === 'pi_inquiry' && $node->uid === $user->uid) {
    $old_roles = nquire_commons_decode_data($node->roles);
    $default_roles = array();
    foreach ($old_roles as $role) {
      $default_roles[$role->id] = $role->default ? true : false;
    }

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

      $role->default = isset($default_roles[$role->id]) ? $default_roles[$role->id] : FALSE;
    }

    $new_roles = json_encode($roles);
    if ($node->roles !== $new_roles) {
      $node->roles = $new_roles;
      $node->revision = TRUE;
      node_save($node);
    }
  }
  nquire_commons_form_close($form, $form_state);
}

function inquiry_creator_defaultroles_form(&$form_state, $node) {
  $form = array();

  nquire_commons_form_set_default_link_action($form, 'submit');

  _inquiry_creator_create_inquiry_id_form_item($node, $form);
  $roles = nquire_commons_decode_data($node->roles);
  $values = array();
  $options = array();
  foreach ($roles as $role) {
    $options[$role['id']] = check_plain($role['title']);
    if ($role['default']) {
      $values[] = $role['id']; // = $role['default'];
    }
  }

  $form['defaultroles'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Default roles'),
      '#description' => t('Select the roles that will be assigned by default.'),
      '#required' => FALSE,
      '#options' => $options,
      '#default_value' => $values,
  );

  nquire_commons_create_form_save_cancel_buttons($form, 'inquiry_creator_defaultroles_form_submit', "creator/{$node->nid}/roles");

  return $form;
}

function inquiry_creator_defaultroles_form_submit($form, $form_state) {
  global $user;
  $node = node_load($form_state['values']['inquiry_design']);
  if ($node && $node->type === 'pi_inquiry' && $node->uid === $user->uid) {
    $roles = nquire_commons_decode_data($node->roles);
    foreach ($roles as &$role) {
      if (isset($form_state['values']['defaultroles'][$role['id']])) {
        $role['default'] = $form_state['values']['defaultroles'][$role['id']] ? TRUE : FALSE;
      }
    }

    $new_roles = json_encode($roles);
    if ($node->roles !== $new_roles) {
      $node->roles = $new_roles;
      $node->revision = TRUE;
      node_save($node);
    }
  }
  nquire_commons_form_close($form, $form_state);
}

function inquiry_creator_stages_form(&$form_state, $node) {
  $form = array();

  nquire_commons_form_set_default_link_action($form, 'submit');

  $structure = json_encode(inquiry_creator_get_inquiry_structure($node->nid));
  $inquiry_info = pi_info()->getInquiryInfo($node->nid);
  $stage_structure = array();

  foreach ($inquiry_info->getStageNodes() as $stage) {
    $stage_data = array(
        'id' => $stage->nid,
        'title' => check_plain($stage->title),
        'values' => array(),
    );

    foreach ($inquiry_info->getPhases() as $phase_node) {
      $stage_data['values'][$phase_node->nid] = isset($stage->phases[$phase_node->nid]) ? $stage->phases[$phase_node->nid] : 'edit';
    }

    $stage_structure[] = $stage_data;
  }


  _inquiry_creator_create_inquiry_id_form_item($node, $form);
  nquire_structure_stages_table_create_form_item($form, 'stages', $structure, json_encode($stage_structure));
  nquire_commons_create_form_save_cancel_buttons($form, 'inquiry_creator_stages_form_submit', "creator/{$node->nid}/");

  $form['#theme'] = 'inquiry_creator_inquiry_metadata_form';

  return $form;
}

function inquiry_creator_stages_form_submit($form, $form_state) {
  global $user;
  $node = node_load($form_state['values']['inquiry_design']);
  if ($node && $node->type === 'pi_inquiry' && $node->uid === $user->uid) {
    $inquiry_info = pi_info()->getInquiryInfo($node->nid);
    $current_stages = $inquiry_info->getStageNodes();

    $stages = json_decode($form_state['values']['stages']);
    $tokeep = array();

    foreach ($stages as $stage) {
      if (isset($current_stages[$stage->id])) {
        $tokeep[$stage->id] = TRUE;

        $stage_node = $current_stages[$stage->id];
        $save = FALSE;

        if ($stage_node->title !== $stage->title) {
          $save = TRUE;
          $stage_node->title = $stage->title;
        }

        if (json_encode($stage_node->phases) != json_encode($stage->values)) {
          $save = TRUE;
          $stage_node->phases = $stage->values;
        }

        if ($save) {
          $stage_node->revision = TRUE;
          node_save($stage_node);
        }
      } else {
        $stage_node = inquiry_creator_create_stage_stub($stage->title, $node->nid, $stage->values);
      }
    }

    foreach ($current_stages as $stage_node) {
      if (!$tokeep[$stage_node->nid]) {
        node_delete($stage_node->nid);
      }
    }
  }

  nquire_commons_form_close($form, $form_state);
}

function inquiry_creator_metadata_form(&$form_state, $node) {
  $form = array();

  nquire_commons_form_set_default_link_action($form, 'submit');

  _inquiry_creator_create_inquiry_id_form_item($node, $form);

  $form['#attributes'] = array('enctype' => "multipart/form-data");

  $form['title'] = array(
      '#type' => 'textfield',
      '#title' => t('Inquiry title'),
      '#description' => t('Write here the title for the inquiry.'),
      '#required' => TRUE,
      '#default_value' => nquire_commons_form_node_default_value($node, 'title', $form_state, 'title', ''),
  );

  //Show a simple textarea, no input selector required....
  $form['body'] = array(
      '#type' => 'textarea',
      '#title' => t('Inquiry description'),
      '#description' => t('An introduction to the inquiry, e.g. about the goals of the inquiry, the scientific field etc.'),
      '#default_value' => nquire_commons_form_node_default_value($node, 'body', $form_state, 'body', ''),
      '#required' => TRUE
  );

  if ($node->image) {
    $form['image_pre'] = array(
        '#value' => '<div><div class="inquiry_creator_metadata_image"><img src="' . file_create_url($node->image) . '"></img></div><div class="inquiry_creator_medatada_image_input_item_block">',
    );
    $form['image'] = array(
        '#type' => 'file',
        '#title' => t('Inquiry image'),
        '#description' => t('Select an image if you want to replace the current one.'),
        '#required' => FALSE,
    );
    $form['image_pos'] = array(
        '#value' => '</div></div>',
    );
  } else {
    $form['image'] = array(
        '#type' => 'file',
        '#title' => t('Inquiry image'),
        '#description' => t('Select an image for the inquiry front page.'),
        '#required' => FALSE,
        '#default_value' => '',
    );
  }

  nquire_commons_create_form_save_cancel_buttons($form, 'inquiry_creator_metadata_form_submit', "creator/");

  $form['#theme'] = 'inquiry_creator_inquiry_metadata_form';

  return $form;
}

function inquiry_creator_metadata_form_submit($form, $form_state) {
  global $user;
  $node_nid = $form_state['values']['inquiry_design'];
  if ($node_nid == 0) {

    $node = new stdClass();
    $node->type = 'pi_inquiry';
    $node->uid = $user->uid;
    $node->title = $form_state['values']['title'];
    $node->body = $form_state['values']['body'];

    $node->roles = json_encode(array());

    node_save($node);

    if (_pi_inquiry_creator_metadata_save_image_file($node, $form, $form_state)) {
      node_save($node);
    }

    inquiry_creator_create_stage_stub(t('Stage 1'), $node->nid);
  } else {
    $node = node_load($node_nid);
    if ($node && $node->type === 'pi_inquiry' && $node->uid === $user->uid) {
      $save = FALSE;

      if ($node->title !== $form_state['values']['title']) {
        $node->title = $form_state['values']['title'];
        $save = TRUE;
      }

      if ($node->body !== $form_state['values']['body']) {
        $node->body = $form_state['values']['body'];
        $save = TRUE;
      }

      if (_pi_inquiry_creator_metadata_save_image_file($node, $form, $form_state)) {
        $save = TRUE;
      }

      if ($save) {
        $node->revision = TRUE;
        node_save($node);
      }
    }
  }

  drupal_goto('creator/' . $node->nid);
}

function _pi_inquiry_creator_metadata_save_image_file(&$node, $form, $form_state) {
  if (isset($_FILES['files']['name']['image'])) {
//	$extensions = 'jpeg jpg gif tiff';
//	$size_limit = file_upload_max_size();
    $validators = array(
            /* 'my_file_validate_extensions' => array($extensions),
              'my_file_validate_size' => array($size_limit), */
    );

    $dest = file_directory_path() . '/pi_inquiry_images/';
    file_check_directory($dest, TRUE);
    $file = file_save_upload('image', $validators, $dest);
    if ($file) {
      if ($node->image) {
        //file_delete($node->image);
      }

      file_set_status($file, FILE_STATUS_PERMANENT);

      $p = strrpos($file->filepath, '.');
      $s = strrpos($file->filepath, '/');

      $path = substr($file->filepath, 0, $s);
      $ext = $p ? substr($file->filepath, $p) : '';

      $new_path = $path . '/' . $node->nid . '_' . nquire_commons_random_string(8) . $ext;
      rename($file->filepath, $new_path);
      $node->image = $new_path;
      return TRUE;
    }
  }
  return FALSE;
}

function inquiry_creator_create_stage_stub($title, $inquiry_nid, $phases = array()) {
  global $user;

  $node = new stdClass();
  $node->type = 'pi_stage';
  $node->inquiry_nid = $inquiry_nid;
  $node->phases = $phases;
  $node->title = $title;
  $node->uid = $user->uid;

  node_save($node);
  return $node;
}

<?php

function _inquiry_creator_add_measures_form_links($measures_data, $current_tool, &$form, $base_path) {
  $value = theme('inquiry_creator_measure_list', $measures_data, $current_tool, $base_path)
          . nquire_commons_form_submit_link(t('Add new measure'), $base_path . '/measure/add');

  $form['measures'] = array(
      '#type' => 'fieldset',
      '#title' => t('Data measures'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#value' => $value,
  );
}

function theme_inquiry_creator_measure_list($list, $current_tool, $base_path) {
  $output = '<div>';

  if (!$list || count($list) === 0) {
    $output .= '<p>' . t('<i>There are no measures in this activity. Click <b>Add new measure</b> to start creating measures.</i>') . '</p>';
  } else {
    $output .= '<ul>';
    $edit_path = $base_path . '/measure/';

    foreach ($list as $i => $nid) {
      $measure = node_load($nid);
      $edit_link = nquire_commons_form_submit_link(t('edit'), $edit_path . $measure->nid);
      $delete_link = nquire_commons_form_submit_link(t('delete'), $edit_path . $measure->nid . '/delete');
      
      $output .= '<li>' . $measure->title . '<small>&nbsp;&nbsp;' . $edit_link . '&nbsp;&nbsp;' . $delete_link . '</small></li>';
    }
    $output .= '</ul>';
  }

  $output .= '</div>';
  return $output;
}

function inquiry_creator_measure_add($design_nid, $activity_nid) {
  return inquiry_creator_measure_edit($design_nid, $activity_nid, FALSE);
}

function inquiry_creator_measure_edit($design_nid, $activity_nid, $measure_nid) {
  $design = node_load($design_nid);
  $activity = node_load($activity_nid);
  $measure = node_load($measure_nid);

  $output = theme('inquiry_creator_inquiry_main', $design, 'activities');

  $form = drupal_get_form('inquiry_creator_measure_form', $design, $activity, $measure);

  $output .= theme('inquiry_creator_side_form', $design, $activity_nid, 'measure', $form);
  return $output;
}

function _inquiry_creator_create_measure_id_form_item($measure, &$form) {
  $form['measure_id'] = array(
      '#type' => 'hidden',
      '#default_value' => $measure ? $measure->nid : '',
      '#required' => FALSE,
  );
}

function inquiry_creator_measure_form(&$form_state, $design, $activity, $measure) {
  $form = array();

  module_load_include('inc', 'inquiry_creator', 'inquiry_creator.edit');
  module_load_include('inc', 'inquiry_creator', 'inquiry_creator.activities');
  _inquiry_creator_create_inquiry_id_form_item($design, $form);
  _inquiry_creator_create_activity_id_form_item($activity, $form);
  _inquiry_creator_create_measure_id_form_item($measure, $form);

  $activity_data = nquire_commons_decode_data($activity->data);
  $data = nquire_commons_decode_data($measure ? $measure->data : NULL);

  $tool = nquire_commons_read_data_value($activity_data, 'tool.type', 'none');

  $title = nquire_commons_form_node_default_value($measure, 'title', $form_state, 'title', '');
  $description = nquire_commons_form_node_default_value($measure, 'body', $form_state, 'description', '');
  $type = nquire_commons_form_data_default_value($data, 'type', $form_state, 'measure_type', '');


  $form['title'] = array(
      '#type' => 'textfield',
      '#title' => t('Measure name'),
      '#description' => t('Write here the name that will identify the measure.'),
      '#required' => TRUE,
      '#default_value' => $title,
  );

  //Show a simple textarea, no input selector required....
  $form['description'] = array(
      '#type' => 'textarea',
      '#title' => t('Measure description'),
      '#description' => t('Write here the description of the measure. This could include, for instance, an explanation of how to use this measure.'),
      '#default_value' => $description,
      '#required' => TRUE
  );

  $options = inquiry_available_measures_for_tool($tool);

  if (!isset($options[$type])) {
    if ($type === '') {
      $options[$type] = t('-Select a measure type');
    } else {
      $options[$type] = inquiry_available_measures_name($type) . t(' (unavailable as @tool is not used)', array('@tool' => inquiry_available_tools_name($tool)));
    }
  }
  
  ksort($options);

  $form['measure_type'] = array(
      '#type' => 'select',
      '#title' => t('Measure type'),
      '#description' => t('Select the measure type. Some measure types are only available when certain intruments are used to collect data.'),
      '#default_value' => $type,
      '#required' => TRUE,
      '#options' => $options,
  );

  _inquiry_creator_create_form_buttons('inquiry_creator_measure_form_submit', "creator/{$design->nid}/activity/{$activity->nid}", $form);

  return $form;
}

function inquiry_creator_measure_form_submit($form, $form_state) {
  global $user;

  $design = node_load($form_state['values']['inquiry_design']);
  $activity = node_load($form_state['values']['activity_id']);

  $measure_nid = $form_state['values']['measure_id'];
  $new = strlen($measure_nid) === 0;

  $title = $form_state['values']['title'];
  $description = $form_state['values']['description'];
  $type = $form_state['values']['measure_type'];

  if ($new) {
    $node = new stdClass();
    $node->type = 'inquiry_measure';
    $node->uid = $user->uid;
  } else {
    $node = node_load($measure_nid);
  }

  $node->title = $title;
  $node->measure_type = $type;
  $node->body = $description;

  $data = array(
      'type' => $type,
  );
  $node->data = json_encode($data);

  node_save($node);

  if ($new) {
    $data = nquire_commons_decode_data($activity->data);
    if (!isset($data['measures'])) {
      $data['measures'] = array();
    }

    if (!in_array($node->nid, $data['measures'])) {
      $data['measures'][] = $node->nid;
      $activity->data = json_encode($data);
      node_save($activity);
    }
  }
  inquiry_creator_form_close($form, $form_state);
}

function inquiry_creator_measure_delete($design_nid, $activity_nid, $measure_nid) {
  $design = node_load($design_nid);
  $activity = node_load($activity_nid);
  $measure = node_load($measure_nid);

  $output = theme('inquiry_creator_inquiry_main', $design, 'activities');

  $form = drupal_get_form('inquiry_creator_measure_delete_form', $design, $activity, $measure);

  $output .= theme('inquiry_creator_side_form', $design, $activity_nid, 'measure', $form);
  return $output;
}

function inquiry_creator_measure_delete_form(&$form_state, $design, $activity, $measure) {
  $form = array();

  module_load_include('inc', 'inquiry_creator', 'inquiry_creator.edit');
  module_load_include('inc', 'inquiry_creator', 'inquiry_creator.activities');
  _inquiry_creator_create_inquiry_id_form_item($design, $form);
  _inquiry_creator_create_activity_id_form_item($activity, $form);
  _inquiry_creator_create_measure_id_form_item($measure, $form);

  $form['info'] = array(
      '#value' => t('<h1 class="title">Are you sure to delete measure @title?</h1>', array('@title' => $measure->title))
      . '<div>' . t('This action cannot be undone.') . '</div>',
  );

  _inquiry_creator_create_form_buttons('inquiry_creator_measure_delete_form_submit', "creator/{$design->nid}/activity/{$activity->nid}", $form, t('Delete'), t('Cancel'));

  return $form;
}

function inquiry_creator_measure_delete_form_submit($form, $form_state) {
  global $user;

  $measure = node_load($form_state['values']['measure_id']);

  if ($measure && $measure->type === 'inquiry_measure' && $user->uid === $measure->uid) {
    $nid = $measure->nid;
    node_delete($measure);

    $activity = node_load($form_state['values']['activity_id']);
    if ($activity && $activity->type === 'inquiry_activity' && $user->uid === $activity->uid) {
      $data = nquire_commons_decode_data($activity->data);
      if (isset($data['measures'])) {
        if (($key = array_search($nid, $data['measures'], FALSE)) !== FALSE) {
          unset($data['measures'][$key]);
          node_save($activity);
        }
      }
    }
  }

  inquiry_creator_form_close($form, $form_state);
}
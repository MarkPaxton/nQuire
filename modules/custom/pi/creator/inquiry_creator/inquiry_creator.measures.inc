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

    $rows = array();
    $edit_path = $base_path . '/measure/';
    foreach ($list as $nid) {
      $measure = node_load($nid);
      $edit_link = nquire_commons_form_submit_link(t('edit'), $edit_path . $measure->nid);
      $delete_link = nquire_commons_form_submit_link(t('delete'), $edit_path . $measure->nid . '/delete');

      $measure_type_name = inquiry_available_features_name('measure', $measure->measure_type);
      $required_tool = inquiry_available_features_measure_required_tool($measure->measure_type);
      if ($required_tool !== 'none' && $required_tool !== $current_tool) {
        $measure_type_name = '<strike>' . $measure_type_name . '</strike>';
      }

      $rows[] = array($measure->title, $measure_type_name, '<small>&nbsp;&nbsp;' . $edit_link . '&nbsp;&nbsp;' . $delete_link . '</small>');
    }

    $header = array(t('Measure'), t('Type'), '');
    $output .= theme('table', $header, $rows, array('id' => 'measures-table'));
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

  ahah_helper_register($form, $form_state);

  nquire_commons_form_set_default_link_action($form, 'submit');

  module_load_include('inc', 'inquiry_creator', 'inquiry_creator.edit');
  module_load_include('inc', 'inquiry_creator', 'inquiry_creator.activities');
  _inquiry_creator_create_inquiry_id_form_item($design, $form);
  _inquiry_creator_create_activity_id_form_item($activity, $form);
  _inquiry_creator_create_measure_id_form_item($measure, $form);

  $activity_data = nquire_commons_decode_data($activity->parameters);

  $tool = nquire_commons_read_data_value($activity_data, 'tool.type', 'none');

  $title = nquire_commons_form_node_default_value($measure, 'title', $form_state, 'title', '');
  $description = nquire_commons_form_node_default_value($measure, 'body', $form_state, 'description', '');
  $type = nquire_commons_form_node_default_value($measure, 'measure_type', $form_state, 'measure_type', '');
  $measure_or_type = $measure && $measure->measure_type == $type ? $measure : $type;


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
      '#required' => FALSE
  );

  $options = inquiry_available_features_list_measures_for_tool($tool);

  if (!isset($options[$type])) {
    if ($type === '') {
      $options[$type] = t('- Select a measure type');
    } else {
      $required_tool = inquiry_available_features_measure_required_tool($type);
      $options[$type] = inquiry_available_features_name('measure', $type) . t(' (unavailable as @tool is not used)', array('@tool' => inquiry_available_features_name('tool', $required_tool)));
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
      '#ahah' => array(
          'path' => ahah_helper_path(array('measure_data')),
          'wrapper' => 'inquiry-creator-measure-custom-form',
          'method' => 'replace',
          'effect' => 'fade',
      ),
  );

  $form['measure_data'] = array();
  $form['measure_data']['wrapper_prefix'] = array(
      '#value' => '<div id="inquiry-creator-measure-custom-form">',
  );
  $form['measure_data']['help'] = array(
      '#value' => '<p><b>' . check_plain(inquiry_available_features_invoke('measure', 'description', $type)) . '</b></p>',
  );

  inquiry_available_features_invoke('measure', 'form', $measure_or_type, $form['measure_data'], $form_state);

  $form['measure_data']['wrapper_suffix'] = array(
      '#value' => '</div>',
  );



  nquire_commons_create_form_save_cancel_buttons($form, 'inquiry_creator_measure_form_submit', "creator/{$design->nid}/activity/{$activity->nid}/");

  return $form;
}

function inquiry_creator_measure_form_validate($form, $form_state) {
  $type = $form_state['values']['measure_type'];
  inquiry_available_features_invoke('measure', 'form_validate', $type, $form, $form_state);
}

function inquiry_creator_measure_form_submit($form, $form_state) {
  global $user;

  $measure_nid = $form_state['values']['measure_id'];
  $new = strlen($measure_nid) === 0;

  $title = $form_state['values']['title'];
  $description = $form_state['values']['description'];
  $type = $form_state['values']['measure_type'];

  $save = FALSE;

  if ($new) {
		$activity_nid = $form_state['values']['activity_id'];
		
    $node = new stdClass();
    $node->type = 'pi_available_measures';
    $node->uid = $user->uid;
		$node->script_measure = TRUE;
		$node->activity_nid = $activity_nid;
    $save = TRUE;
  } else {
    $node = node_load($measure_nid);
  }


  if ($node->title !== $title || $node->measure_type !== $type || $node->body !== $description) {
    $node->title = $title;
    $node->measure_type = $type;
    $node->body = $description;
    $save = TRUE;
  }

  if (inquiry_available_features_invoke('measure', 'form_process', $node, $form_state['values'])) {
    $save = TRUE;
  }

  if ($save) {
    $node->revision = !$new;
    node_save($node);
  }

  nquire_commons_form_close($form, $form_state);
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

  nquire_commont_create_form_delete_cancel_buttons($form, 'inquiry_creator_measure_delete_form_submit', "creator/{$design->nid}/activity/{$activity->nid}");

  return $form;
}

function inquiry_creator_measure_delete_form_submit($form, $form_state) {
  global $user;

  $measure_nid = $form_state['values']['measure_id'];

  if (_inquiry_creator_measure_delete_node($measure_nid)) {
    $activity = node_load($form_state['values']['activity_id']);
    if ($activity && $activity->type === 'inquiry_activity' && $user->uid === $activity->uid) {
      $data = nquire_commons_decode_data($activity->data);
      if (isset($data['measures'])) {
        if (($key = array_search($measure_nid, $data['measures'], FALSE)) !== FALSE) {
          unset($data['measures'][$key]);
          $activity->data = json_encode($data);
          $activity->revision = TRUE;
          node_save($activity);
        }
      }
    }
  }

  nquire_commons_form_close($form, $form_state);
}

function _inquiry_creator_measure_delete_node($nid) {
  global $user;

  $measure = node_load($nid);

  if ($measure && $measure->type === 'inquiry_measure' && $measure->uid == $user->uid) {
    node_delete($nid);
    return TRUE;
  }

  return FALSE;
}


<?php

function theme_inquiry_creator_settings_panel($inquiry_node) {

  $sections = array();

  /* access */

  $sections['access'] = array('title' => t('Inquiry access:'), 'items' => array());
  $inquiry_selection = pi_inquiry_groupings_get_selection_type($inquiry_node);
  switch ($inquiry_selection) {
    case 'justme':
      $sections['access']['items'][] = t('This inquiry is just for myself. ');
      $show_selective = FALSE;
      break;
    case 'group':
      $cohort = node_load(pi_inquiry_groupings_get_cohort_og($inquiry_node));
      og_load_group($cohort);

      $sections['access']['items'][] = t('This inquiry is for members of group: <b>@group</b>. ', array('@group' => $cohort->og_description));
      $show_selective = TRUE;
      break;
    case 'anyone':
      $sections['access']['items'][] = t('This inquiry is for anyone. ');
      $show_selective = TRUE;
      break;
  }

  if ($show_selective) {
    switch (pi_inquiry_groupings_get_group_selective_type($inquiry_node)) {
      case OG_OPEN:
        $sections['access']['items'][] = $inquiry_selection === 'group' ? t('Access to the inquiry is open for group members.') : t('Access to the inquiry is open, users can join without need for approval.');
        break;
      case OG_MODERATED:
        $sections['access']['items'][] = t('Access needs to be approved by the inquiry owner.');
        break;
      case OG_INVITE_ONLY:
        $sections['access']['items'][] = t('Access is through invitation only.');
        break;
    }
  }

  /* monitoring */

  $sections['supervised'] = array('title' => t('Inquiry supervision:'), 'items' => array());
  if ($inquiry_node->supervised) {
    $sections['supervised']['items'][] = t('This inquiry is monitored.');
  } else {
    $sections['supervised']['items'][] = t('This inquiry is not monitored.');
  }

  /* groups */

  $has_collaborative_phases = pi_inquiry_groupings_has_collaboration_phases($inquiry_node->nid, 'group');
  if ($has_collaborative_phases) {
    $sections['groups'] = array('title' => t('Group management:'), 'items' => array());

    $normal_group_management = $inquiry_node->group_creation === 'normal';

    if ($normal_group_management) {
      $sections['groups']['items'][] = t('Groups are self-managed by inquiry members.');
      $sections['groups']['items'][] = t('To view the current groups, go to the <a href="!url">live inquiry</a>.', array(
          '!url' => url("inquiry/{$inquiry_node->nid}/participants"),
      ));
    } else {
      $sections['groups']['items'][] = t('Groups are managed by inquiry owner.');
      $sections['groups']['items'][] = t('To manage the groups, go to the <a href="!url">live inquiry</a>.', array(
          '!url' => url("inquiry/{$inquiry_node->nid}/participants"),
      ));
    }
  }

  /* access */

  $output = '';
  foreach ($sections as $section) {
    $output .= '<p>' . $section['title'] . '</p>' . theme('item_list', $section['items']);
  }

  $output .= '<p>' . l(t('Change settings'), "creator/{$inquiry_node->nid}/settings") . '</p>';

  return $output;
}

function _inquiry_creator_settings_panel_form(&$form_state, $inquiry_node) {
  $form = array();

  ahah_helper_register($form, $form_state);

  nquire_commons_load_ahah_fix();

  $current_participants_selection = $inquiry_node->participants_selection;
  $participants_selection = nquire_commons_form_node_default_value($inquiry_node, 'participants_selection', $form_state, 'target', 'justme');

  $has_changed = $current_participants_selection !== $participants_selection;

  $form['target'] = array(
      '#type' => 'select',
      '#title' => t('Inquiry access'),
      '#description' => t('Select the group of users that may become participants of this inquiry.'),
      '#options' => array('justme' => t('Just me'), /* 'group' => t('Members of a group'), */ 'anyone' => t('Anyone')),
      '#default_value' => $participants_selection,
      '#ahah' => array(
          'path' => ahah_helper_path(array('target_dependent_section')),
          'wrapper' => 'target_dependent_section',
          'method' => 'replace',
          'effect' => 'fade',
      ),
      '#required' => TRUE,
  );

  nquire_commons_create_form_wrapped_section($form, 'target_dependent_section');


  $participants_cohort = nquire_commons_form_node_default_value($inquiry_node, 'participants_cohort', $form_state, 'cohort_group', 0);

  if ($participants_selection === 'group') {
    $groups = array('' => t('- Select group')) + pi_inquiry_groupings_get_normal_groups();

    $form['target_dependent_section']['wrapped_content']['cohort_group'] = array(
        '#type' => 'select',
        '#title' => t('User group'),
        '#description' => t('Select the group whose participants may join this inquiry.'),
        '#options' => $groups,
        '#default_value' => $participants_cohort,
        '#required' => TRUE,
        '#ahah' => array(
            'path' => ahah_helper_path(array('target_dependent_section', 'wrapped_content', 'cohort_dependent_section')),
            'wrapper' => 'cohort_dependent_section',
            'method' => 'replace',
            'effect' => 'fade',
        ),
    );

    if (!$has_changed) {
      $has_changed = $participants_cohort !== $inquiry_node->participants_cohort;
    }
  }


  pi_inquiry_manager_participants_create_group_join_mode_form_item($form['target_dependent_section']['wrapped_content'], $form_state, $inquiry_node->actor_gid, 'join_mode', t('How to join the inquiry'), t('Select how users can join this inquiry.'), $participants_selection === 'justme', FALSE);

  nquire_commons_create_form_wrapped_section($form['target_dependent_section']['wrapped_content'], 'cohort_dependent_section');
  if ($has_changed) {
    $to_be_expelled = array();
    $new_allowed_members = pi_inquiry_groupings_get_allowed_participants($participants_selection, $inquiry_node->uid, $participants_cohort);
    if ($new_allowed_members !== FALSE) {
      $current_members = nquire_group_get_member_uids($inquiry_node->actor_gid);
      foreach ($current_members as $member_uid) {
        if (!in_array($member_uid, $new_allowed_members)) {
          $to_be_expelled[] = $member_uid;
        }
      }
    }

    if (count($to_be_expelled) > 0) {
      $message = '<div><p>' . t('The following users will be expelled from the inquiry:') . '</p>'
              . theme_item_list(array_map(function($uid) {
                        $u = user_load($uid);
                        return check_plain($u->name);
                      }, $to_be_expelled))
              . '<div>';

      $form['target_dependent_section']['wrapped_content']['cohort_dependent_section']['wrapped_content'] = array(
          '#value' => $message,
      );
    }
  }



  $form['supervision_intro'] = array(
      '#value' => '<b>' . t('Inquiry supervision:') . '<b>',
  );
  
  $form['supervision'] = array(
      '#type' => 'checkbox',
      '#title' => t('Inquiry monitoring'),
      '#description' => t('Check this option if you wish to be the supervisor of this inquiry. This feature can be used to give the inquiry owner (e.g., a teacher) access to content that is private for the rest or participants.'),
      '#default_value' => nquire_commons_form_node_default_value($inquiry_node, 'supervised', $form_state, 'supervision', FALSE),
      '#required' => FALSE,
  );


  $has_collaborative_phases = pi_inquiry_groupings_has_collaboration_phases($inquiry_node->nid, 'group');
  if ($has_collaborative_phases) {
    $policy = nquire_commons_form_node_default_value($inquiry_node, 'group_creation', $form_state, 'group_creation', 'controlled');

    $form['group_creation'] = array(
        '#type' => 'select',
        '#title' => t('Group management'),
        '#description' => t('Select who is responsible for group formation.'),
        '#options' => array('controlled' => t('Controlled by owner'), 'normal' => t('Free (self-managed by inquiry members)')),
        '#default_value' => $policy,
        '#required' => TRUE,
    );
  }


  nquire_commons_create_form_save_cancel_buttons($form, 'inquiry_creator_settings_panel_form_submit', "creator/{$inquiry_node->nid}");

  return $form;
}

function _inquiry_creator_settings_panel_form_submit($form, $form_state) {

  $inquiry_nid = arg(1);
  if (pi_inquiry_manager_participants_group_configure_inquiry_participants_access($inquiry_nid)) {
    $inquiry = node_load($inquiry_nid);
    $new_participant_selection = $form_state['values']['target'];
    $new_participant_cohort = $form_state['values']['cohort_group'];

    $inquiry->participants_selection = $new_participant_selection;
    $inquiry->participants_cohort = $new_participant_cohort;

    $inquiry->supervised = $form_state['values']['supervision'] ? TRUE : FALSE;

    if (isset($form_state['values']['group_creation'])) {
      $inquiry->group_creation = $form_state['values']['group_creation'];
    }

    $inquiry->revision = TRUE;
    node_save($inquiry);

    nquire_group_set_join_mode($inquiry->actor_gid, $form_state['values']['join_mode']);
  }

  nquire_commons_form_close($form, $form_state);
}

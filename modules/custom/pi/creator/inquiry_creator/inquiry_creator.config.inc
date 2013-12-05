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

  $sections['supervised'] = array('title' => t('Monitoring:'), 'items' => array());
  if ($inquiry_node->supervised) {
    $sections['supervised']['items'][] = t('This inquiry is monitored.');
  } else {
    $sections['supervised']['items'][] = t('This inquiry is not monitored.');
  }

  /* groups */

  $has_collaborative_phases = pi_inquiry_groupings_has_collaboration_phases($inquiry_node->nid, 'group');
  if ($has_collaborative_phases) {
    $sections['groups'] = array('title' => t('Groups:'), 'items' => array());

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

  return $output;
}

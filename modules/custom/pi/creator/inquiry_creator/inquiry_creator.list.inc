<?php

function inquiry_creator_home() {

  global $user;

  $designs = nquire_commons_nodes_by_type_and_user('pi_inquiry', $user->uid);

  $admin_groups = pi_inquiry_groupings_get_groups_for_user($user->uid, NQUIRE_GROUP_TYPE_INQUIRY, TRUE, TRUE);
  $admin_designs = pi_inquiry_groupings_get_inquiries_for_groups($admin_groups['active']);
  foreach ($admin_designs as $nid) {
    if (!in_array($nid, $designs)) {
      $designs[$nid] = node_load($nid);
    }
  }

  return theme('inquiry_creator_design_list', $designs);
}

function theme_inquiry_creator_design_list($design_list) {
  $output = '<p>' . t('Create and edit new inquiries.') . '</p>';

  $help_story = pi_promote_get_story('help_authoring_tool');
  if ($help_story) {
    $help_content = t('Read the <a href="!url" target="_blank">tutorial</a> to learn about creating and editing your own inquiries.', array(
        '!url' => url("node/$help_story"),
    ));
    $output .= nquire_commons_create_page_section($help_content, t('Learn'));
  }

  $title = t('My designs');

  if (count($design_list) == 0) {
    $content = t('You have no designs. Click <a href="!url">here</a> to create your first design.', array('!url' => url('creator/new')));
  } else {
    global $user;

    $header = array(t('Title'), t('Description'));
    $rows = array();
    foreach ($design_list as $nid => $node) {
      $links = l(t('edit'), "creator/$nid");
      $inquiry_title = '<b>' . $node->title . '</b>';
      if ($node->uid !== $user->uid) {
        $inquiry_title .= '<br/><small>' . t('Created by !user', array('!user' => l(nquire_commons_user_name_or_me($node->uid), 'user/' . $node->uid, array('html' => TRUE)))) . '</small>';
      }
      $inquiry_title .= '<br/><small>' . $links . '</small>';

      $rows[] = array(
          $inquiry_title,
          nl2br(check_plain($node->body)),
      );
    }
    $content = nquire_commons_create_not_expanding_table_block(theme_table($header, $rows))
            . l(t('Create new inquiry'), 'creator/new');
  }

  $output .= nquire_commons_create_page_section($content, $title);

  return $output;
}

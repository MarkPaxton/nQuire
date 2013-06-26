<?php

function inquiry_creator_home() {

  global $user;

  $designs = nquire_commons_nodes_by_type_and_user('pi_inquiry', $user->uid);

  return theme('inquiry_creator_design_list', $designs);
}

function theme_inquiry_creator_design_list($design_list) {
  $output = '<p>' . t('Create and edit new inquiries.') . '</p>';

  $title = t('My designs');

  if (count($design_list) == 0) {
    $content = t('You have no designs. Click <a href="!url">here</a> to create your first design.', array('!url' => url('creator/new')));
  } else {
    $header = array(t('Title'), t('Description'));

    $rows = array();
    foreach ($design_list as $nid => $node) {
      $links = l(t('edit'), "creator/$nid");
      $rows[] = array(
          '<b>' . $node->title . '</b><br/><small>' . $links . '</small>',
          nl2br(check_plain($node->body)),
      );
    }
    $content = nquire_commons_create_not_expanding_table_block(theme_table($header, $rows))
            . l(t('Create new inquiry'), 'creator/new');
  }

  $output .= nquire_commons_create_page_section($content, $title);

  return $output;
}

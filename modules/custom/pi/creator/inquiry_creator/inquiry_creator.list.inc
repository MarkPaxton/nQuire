<?php

function inquiry_creator_home() {

  global $user;

  $designs = nquire_commons_nodes_by_type_and_user('inquiry_design', $user->uid);

  return theme('inquiry_creator_design_list', $designs);
}

function theme_inquiry_creator_design_list($design_list) {
  $header = array(
      t('Title'),
      t('Description'),
  );


  $rows = array();
  foreach ($design_list as $nid => $node) {
    $links = l(t('edit'), "creator/$nid") . '&nbsp;&nbsp;' . l(t('delete'), "creator/$nid/delete");
    $rows[] = array(
        '<b>' . $node->title . '</b><br/><small>' . $links . '</small>',
        $node->body,
    );
  }

  $output = t('My designs: ');
  $output .= theme_table($header, $rows);
  $output .= l(t('Create new inquiry'), 'creator/new');

  return $output;
}

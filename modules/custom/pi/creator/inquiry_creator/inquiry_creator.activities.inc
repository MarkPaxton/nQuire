<?php


function inquiry_creator_activity_edit($design_nid, $activity_nid) {
  $design = node_load($design_nid);
  $node = node_load($activity_nid);

  $output = theme('inquiry_creator_inquiry_main', $design, 'activities');
  
  //$form = drupal_get_form('inquiry_activity_editor_activity_form', $design, $node);
  $form = 'hey!';
  
  $output .= theme('inquiry_creator_side_form', $design, $activity_nid, $form);
  return $output;
}


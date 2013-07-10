<?php

function pi_inquiry_manager_promote() {
  if (pi_info()->getCurrentPath()->inquiry_nid) {
    $output = drupal_get_form('pi_promote_inquiry_form', pi_info()->getCurrentPath()->inquiry_nid, 'inquiry/' . pi_info()->getCurrentPath()->inquiry_nid . '/promote');
    return $output;
  }
}

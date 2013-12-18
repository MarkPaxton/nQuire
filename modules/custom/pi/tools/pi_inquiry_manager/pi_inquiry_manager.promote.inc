<?php

function pi_inquiry_manager_promote() {
  if (pi_info()->getCurrentPath()->inquiry_nid) {
    $output = drupal_get_form('pi_promote_inquiry_form', pi_info()->getCurrentPath()->inquiry_nid, 'inquiry/' . pi_info()->getCurrentPath()->inquiry_nid . '/promote');
    return $output;
  }
}

function pi_inquiry_manager_rate() {
  $inquiry_info = pi_info()->getCurrentInquiryInfo();
  if ($inquiry_info) {
    global $user;
    $is_admin = $inquiry_info->userIsInquiryAdmin($user->uid);

    if ($is_admin) {
      $output = '<p>' . t('Authors cannot rate their own inquiries.') . '</p>';
      $ratings = pi_promote_inquiry_ratings($inquiry_info->getInquiryNid());
      $rows = array();
      $total = 0;
      foreach ($ratings as $rating => $count) {
        $rows[] = array($rating, $count);
        $total += $count;
      }
      $rows[] = array(t('Total:'), $total);
      
      $header = array(t('Rating'), t('Vote count'));
      $output .= nquire_commons_create_not_expanding_table_block(theme('table', $header, $rows));
    } else {
      $output = drupal_get_form('pi_promote_inquiry_rate_form', $inquiry_info->getInquiryNid(), 'inquiry/' . $inquiry_info->getInquiryNid() . '/rate');
    }
    
    return $output;
  }
}

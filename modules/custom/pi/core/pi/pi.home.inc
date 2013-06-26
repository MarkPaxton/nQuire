<?php

/** @file This file contains all the functions to display the nQuire home view; this is the
 * page with all the inquiries listed.
 */

/**
 * pi_home_view - Home page for PI that lists all the inquiries
 */
function pi_home_view() {
  drupal_set_title("My inquiries");
  $output = '<p>' . t('Browse the list of available inquiries for you.') . '</p>';

  global $user;

  global $user;
  $inquiries_for_user = pi_inquiry_groupings_get_inquiries_for_user($user->uid);

  $active_inquiries = array_merge($inquiries_for_user['joined'], $inquiries_for_user['awaiting']);

  if (count($active_inquiries) > 0) {
    $title = t('Active inquiries');
    $intro = t('Inquiries you are currently engaged with:');

    $header = array(t('Inquiry'), t('Status'));

    $rows = array();
    foreach ($active_inquiries as $inquiry_nid) {
      $inquiry = node_load($inquiry_nid);
      $rows[] = _pi_home_inquiry_presentation_in_list($inquiry);
    }

    $output .= nquire_commons_create_page_section($intro 
            . nquire_commons_create_not_expanding_table_block(theme('table', $header, $rows), $title));
  }


  if (count($inquiries_for_user['available']) > 0) {
    $title = t('Available inquiries');
    $intro = t('Inquiries you can join:');

    $header = array(t('Inquiry'), t('Join'));

    $rows = array();
    foreach ($inquiries_for_user['available'] as $inquiry_nid) {
      $inquiry = node_load($inquiry_nid);
      $rows[] = _pi_home_inquiry_presentation_in_list($inquiry);
    }

    $output .= nquire_commons_create_page_section($intro 
            . nquire_commons_create_not_expanding_table_block(theme('table', $header, $rows), $title));
  }
  return $output;
}

function _pi_home_inquiry_presentation_in_list($inquiry) {
  global $user;

  $status = theme('pi_inquiry_manager_participants_group_status_and_options', $inquiry, $user->uid);

  $output = l(check_plain($inquiry->title), "inquiry/{$inquiry->nid}", array('attributes' => array('style' => 'font-size: 1.5em;font-weight: bold;')))
          . '<div style="padding: .5em 0 .5em 1em;">' . nl2br(check_plain($inquiry->body)) . '</div>'
          . '<div style="padding-left: 1em;">'
          . '<small>' . t('Created by !user', array('!user' => l(nquire_commons_user_name_or_me($inquiry->uid), 'user/' . $inquiry->uid, array('html' => TRUE)))) . '<br/>'
          . t('Number of participants: !n', array('!n' => count(nquire_group_get_member_uids($inquiry->actor_gid, TRUE)))) . '</small>'
          . '</div>';

  return array($output, $status);
}

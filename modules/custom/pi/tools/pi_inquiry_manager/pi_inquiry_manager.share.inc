<?php

function pi_inquiry_manager_share() {
  if (pi_info()->getCurrentPath()->inquiry_nid) {
    $output = drupal_get_form('pi_inquiry_manager_share_content_form');
    return $output;
  }
}

function pi_inquiry_manager_share_content_form(&$form_state) {
  $form = array();

  $inquiry_info = pi_info()->getCurrentInquiryInfo();
  if ($inquiry_info) {
    global $user;

    $share_policy = $inquiry_info->getUserSharePolicy($user->uid);

    $form['introinquiry'] = array(
        '#value' => '<h3>' . t('Share content with inquiry participants') . '</h3>',
    );

    $value = array(
        array('id' => 'members', 'title' => t('Share with inquiry members'), 'values' => $share_policy['members']),
    );

    if ($inquiry_info->getInquiryNode()->supervised) {
      $value[] = array('id' => 'supervisor', 'title' => t('Inquiry supervisor can see'), 'values' => $share_policy['supervisor']);
    }

    nquire_structure_widget_share_content_table_create_form_item($form, 'sharecontent', json_encode($share_policy['structure']), json_encode($value));


    nquire_commons_create_form_save_cancel_buttons($form, 'pi_inquiry_manager_share_content_form_submit', 'inquiry/' . $inquiry_info->getInquiryNid() . '/share');
  }
  return $form;
}

function pi_inquiry_manager_share_content_form_submit($form, $form_state) {
  global $user;
  $inquiry_info = pi_info()->getCurrentInquiryInfo();
  $access_manager = pi_info()->getAccessManager($inquiry_info->getInquiryNid(), $user->uid);

  $sc = nquire_commons_decode_data($form_state['values']['sharecontent']);
  $query = "INSERT INTO pi_activity_share (activity_id, actor, mode, share) VALUES (%d, %d, '%s', '%s') ON DUPLICATE KEY UPDATE share='%s'";
  foreach ($sc as $section) {
    if (isset($section['values'])) {
      foreach ($section['values'] as $anid => $value) {
        $activity = $inquiry_info->getActivity($anid);
        if ($activity) {
          switch ($inquiry_info->getActivityCollaboration($anid)) {
            case 'individual':
              db_query($query, $anid, $user->uid, $section['id'], $value, $value);
              break;
            case 'group':
              if ($access_manager->isGroupOwner()) {
                db_query($query, $anid, $access_manager->getSubgroup(), $section['id'], $value, $value);
              }
              break;
            default:
              if ($access_manager->isInquiryOwner()) {
                db_query($query, $anid, $inquiry_info->getInquiryActorGid(), $section['id'], $value, $value);
              }
              break;
          }
        }
      }
    }
  }
}


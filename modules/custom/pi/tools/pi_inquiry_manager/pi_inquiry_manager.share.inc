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
  $current_policy = $inquiry_info->getUserSharePolicy($user->uid);
  $content_manager = pi_info()->getContentManager();

  $sc = nquire_commons_decode_data($form_state['values']['sharecontent']);


  $changes = array();


  foreach ($sc as $section) {
    if (isset($section['values'])) {
      foreach ($section['values'] as $anid => $value) {
        if ($value != $current_policy[$section['id']][$anid]) {
          switch ($inquiry_info->getActivityCollaboration($anid)) {
            case 'individual':
              $actor = $user->uid;
              break;
            case 'group':
              if ($access_manager->isGroupOwner()) {
                $actor = $access_manager->getSubgroup();
              }
              break;
            default:
              if ($access_manager->isInquiryOwner()) {
                $actor = $inquiry_info->getInquiryActorGid();
              }
              break;
          }
          if ($actor) {
            if (!isset($changes[$anid])) {
              $changes[$anid] = array();
            }
            $changes[$anid][$section['id']] = array($actor, $value);
          }
        }
      }
    }
  }

  $query = "INSERT INTO pi_activity_share (activity_id, actor, mode, share) VALUES (%d, %d, '%s', '%s') ON DUPLICATE KEY UPDATE share='%s'";
  foreach ($changes as $activity_nid => $change) {
    $shared = FALSE;
    foreach ($change as $mode => $data) {
      $shared = $shared || substr($data[1], -2) !== 'no';
      db_query($query, $activity_nid, $data[0], $mode, $data[1], $data[1]);
    }

    $content_nid = $content_manager->getContent($activity_nid, $user->uid, FALSE);
    if ($content_nid) {
      pi_feeds_share_activity_change($activity_nid, $content_nid, $shared);
    }
  }

  return;
}


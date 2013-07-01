<?php

function pi_activities_page($node, $mode, $child_node = NULL) {
  global $user;

  switch ($mode) {
    case 'phase':
      $ok = $node->type === 'pi_phase';
      break;
    case 'edit':
    case 'child_add':
    case 'child_edit':
    case 'child_delete':
      $ok = $node->type === 'pi_activity';
      break;
  }

  if ($ok) {
    include_once(drupal_get_path('module', 'node') . '/node.pages.inc');

    $inquiry_info = pi_info()->getCurrentInquiryInfo();
    $inquiry_access = pi_info()->getAccessManager($inquiry_info->getInquiryNid(), $user->uid);

    $phase_node = $mode === 'phase' ? $node : $inquiry_info->getPhase($node->phase_nid);
    $activity_node = $mode === 'phase' ? NULL : $node;

    $shared_view = $node->type === 'pi_phase' && arg(2) === 'shared' && $phase_node->collaboration !== 'all';

    pi_last_visited_phase_set($user->uid, $inquiry_info->getInquiryNid(), $phase_node->nid);

    $phase_key = $inquiry_info->getPhaseKey($phase_node->nid);

    if ($mode === 'child_edit' || 'child_add') {
      $separate_f = "{$activity_node->activity_type}_edit_child_in_separate_page";
      $separate_page = function_exists($separate_f) && $separate_f($activity_node);
    }

    $data = array(
        'in_phase' => !$separate_page,
        'shared_view' => $shared_view,
        'phase' => array(
            'node' => $phase_node,
            'phase_key' => $phase_key,
            'title' => theme('pi_inquiry_phase_title', $inquiry_info, $phase_node),
            'description' => nl2br(check_plain($phase_node->body)),
        ),
        'activities' => array(),
    );



    if ($data['in_phase']) {
      $link = l(t('here'), $shared_view ? "phase/{$phase_node->nid}" : "phase/{$phase_node->nid}/shared");
      switch ($phase_node->collaboration) {
        case 'individual':
          $data['phase']['sharing'] .= $shared_view ?
                  t('This is the content shared by other participats in this phase. Click !link to go to your activities.', array('!link' => $link)) :
                  t('Activities in this phase are carried out individually. Click !link to see what other participants are doing.', array('!link' => $link));
          break;
        case 'group':
          $subgroup = $inquiry_access->getSubgroup();
          if ($subgroup) {
            $members = nquire_group_get_member_uids($subgroup, TRUE);
            if (count($members) === 1 && $members[0] == $user->uid) {
              $data['phase']['sharing'] .= $shared_view ?
                      t('This is the content shared by other groups. Click !link to go to your activities.', array('!link' => $link)) :
                      t('I\'m carrying out activities in this phase by myself (only me in my group). Click !link to see what other groups are doing.', array('!link' => $link));
            } else {
              $data['phase']['sharing'] .= $shared_view ?
                      t('This is the content shared by other groups. Click !link to go to your group activities.', array('!link' => $link)) :
                      t('I\'m carrying out activities in this phase with members of my group: ') . implode(', ', nquire_commons_user_names($members, $user->uid, FALSE, TRUE)) . t('. Click !link to see what other groups are doing.', array('!link' => $link));
            }
          } else {
            $data['phase']['sharing'] .= $shared_view ?
                    t('This is the content shared by other groups. Click !link to go back.', array('!link' => $link)) :
                    t('Activities in this phase are carried out in groups (you do not belong to a group yet). Click !link to see what other groups are doing.', array('!link' => $link));
          }
          break;
        default:
          $data['phase']['sharing'] .= t('Activities in this phase are done by all inquiry participants working together.');
          break;
      }
    }


    $activities = $mode === $separate_page ? array($activity_node->nid => $activity_node) : $inquiry_info->getActivitiesForPhase($phase_node->nid);

    foreach ($activities as $a_nid => $a_node) {
      $active_activity = $a_nid == $node->nid;
      $data['activities'][$a_nid] = _pi_activities_get_activity_data($inquiry_info, $inquiry_access, $user, $phase_key, $separate_page, $a_node, $active_activity, $mode, $child_node, $shared_view);
    }

    $output = theme('pi_activities_view_phase', $data);

    return $output;
  } else {
    return '';
  }
}

function _pi_activities_get_activity_data($inquiry_info, $inquiry_access, $user, $phase_key, $separate_page, $activity_node, $active_activity, $mode, $child_node, $shared_view) {
  $access = $inquiry_access->getAccessToItem($activity_node);
  $reason = $inquiry_access->getAccessReason($activity_node);
  $inquiry_nid = $inquiry_info->getInquiryNid();
  $exaplanation = pi_activity_access_get_explanation_text($access, $reason, $inquiry_nid);
  $can_view = $active_activity ? pi_activity_tab_edit_access_callback($activity_node) : pi_activity_tab_view_access_callback($activity_node);

  $content_manager = pi_info()->getContentManager();

  if ($can_view && !$shared_view) {
    $content_nid = $content_manager->getContent($activity_node->nid, $user->uid, TRUE);
    $content_node = node_load($content_nid);
  }

  $data = array(
      'node' => $activity_node,
      'title' => check_plain($activity_node->title),
      'description' => nl2br(check_plain($activity_node->body)),
      'links' => $access === 'edit' ? pi_activity_create_edit_links($activity_node, $active_activity && $mode === 'edit') : NULL,
      'access' => $access,
      'reason' => $reason,
      'access_explanation' => $exaplanation,
      'phase_key' => $phase_key,
      'can_view' => $can_view,
      'shared_view' => $shared_view,
  );

  if ($can_view) {
    if ($shared_view) {
      $options = array(
          'edit' => FALSE,
          'child_edit' => NULL,
          'child_delete' => NULL,
          'add_child' => FALSE,
          'editable' => FALSE,
          'activity_node' => $activity_node,
          'separate_page' => FALSE,
      );

      $view_f = "{$activity_node->activity_type}_display_content_in_activity";
      $data['shared_content'] = array();
      $is_supervisor = $inquiry_info->getInquiryOwner() === $user->uid;
      $content_nids = $content_manager->getSharedContent($activity_node->nid, $user->uid, $is_supervisor);

      foreach ($content_nids as $content_nid) {
        $content_node = node_load($content_nid);
        $actor = $content_manager->getContentAuthor($content_nid);
        $data['shared_content'][] = array(
            'actor' => $actor,
            'content' => $view_f($content_node, $options),
        );
      }
    } else {
      $options = array(
          'edit' => $active_activity && $mode === 'edit',
          'child_edit' => $active_activity && $mode === 'child_edit' ? $child_node->nid : NULL,
          'child_delete' => $active_activity && $mode === 'child_delete' ? $child_node->nid : NULL,
          'add_child' => $active_activity && $mode === 'child_add',
          'editable' => $access === 'edit',
          'activity_node' => $activity_node,
          'separate_page' => $separate_page,
      );


      if ($mode === 'edit' && !$content_node) {
        global $user;
        $content_node = new stdClass();
        $content_node->type = $activity_node->activity_type;
        $content_node->uid = $user->uid;

        $options['new'] = TRUE;
      }

      $view_f = "{$activity_node->activity_type}_display_content_in_activity";
      $data['content'] = $view_f($content_node, $options);
    }
  }

  return $data;
}

function theme_pi_activities_view_phase($data) {
  $output = '';
  return $output;
}

function theme_pi_activities_view_activity($activity_data) {
  $output = '';
  return $output;
}

function theme_pi_activities_view_contribution_content($phases_data) {
  $output = '';
  return $output;
}

function theme_pi_activities_view_contributed_activity($activity_data) {
  $output = '';
  return $output;
}

function theme_pi_activities_view_shared_activity($activity_data) {
  $output = '';
  return $output;
}

function theme_pi_inquiry_phase_title($inquiry_info, $phase_node) {
  return t('Phase @index: @title', array(
      '@index' => 1 + $inquiry_info->getPhaseIndex($phase_node->nid),
      '@title' => $phase_node->title,
  ));
}
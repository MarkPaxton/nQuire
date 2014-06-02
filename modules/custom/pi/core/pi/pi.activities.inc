<?php

function pi_activities_page($node, $mode, $child_node = NULL) {
  global $user;

  switch ($mode) {
    case 'phase':
      $ok = $node->type === 'pi_phase';
      break;
    case 'edit':
    case 'feedback':
    case 'child_add':
    case 'child_edit':
    case 'child_delete':
      $ok = $node->type === 'pi_activity';
      break;
  }

  if ($ok) {
    include_once(drupal_get_path('module', 'node') . '/node.pages.inc');

    $inquiry_info = pi_info()->getCurrentInquiryInfo();
    $inquiry_nid = $inquiry_info->getInquiryNid();
    $inquiry_access = pi_info()->getAccessManager($inquiry_nid, $user->uid);

    $phase_node = $mode === 'phase' ? $node : $inquiry_info->getPhase($node->phase_nid);
    $activity_node = $mode === 'phase' ? NULL : $node;

    $feedback_edit = $node->type === 'pi_activity' && $mode === 'feedback';
    $shared_view = $phase_node->collaboration !== 'all' &&
            (($node->type === 'pi_phase' && arg(2) === 'shared') || $feedback_edit);



    pi_last_visited_phase_set($user->uid, $inquiry_nid, $phase_node->nid);

    $phase_key = $inquiry_info->getPhaseKey($phase_node->nid);

    if ($mode === 'child_edit' || 'child_add') {
      $separate_f = "{$activity_node->activity_type}_edit_child_in_separate_page";
      $separate_page = function_exists($separate_f) && $separate_f($activity_node);
    }

    $data = array(
        'in_phase' => !$separate_page,
        'shared_view' => $shared_view,
        'feedback_edit' => $feedback_edit,
        'phase' => array(
            'node' => $phase_node,
            'phase_key' => $phase_key,
            'title' => check_plain($phase_node->title),
            'formatted_title' => theme('pi_inquiry_phase_title', $inquiry_info, $phase_node),
            'description' => nl2br(check_plain($phase_node->body)),
        ),
        'activities' => array(),
    );



    if ($data['in_phase']) {
      $link_attrs = array('attributes' => array('class' => "nquire_button_link nquire_button_link_phase_banner"));
      $link = $shared_view ? l(t('Go'), "phase/{$phase_node->nid}", $link_attrs) : l(t('View'), "phase/{$phase_node->nid}/shared", $link_attrs);

      switch ($phase_node->collaboration) {
        case 'individual':
          $data['phase']['sharing'] .= $shared_view ?
                  t('Shared content. !link to your activities.', array('!link' => $link)) :
                  t('Individual activities. !link work from other participants.', array('!link' => $link));
          break;
        case 'group':
          $subgroup = $inquiry_access->getSubgroup();
          if ($subgroup) {
            $members = nquire_group_get_member_uids($subgroup, TRUE);
            if ($shared_view) {
              $data['phase']['sharing'] .= t('Shared content. !link to your activities.', array('!link' => $link));
            } else {
              $group_link = l(t('View'), $shared_view ? "phase/{$phase_node->nid}" : "inquiry/$inquiry_nid/participants/$subgroup", array('attributes' => array('class' => "nquire_button_link nquire_button_link_phase_banner")));
              $data['phase']['sharing'] .= t('Group activities. !link work from other groups. !link2 your group configuration.', array('!link' => $link, '!link2' => $group_link));
            }
          } else {
            $group_link = l(t('Create or join'), $shared_view ? "phase/{$phase_node->nid}" : "inquiry/$inquiry_nid/participants", array('attributes' => array('class' => "nquire_button_link nquire_button_link_phase_banner")));
            $data['phase']['sharing'] .= $shared_view ?
                    t('Shared content. !link to your activities.', array('!link' => $link)) :
                    t('Group activities: !group_link a group. !link work from other groups.', array('!link' => $link, '!link2' => $group_link));
          }
          break;
        default:
          $data['phase']['sharing'] .= t('All participants work together in this phase.');
          break;
      }
    }


    $activities = $separate_page ? array($activity_node->nid => $activity_node) : $inquiry_info->getActivitiesForPhase($phase_node->nid);

    foreach ($activities as $a_nid => $a_node) {
      $active_activity = $a_nid == $node->nid;
      $data['activities'][$a_nid] = _pi_activities_get_activity_data($inquiry_info, $inquiry_access, $user, $phase_key, $separate_page, $a_node, $active_activity, $mode, $child_node, $shared_view, $feedback_edit);
    }

    $output = theme('pi_activities_view_phase', $data);

    return $output;
  } else {
    return '';
  }
}

function _pi_activities_get_activity_data($inquiry_info, $inquiry_access, $user, $phase_key, $separate_page, $activity_node, $active_activity, $mode, $child_node, $shared_view, $feedback_edit) {
  $access = $inquiry_access->getAccessToItem($activity_node);
  $reason = $inquiry_access->getAccessReason($activity_node);
  $inquiry_nid = $inquiry_info->getInquiryNid();
  $exaplanation = pi_activity_access_get_explanation_text($access, $reason, $inquiry_nid);
  $edit_mode = $active_activity && pi_activity_tab_edit_access_callback($activity_node);
  $can_view = $edit_mode ? pi_activity_tab_edit_access_callback($activity_node) : pi_activity_tab_view_access_callback($activity_node);

  $content_manager = pi_info()->getContentManager();

  if ($can_view && !$shared_view) {
    $content_nid = $content_manager->getContent($activity_node->nid, $user->uid, TRUE);
    $content_node = node_load($content_nid);
  }

  $activity_is_being_edited = $edit_mode && $active_activity && $mode === 'edit';


  $links_block = NULL;
  $links_edit_content_click = FALSE;

  if ($access === 'edit') {
    $links_block_f = "{$activity_node->activity_type}_format_edit_links_block";
    if (function_exists($links_block_f)) {
      $links_block = $links_block_f($activity_node, $activity_is_being_edited);
    }

    if (!$links_block) {
      $links_position_f = "{$activity_node->activity_type}_links_block_position";
      $links_position = function_exists($links_position_f) ? $links_position_f($activity_node, $activity_is_being_edited) : 'after';

      $links_edit_content_click_f = "{$activity_node->activity_type}_links_edit_content_click";
      $content_link_mode = !$edit_mode && function_exists($links_edit_content_click_f) ? $links_edit_content_click_f() : FALSE;
      $links_edit_content_click = $content_link_mode === TRUE || ($content_link_mode === 'create' && !$content_node);


      if (!$links_edit_content_click) {
        $links_block = array(
            'position' => $links_position,
            'content' => pi_activity_create_edit_links($inquiry_info, $content_node, $activity_node, $edit_mode),
        );
      }
    }
  }

  $data = array(
      'node' => $activity_node,
      'title' => check_plain($activity_node->title),
      'description' => nl2br(check_plain($activity_node->body)),
      'links' => $links_block,
      'access' => $access,
      'reason' => $reason,
      'access_explanation' => $exaplanation,
      'phase_key' => $phase_key,
      'can_view' => $can_view,
      'shared_view' => $shared_view,
      'content_click_edit' => $links_edit_content_click,
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
      $is_supervisor = $inquiry_info->userIsInquiryAdmin($user->uid);

      $content_nids = $content_manager->getSharedContent($activity_node->nid, $user->uid, $is_supervisor);
      if ($feedback_edit && in_array($child_node->nid, $content_nids)) {
        $content_nids = array($child_node->nid);
      }

      foreach ($content_nids as $content_nid) {
        $content_node = node_load($content_nid);
        $actor = $content_manager->getContentAuthor($content_nid);

        if ($is_supervisor) {
          if ($feedback_edit) {
            $feedback_content = drupal_get_form("pi_feedback_in_activity_form", $activity_node->phase_nid, $child_node->nid);
          } else {
            $feedback_node = pi_feedback_for_content($content_nid);
            $feedback_path = "activity/{$activity_node->nid}/feedback/{$content_nid}";
            if ($feedback_node) {
              $feedback_content = '<div>' . nl2br(check_plain($feedback_node->feedback)) . '</div>'
                      . l('Edit feedback', $feedback_path);
            } else {
              $feedback_content = l('Add feedback', $feedback_path);
            }
          }
        } else {
          $feedback_content = FALSE;
        }

        $data['shared_content'][] = array(
            'actor' => $actor,
            'content' => $view_f($content_node, $options),
            'feedback' => $feedback_content
        );
      }
    } else {
      $options = array(
          'edit' => $edit_mode && $mode === 'edit',
          'child_edit' => $edit_mode && $mode === 'child_edit' ? $child_node->nid : NULL,
          'child_delete' => $edit_mode && $mode === 'child_delete' ? $child_node->nid : NULL,
          'add_child' => $edit_mode && $mode === 'child_add',
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
      $feedback_node = pi_feedback_for_content($content_node->nid);
      if ($feedback_node) {
        $data['feedback'] = nl2br(check_plain($feedback_node->feedback));
      }
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
  return check_plain($phase_node->title);
}

<?php

/* copy inquiry */

function inquiry_creator_copy_new() {
  global $user;

  $content = t('<h1 class="title">Copy existing inquiry</h1>')
          . '<p>' . t('Here you can select one existing inquiry.') . '</p>'
          . '<p>' . t('Title, description, front page and phases and activities from your selected inquiry will be used to create a new inquiry.') . '</p>'
          . '<p>' . t('Please note that users and the content they created will not be copied in the new inquiry.') . '</p>'
          . '<ul>';


  $inquiries = array_merge(pi_inquiry_groupings_get_inquiries_for_user($user->uid, TRUE), pi_inquiry_groupings_get_inquiries_for_user($user->uid, FALSE, FALSE));

  foreach ($inquiries as $nid) {
    $node = node_load($nid);
    if ($node) {
      $content .= '<li>'
              . check_plain($node->title)
              . "&nbsp;&nbsp;&nbsp;&nbsp;<small>" . l(t('View'), "inquiry/$nid", array('attributes' => array('target' => '_blank'))) . '</small>'
              . "&nbsp;&nbsp;&nbsp;&nbsp;<small>" . l(t('Copy inquiry'), "creator/copy/$nid") . '</small>'
              . '</li>';
    }
  }

  $content .= '</ul>';

  $output = $content;

  return $output;
}

function inquiry_creator_copy_new_confirm($source_nid) {
  $source_info = pi_info()->getInquiryInfo($source_nid);

  $content = t('<h1 class="title">Copy inquiry <i>@title</i></h1>', array('@title' => $source_info->getInquiryNode()->title))
          . '<p>' . t('The new copy will include the following phases and activities.') . '</p>'
          . '<ul>';

  foreach ($source_info->getPhases() as $phase_nid => $phase) {
    $content .= '<li>' . check_plain($phase->title) . '<ul>';
    foreach ($source_info->getActivitiesForPhase($phase_nid) as $activity) {
      $content .= '<li>' . check_plain($activity->title) . '</li>';
    }
    $content .= '</ul></li>';
  }

  $content .= '</ul>';

  $form = drupal_get_form('inquiry_creator_copy_new_confirm_form');
  $content .= $form;

  $output = $content;

  return $output;
}

function inquiry_creator_copy_new_confirm_form(&$form_state) {
  $form = array();

  nquire_commons_create_form_save_cancel_buttons($form, 'inquiry_creator_copy_new_form_submit', "creator/", t('Copy inquiry'));

  return $form;
}

function inquiry_creator_copy_new_form_submit($form, $form_state) {
  global $user;

  $source_nid = arg(2);
  $source_info = pi_info()->getInquiryInfo($source_nid);
  $source_node = $source_info->getInquiryNode();

  $node = new stdClass();
  $node->type = 'pi_inquiry';
  $node->uid = $user->uid;
  $node->title = $source_node->title . t(' (copy)');
  $node->body = '';$source_node->body;
  $node->forum = $source_node->forum;
  $node->frontpage = '';$source_node->frontpage;

  $node->participants_selection = $source_node->participants_selection;
  $node->participants_cohort = $source_node->participants_cohort;
  $node->group_creation = $source_node->group_creation;
  $node->supervised = $source_node->supervised;
  /* do not set up actor_gid, this is done when saving */

  $node->roles = '';

  node_save($node);
  
    /* TODO convert roles ids */
  $node->roles = '[]';
  $node->body = str_replace("inquiry/{$source_nid}", "inquiry/{$node->nid}", $source_node->body);
  $node->frontpage = str_replace("inquiry/{$source_nid}", "inquiry/{$node->nid}", $source_node->frontpage);
  node_save($node);
  
  
  $group = node_load($node->actor_gid);
  $source_group = node_load($source_node->actor_gid);
  $group->selective = $source_group->selective;
  node_save($group);
  
  

  $ids = array();

  foreach ($source_info->getPhases() as $source_phase_nid => $source_phase_node) {
    $phase_node = inquiry_creator_create_phase_stub($node->nid, $source_phase_node->title, $source_phase_node->weight, $source_phase_node->body, $source_phase_node->collaboration);
    $ids[$source_phase_node->nid] = $phase_node->nid;

    foreach ($source_info->getActivitiesForPhase($source_phase_nid) as $source_activity_node) {
      $activity_node = inquiry_creator_create_activity_stub($phase_node->nid, $source_activity_node->activity_type, $source_activity_node->title, $source_activity_node->weight, $source_activity_node->body);
      $ids[$source_activity_node->nid] = $activity_node->nid;
    }
  }

  $source_stage_nodes = $source_info->getStageNodes();
  foreach ($source_stage_nodes as $source_stage) {
    $stage = new stdClass();
    $stage->type = 'pi_stage';
    $stage->uid = $user->uid;
    $stage->inquiry_nid = $node->nid;
    $stage->title = $source_stage->title;
    $stage->phases = array();
    foreach ($source_stage->phases as $source_phase_nid => $value) {
      if (isset($ids[$source_phase_nid])) {
        $stage->phases[$ids[$source_phase_nid]] = $value;
      }
    }
    node_save($stage);
  }
  
  nquire_commons_form_close($form, $form_state);
}

/* copy structure */

function inquiry_creator_copy($design_nid) {
  global $user;

  $design = node_load($design_nid);

  $content = t('<h1 class="title">Copy structure from an existing inquiry</h1>')
          . '<p>' . t('Here you can select one existing inquiry.') . '</p>'
          . '<p>' . t('Phases and activities from your selected inquiry will be copied into your design.') . '</p>'
          . '<ul>';


  $inquiries = array_filter(array_merge(pi_inquiry_groupings_get_inquiries_for_user($user->uid, TRUE), pi_inquiry_groupings_get_inquiries_for_user($user->uid, FALSE, FALSE)), function($nid) use($design) {
    return $nid != $design->nid;
  });

  foreach ($inquiries as $nid) {
    $node = node_load($nid);
    if ($node) {
      $content .= '<li>'
              . check_plain($node->title)
              . "&nbsp;&nbsp;&nbsp;&nbsp;<small>" . l(t('View'), "inquiry/$nid", array('attributes' => array('target' => '_blank'))) . '</small>'
              . "&nbsp;&nbsp;&nbsp;&nbsp;<small>" . l(t('Copy structure'), "creator/$design_nid/copy/$nid") . '</small>'
              . '</li>';
    }
  }

  $content .= '</ul>';

  $output = theme('inquiry_creator_inquiry_main', $design, '', $content);

  return $output;
}

function inquiry_creator_copy_confirm($design_nid, $source_nid) {
  $design = node_load($design_nid);
  $source_info = pi_info()->getInquiryInfo($source_nid);


  $content = t('<h1 class="title">Copy structure from inquiry <i>@title</i></h1>', array('@title' => $source_info->getInquiryNode()->title))
          . '<p>' . t('The following phases and activities will be copied into your design.') . '</p>'
          . '<ul>';

  foreach ($source_info->getPhases() as $phase_nid => $phase) {
    $content .= '<li>' . check_plain($phase->title) . '<ul>';
    foreach ($source_info->getActivitiesForPhase($phase_nid) as $activity) {
      $content .= '<li>' . check_plain($activity->title) . '</li>';
    }
    $content .= '</ul></li>';
  }

  $content .= '</ul>';

  $form = drupal_get_form('inquiry_creator_copy_confirm_form', $design);
  $content .= $form;

  $output = theme('inquiry_creator_inquiry_main', $design, '', $content);

  return $output;
}

function inquiry_creator_copy_confirm_form(&$form_state, $design) {
  $form = array();

  _inquiry_creator_create_inquiry_id_form_item($design, $form);

  nquire_commons_create_form_save_cancel_buttons($form, 'inquiry_creator_copy_form_submit', "creator/{$design->nid}/", t('Copy structure'));

  return $form;
}

function inquiry_creator_copy_form_submit($form, $form_state) {
  $design_info = pi_info()->getInquiryInfo($form_state['values']['inquiry_design']);
  $weight = count($design_info->getPhases());

  $source_nid = arg(3);
  $source_info = pi_info()->getInquiryInfo($source_nid);

  foreach ($source_info->getPhases() as $source_phase_nid => $source_phase_node) {
    $phase_node = inquiry_creator_create_phase_stub($design_info->getInquiryNid(), $source_phase_node->title, $weight, $source_phase_node->body);
    $weight ++;

    foreach ($source_info->getActivitiesForPhase($source_phase_nid) as $source_activity_node) {
      inquiry_creator_create_activity_stub($phase_node->nid, $source_activity_node->activity_type, $source_activity_node->title, $source_activity_node->weight, $source_activity_node->body);
    }
  }

  nquire_commons_form_close($form, $form_state);
}

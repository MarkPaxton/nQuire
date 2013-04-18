<?php

/**
 * @file
 * User Relationships theme-able functions
 */

/**
 * Show the current status of a relationship
 */
function theme_user_relationships_approval_status($approved) {
  return isset($approved) ? ($approved ? t('Approved') : t('Not Approved')) : t('Unknown');
}

/**
 * Create relationship link
 */
function theme_user_relationships_request_relationship_link($relate_to) {
  return l(
    t('Create a relationship with %name', array('%name' => $relate_to->name)),
    "relationship/{$relate_to->uid}/request",
    array(
      'query' => drupal_get_destination(),
      'html'  => TRUE,
    )
  );
}

/**
 * Create a direct relationship link
 */
function theme_user_relationships_request_relationship_direct_link($relate_to, $relationship_type) {
  //safety, revert to  a generic link
  if (!isset($relationship_type)) {
    return theme('user_relationships_request_relationship_link', $relate_to);
  }
  return l(
    t("Become %name's %rel_name", array('%name' => $relate_to->name, '%rel_name' => ur_tt("user_relationships:rtid:$relationship_type->rtid:name", $relationship_type->name), '%rel_plural_name' => ur_tt("user_relationships:rtid:$relationship_type->rtid:plural_name", $relationship_type->plural_name))),
    "relationship/{$relate_to->uid}/request/{$relationship_type->rtid}",
    array(
      'query' => drupal_get_destination(),
      'html'  => TRUE,
      'attributes' => array('class' => 'user_relationships_popup_link'),
    )
  );
}

/**
 * Remove relationship link
 */
function theme_user_relationships_remove_link($uid, $rid) {
  return l(
    t('Remove'),
    "user/{$uid}/relationships/{$rid}/remove",
    array(
      'query' => drupal_get_destination(),
      'attributes' => array(
        'class' => 'user_relationships_popup_link',
        'title' => t('Remove'),
      ),
    )
  );
}

/**
 * Approve to pending relationship link
 */
function theme_user_relationships_pending_request_approve_link($uid, $rid) {
  return l(
    t('Approve'),
    "user/{$uid}/relationships/requested/{$rid}/approve",
    array(
      'query' => drupal_get_destination(),
      'attributes' => array(
        'class' => 'user_relationships_popup_link',
        'title' => t('Approve'),
      ),
    )
  );
}

/**
 * Disapprove to pending relationship link
 */
function theme_user_relationships_pending_request_disapprove_link($uid, $rid) {
  return l(
    t('Disapprove'),
    "user/{$uid}/relationships/requested/{$rid}/disapprove",
    array(
      'query' => drupal_get_destination(),
      'attributes' => array(
        'class' => 'user_relationships_popup_link',
        'title' => t('Disapprove'),
      ),
    )
  );
}

/**
 * Cancel request link
 */
function theme_user_relationships_pending_request_cancel_link($uid, $rid) {
  return l(
    t('Cancel'),
    "user/{$uid}/relationships/requested/{$rid}/cancel",
    array(
      'query' => drupal_get_destination(),
      'attributes' => array(
        'class' => 'user_relationships_popup_link',
        'title' => t('Cancel'),
      ),
    )
  );
}

/**
 * Link to user's profile
 */
function theme_user_relationships_user_link($uid) {
  return url("user/$uid", array('absolute' => TRUE));
}

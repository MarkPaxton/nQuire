<?php

/**
 * @file
 * Implements Activity Guide workflow.
 */
/* this is  the new non-database workflow rules */


function find_unique_node_type_for_uid_and_inquiry($node_type, $uid, $inquiry_id) {
  $query = "SELECT {node}.`nid` FROM {node} " .
          "LEFT JOIN {pi_activity_node} ON {pi_activity_node}.`nid` = {node}.`nid` " .
          "LEFT JOIN {pi_activity} ON {pi_activity_node}.`activity_id` = {pi_activity}.`nid` " .
          "LEFT JOIN {pi_phase} ON {pi_activity}.`phase_nid` = {pi_phase}.`nid` " .
          "LEFT JOIN {node_access} ON {node_access}.`nid` = {node}.`nid` " .
          "LEFT JOIN {og_uid} ON {node_access}.`gid` = {og_uid}.`nid` " .
          "WHERE {node}.`type` = '%s' " .
          "AND ( {og_uid}.uid = %d OR {node}.uid = %d ) " .
          "AND {pi_phase}.`inquiry_nid` = %d " .
          "ORDER BY {node}.`nid` DESC ";
  $result = db_result(db_query($query, $node_type, $uid, $uid, $inquiry_id));
  return $result;
}


function update_activity_when_activity_node_updated($node, $activity_title) {
  global $user;
  $uid = $user->uid;
  $details = get_inquiry_details_from_current_path();

  $loaded_activity = node_load($details->activity_id);
  $loaded_activity->title = $activity_title;
  node_save($loaded_activity);
}

/**
 *
 * Add an activity node when an activity content item is created
 * @param node $add_activity
 * @param node $content_node
 */
function pi_activity_add_activity_node_for_activity_content_node($add_activity, $content_node) {
  //This is the new activity node to be created
  $new_activity_node = new stdClass();
  //copy over current except for the activity_id

  
  /* FIX (eloy) */
  $new_activity_node->type = 'pi_activity';
  $new_activity_node->activity_phases_stages = $add_activity->activity_phases_stages;
  $new_activity_node->selects = $add_activity->selects;
  $new_activity_node->uid = $content_node->uid;
  
  $new_activity_node->title = $content_node->title;
  $new_activity_node->body = $add_activity->description;
  $new_activity_node->activity_type = $add_activity->activity_type;
  $new_activity_node->parameters = $add_activity->parameters;
  $new_activity_node->script_activity = FALSE;
  $new_activity_node->phase_nid = $add_activity->phase_nid;
  $new_activity_node->weight = $add_activity->weight;
  $new_activity_node->parent_activity = $add_activity->parent_activity;

	  node_save($new_activity_node);
  
  /* FIX (eloy) */
  pi_activity_link_content_with_activity($content_node, $new_activity_node->nid);
}

/**
 * Add an activity node when an activity content item is created
 * @param $node
 * @param $node_function
 * @param $visible_status
 * @param $activity_title
 * @param $activity
 */
function pi_activity_add_new_activity_for_activity_content_node($node, $activity) {
  if (!pi_check_is_node_of_type($activity, 'pi_activity')) {
    $activity = node_load(pi_activity_get_activity_nid_for_content_nid($node->nid));
  }
  if (pi_check_is_node_of_type($activity, 'pi_activity')) {
    pi_activity_add_activity_node_for_activity_content_node($activity, $node);
  } else {
    drupal_set_message(t("Can't identify activity for created node @nid, of type @type", array('@nid' => $node->nid, '@type' => $node->type)), 'error');
  }
}

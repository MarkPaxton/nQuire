<?php

/**
 * @file
 * Implements Activity Guide workflow.
 */
/*this is  the new non-database workflow rules*/


function find_unique_node_type_for_uid_and_inquiry($node_type, $uid, $inquiry_id)
{
  $query = "SELECT {node}.`nid` FROM {node} " .
      "LEFT JOIN {pi_activity_node} ON {pi_activity_node}.`nid` = {node}.`nid` ". 
      "LEFT JOIN {pi_activity} ON {pi_activity_node}.`activity_id` = {pi_activity}.`nid` ". 
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

function find_sort_key_questions_nid_for_uid_and_inquiry($uid, $inquiry_id)
{
  return find_unique_node_type_for_uid_and_inquiry('pi_sort_key_questions', $uid, $inquiry_id);
}

function find_hypothesis_nid_for_uid_and_inquiry($uid, $inquiry_id)
{
  return find_unique_node_type_for_uid_and_inquiry('pi_hypothesis', $uid, $inquiry_id);
}

function update_activity_when_activity_node_updated($node, $activity_title)
{
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
 * @param node $parent_activity
 * @param node $content_node
 */
function pi_activity_add_activity_node_for_activity_content_node($parent_activity, $content_node)
{

  //This is the new activity node to be created
  $new_activity_node = new stdClass();
  //copy over current except for the activity_id


  $new_activity_node->title = $content_node->title;
  $new_activity_node->body = $parent_activity->description;
  $new_activity_node->activity_type = $parent_activity->activity_type;
  $new_activity_node->node_type = $content_node->node_type;
  $new_activity_node->parameters = $parent_activity->parameters;
  $new_activity_node->destination_after = $parent_activity->destination_after;
  $new_activity_node->audience = $parent_activity->audience;
  $new_activity_node->actor = $parent_activity->actor;
  $new_activity_node->weight = $parent_activity->weight;

  node_save($new_activity_node);
}

/**
 *
 * Add an activity node when an activity content item is created
 * @param unknown_type $node
 * @param unknown_type $node_function
 * @param unknown_type $visible_status
 * @param unknown_type $activity_title
 * @param unknown_type $details
 */
function pi_activity_add_new_activity_for_activity_content_node($node, $node_function, $visible_status, $activity_title, $activity)
{
  //pi_debug_message("pi_activity_add_activity_when_activity_content_node_created called for " . $content_node->nid . "(" . $content_node->title . ")");
  //load the defined "activity"
  if(!pi_check_is_node_of_type($activity, 'pi_activity'))
  {
    //pi_debug_message("pi_activity_add_activity_when_activity_content_node_created called with no activity - getting activity from details: " . $details->activity_id);
    $activity = node_load(pi_activity_get_activity_nid_for_content_nid($node->nid));
  }
  if(pi_check_is_node_of_type($activity, 'pi_activity'))
  {
    pi_activity_add_activity_node_for_activity_content_node($activity, $node);
    // Then set the status...
    $new_activity_nid = pi_activity_get_activity_nid_for_content_nid($node->nid);
    if(pi_check_is_node_of_type($activity))
    {
      pi_activity_set_status($nid, $actor);
    }

  }
  else
  {
    drupal_set_message(t("Can't identify activity for created node @nid, of type @type", array('@nid' => $node->nid, '@type'=>$node->type)), 'error');
  }
  /*
   //copy over current except for the activity_id
  $activity_array = new stdClass();
  $activity_array->title = check_plain($activity_title);
  $activity_array->body = $loaded_activity->description;
  $activity_array->activity_type = $loaded_activity->activity_type;
  $activity_array->node_type = $loaded_activity->node_type;
  $activity_array->parameters = $loaded_activity->parameters;
  $activity_array->destination = $loaded_activity->destination;
  $activity_array->audience = $loaded_activity->audience;
  $activity_array->actor = $loaded_activity->actor;
  $activity_array->weight = $loaded_activity->weight;
  */
  /*
   * @todo The following code changes will cause problems with the authoring interface!
  * This part of the code uses the node id as activity id.
  * Authoring interface generates activity id via max(activity_id) + 1. This generated
  * activity_id will sooner or later conflict with a node id !
  *
  * Either change this code back to the original version, or change authoring interface.
  */
  //$activity_array["nid"] = $node->nid;
  //$new_activity_id = $node->nid;

  //create the new activity
  //node_save($activity_array);

  //$new_activity_id = $activity_array->nid;
  /*	$new_activity_id = $node->nid;

  //****new details object
  $new_details = new stdClass;
  $new_details->inquiry_id = $details->inquiry_id;
  $new_details->stage_id = $details->stage_id;
  $new_details->phase_id = $details->phase_id;
  $new_details->activity_id = $new_activity_id;

  //set status for the new activity
  //$activity_status = load_activity_status($details->activity_id, $user->uid);
  $activity_status = pi_activity_load_status($details->activity_id, $user->uid);

  $new_activity_status = new stdClass;
  $new_activity_status->inquiry_id = $details->inquiry_id;
  $new_activity_status->activity_id = $new_activity_id;
  $new_activity_status->uid = $activity_status['uid'];
  $new_activity_status->gid = $activity_status['gid'];
  $new_activity_status->node_function = $activity_status['node_function'];
  $new_activity_status->visible_status = $activity_status['visible_status'];

  pi_activity_set_status($new_activity_status['activity_id'], $new_status['gid'], $new_activity_status['visible_status'], $new_activity_status['node_function']);*/
  /*
   //set actor for the new activity
  $activity_actor = load_activity_actor($details->inquiry_id, $details->activity_id, NULL, NULL);
  $new_activity_actor["inquiry_id"] = $details->inquiry_id;
  $new_activity_actor["activity_id"] = $new_activity_id;

  ///for audience smaller than actor in add activity
  $actor_gid = $activity_actor[0]["gid"];
  $actor_uid = $activity_actor[0]["uid"];
  $actor_group_type = pi_get_type_of_group($actor_gid);
  $users_group = get_group_of_type($uid, $loaded_activity->audience, $details->inquiry_id, $details->stage_id, $details->phase_id, $details->activity_id);
  $audience_gid = $users_group;
  $audience_group_type = $activity->audience;

  if( ($actor_group_type == 'group' && $audience_group_type == 'individual') ||
  ($actor_group_type == 'class' && $audience_group_type != 'class' && $audience_group_type != 'multiclass') )
  {
  $new_activity_actor["uid"] = $uid;
  $new_activity_actor["gid"] = $audience_gid;
  }
  else
  {
  $new_activity_actor["uid"] = $activity_actor[0]["uid"];
  $new_activity_actor["gid"] = $activity_actor[0]["gid"];
  }

  insert_activity_actor($new_activity_actor);
  */
  //set the stage phase activity
  /*	$psp_id = get_psp_id_for_stage_and_phase($details->stage_id, $details->phase_id);

  if(!$psp_id)
  {
  $psp_id = get_psp_id_for_stage_and_phase(NULL, $details->phase_id);
  }

  $query = "SELECT position FROM {pi_stage_phase_activity} WHERE psp_id = %d ORDER BY position DESC";
  $current_pos = db_result(db_query($query, $psp_id));
  $new_pos = $current_pos + 1;

  insert_stage_phase_activity($psp_id, $new_activity_id, $new_pos);
  */
  //set pi activity node
  /*	add_new_pi_activity_node($node, $node_function, $visible_status, $new_details);

  //set og if necessary for new node
  if($loaded_activity->audience != "individual")
  {
  $users_group = get_group_of_type($uid, $loaded_activity->audience, $new_details->inquiry_id, $new_details->stage_id, $new_details->phase_id, $new_details->activity_id);
  $context['groups'] = array($users_group);
  og_add_group_action($node, $context);
  }*/
}



/**
 * Link an activity content node to a given activity, setting the status as given, and also adding
 * view status to wider audience group
 *
 * @param unknown_type $node Content node to link to an activity
 * @param unknown_type $node_function The function (view, add, edit) that the activity will use to link to the content node
 * @param unknown_type $visible_status The UI status to show for the activity linking to the conent node
 * @param unknown_type $details Current details of the inquiry
 */
function pi_activity_link_activity_content($node, $node_function, $visible_status, $activity_nid_to_link)
{
  // First check the linked activity is valid
  $activity = node_load($activity_nid_to_link);
  if(pi_check_is_node_of_type($activity, 'pi_activity'))
  {
    // First of call create the link in pi_activity_node
    $activity_node = array();
    $activity_node['activity_id'] = $activity_nid_to_link;
    $activity_node['nid'] = $node->nid;
    drupal_write_record('pi_activity_node', $activity_node);

    // Then set the actor group status - get and use existing actor status if it exists otherwise
    // create one. Update is done by passing the key column name in drupal write record, or left
    // as an empty array to do an insert
    $actor_activity_status = pi_activity_load_status($activity_nid_to_link, $node->uid);
    $actor_status_key = array();
    if($actor_activity_status['gid']==$activity->actor)
    {
      $activity_status_node = $actor_activity_status;
      $actor_status_key = 'pas_id';
    }
    else
    {
      $activity_status_node = array();
    }

    $activity_status_node['activity_id'] = $activity->nid;
    $activity_status_node['gid'] = $activity->actor;
    $activity_status_node['visible_status'] = $visible_status;
    $activity_status_node['node_function'] = $node_function;
    drupal_write_record('pi_activity_status', $activity_status_node, $actor_status_key);

    // Set group audience to audience of the activity
    $users_group = pi_groups_get_users_audience_groups($node->uid, $activity->audience, $activity->actor);
    $context['groups'] = $users_group;
    //pi_debug_message($context['groups']);
    og_add_group_action($node, $context);

    if(!in_array($activity_status->gid, $users_group))
    {
      $activity_status->gid = $users_group;
      unset($activity_status->pas_id);
    }

    //set the status for the audience to view
    $audience_group_nids = pi_groups_get_activity_audience($actor_gid, $audience_group_type);
    foreach($audience_group_nids as $audience_group_nid)
    {
      pi_activity_set_status($activity_node->nid, $audience_group_nid, 'view', 'view');
    }

    //extend activity actor to size of audience
    // @TODO - change the activity view to get activities with a status set for the user rather than by actor
    if(count($audience_group_nids)>0)
    {
      $activity->actor = $audience_gid[0];
      node_save($activity);
    }
     
    //add extra status for the original actor
    $actor_groups =  pi_groups_get_users_actor_groups($node->uid, $activity->audience, $activity->actor);
    foreach($actor_groups as $group_nid)
    {
      pi_activity_set_status($activity_nid_to_link, $group_nid, $node_function, $visible_status);
    }
  }
  else
  {
    drupal_set_message(t('Trying to link an activity content to a node that does not exist or that is not a pi_activity (nid @nid).', array('@nid'=>$activity_nid_to_link)), 'error');
  }
}

?>
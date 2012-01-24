<?php
/** @file pi_activity_extra.inc
 *  This is an include file that contains all the extra functions for pi_actitivy, that do not
 *  confirm to the drupal function naming convention "modulename"_name() etc
 */


/**
 * @see get_nid_for_inquiry_activity_and_user
 * @param $inquiry_id
 * @param $activity_id
 * @param $uid
 */
function get_nid_for_inquiry_activity_and_user($inquiry_id, $activity_id, $uid) {
  //@todo depricate this function
  return pi_get_activity_content_nid_for_activity_and_user($activity_id, $uid);
}

/**
 * Returns the note type that the activity sorts or null if it doesn't sort
 * another node type
 * @param unknown_type $node_type
 */
function pi_activity_get_sorted_node_type($node_type)
{
  $generated_node_type = null;
  // Then get the type of data that activity content module provides
  $info_function_name = $node_type . '_node_info';
  if(function_exists($info_function_name))
  {
    $node_info = $info_function_name();
    if(isset($node_info[$node_type]['add_node_type']))
    {
      $generated_node_type = $node_info[$node_type]['add_node_type'];
    }
  }
  return $generated_node_type;
}


/**
 * Get the activity content node id for an activity and user
 * @param $activity_id
 * @param $uid
 * @return NULL or uint nid
 */
function pi_get_activity_content_nid_for_activity_and_user($activity_id, $uid) {
  $nid_to_return = null;

  // first of all see if a user has directly created a node
  $query = "SELECT DISTINCT {pi_activity_node}.`nid` FROM {pi_activity_node} " .
    "LEFT JOIN {node} ON {node}.`nid` = {pi_activity_node}.`nid` " .
    "WHERE {pi_activity_node}.`activity_id` = %d AND {node}.`uid` = %d " .
    "ORDER BY {pi_activity_node}.`nid` DESC";

  //    pi_debug_message(sprintf(str_replace(array("{", "}"), "", $query), $activity_id, $uid));
  $result = db_query($query, $activity_id, $uid);
  $nids_returned = array();
  while($nid_result = db_fetch_object($result))
  {
    $nids_returned[] = $nid_result->nid;
  }
  if(count($nids_returned)>0)
  {
    $nid_to_return = $nids_returned[0];
    if(count($nids_returned)>1)
    {
      drupal_set_message(t("Multiple nids returned for content for activity %id and user %user - using highest nid", array("%id" => $activity_id, "%user" => $uid)), 'warning');
    }
  }

  // If there isn't a content nid directly owned by this user, then check the group ownerships
  if($nid_to_return==null)
  {
    $query = "SELECT DISTINCT {pi_activity_node}.`nid` FROM {pi_activity_node} " .
      "LEFT JOIN {pi_activity} ON {pi_activity}.`nid` = {pi_activity_node}.`activity_id` " .
      "LEFT JOIN {og_uid} ON {og_uid}.`nid` = {pi_activity}.`actor` " .
      "LEFT JOIN {node_access} ON {pi_activity_node}.`nid` = {node_access}.`nid` " .
      "WHERE {pi_activity_node}.`activity_id` = %d " .
      "AND ( {node_access}.`gid` = {og_uid}.`nid` AND {og_uid}.`uid` = %d ) " .
	  " ORDER BY {pi_activity}.`weight`";
    //"OR {node_access}.`realm` = 'all')";

    $result = db_query($query, $activity_id, $uid);
    $nids_returned = array();
    while($nid_result = db_fetch_object($result))
    {
      $nids_returned[] = $nid_result->nid;
    }
    if (count($nids_returned)>0)
    {
      $nid_to_return = $nids_returned[0];
      if(count($nids_returned)>1)
      {
        drupal_set_message(t("Multiple nids returned for content for group activity !id and user !user - using highest nid !nid",
        array("!id" => $activity_id, "!user" => $uid, "!nid" => $nid_to_return)), 'warning');
      }
    }
  }
  return $nid_to_return;
}
	
/**
 * Returns the phase nid.
 * @param uint $activity_nid
 * @return uint nid of phase or NULL
 */
function pi_activity_get_phase_nid($activity_nid)
	{
  $return_value = NULL;
  $activity = node_load($activity_nid);
  if(pi_check_is_node_of_type($activity, 'pi_activity'))
  {
      $return_value = $activity->phase_nid;
      //Check this is a valid phase...
      if(!pi_check_node_exists_and_is_of_type($return_value, 'pi_phase'))
      {
      	drupal_set_message(t("Can't get phase for activity !nid as it does not exist, or is not a valid phase.",
      		array('!nid'=>$activity_nid)), 'error');
      }
  }
  else
  {
  	drupal_set_message(t("Can't get phase nid for node !nid, as it is not a valid pi_activity node",
  	array('!nid'=>$activity_nid)), 'error');
  }
  return $return_value;
}

/**
* Returns the inquiry id.
* Assumes 1-1 mapping of activities to phases
* @param uint nid for an activity.
*/
function pi_activity_get_inquiry_nid($activity_id)
{
	$phase_id = pi_activity_get_phase_nid($activity_id);
	$phase = node_load($phase_id);
	$inquiry_id = $phase->inquiry_nid;
	return $inquiry_id;
}




/**
 * Returns an array of activity_ids of the specified activity_type
 * that are visible to that user according to pi_activity_actor.
 * This uses pi_activity_actor and pi_activity tables.
 */
function get_activity_ids_for_activity_type($inquiry_id, $activity_type, $uid) {
  $query = "SELECT {pi_activity}.activity_id FROM {pi_activity} "
  . "LEFT JOIN {pi_activity_actor} ON {pi_activity_actor}.activity_id = {pi_activity}.activity_id "
  . "LEFT JOIN {og_uid} ON {og_uid}.nid = {pi_activity_actor}.gid "
  . "WHERE {pi_activity_actor}.inquiry_id = %d "
  . "AND {pi_activity}.activity_type =  '%s' "
  . "AND ( {pi_activity_actor}.uid = %d OR {og_uid}.uid = %d)";

  $result = db_result(db_query($query, $inquiry_id, $activity_type, $uid, $uid));
  return $result;

}

/**
 * Returns an array of node_ids of the specified activity_type
 * that are visible to that user according to pi_activity_actor.
 * This uses pi_activity_actor and pi_activity tables.
 */
function get_node_ids_for_activity_type ($inquiry_id, $activity_type, $uid) {
  $query = "SELECT {pi_activity_node}.nid FROM {pi_activity_node} "
  . "LEFT JOIN {pi_activity} ON {pi_activity_node}.activity_id = {pi_activity}.activity_id "
  . "LEFT JOIN {pi_activity_actor} ON (( {pi_activity_node}.inquiry_id = {pi_activity_actor}.inquiry_id ) AND ( {pi_activity_actor}.activity_id = {pi_activity}.activity_id )) "
  . "LEFT JOIN {og_uid} ON {og_uid}.nid = {pi_activity_actor}.gid "
  . "WHERE {pi_activity_node}.inquiry_id = %d "
  . "AND {pi_activity}.activity_type =  '%s' "
  . "AND ({pi_activity_actor}.uid = %d OR {og_uid}.uid = %d)";

  // print $query;
  $result = db_query($query, $inquiry_id, $activity_type, $uid, $uid);
  return $result;

}

/**
 * Returns the activity nid for a content item, if it has one.
 * Assumes only one activivity nid per item
 * @param uint $content_nid
 * @returns uint of an node id (unverified if it exits though) or NULL if no activity node found
 */
function pi_activity_get_activity_nid_for_content_nid($content_nid)
{
  $activity_nid = null;
  $query = "SELECT `activity_id` FROM {pi_activity_node} WHERE `nid` = %d";
  $result = db_fetch_object(db_query($query, $content_nid));
  if($result != false)
  {
  	if(isset($result->activity_id))
  	{
    	$activity_nid = $result->activity_id;
  	}
  }
  return $activity_nid;
}


/**
 * Get the activity ids for all child activities for an activity
 * @param unknown_type $activity_nid
 */
function pi_activity_get_child_activities_for_activity($activity_nid)
{
  $children = array();
  $query = "SELECT `nid` FROM {pi_activity} WHERE `parent_activity` = %d";
  while($result = db_fetch_object(db_query($query, $activity_nid)))
  {
    $children[] = $result->nid;
  }
  return $children;
}


/**
 * Uses arguments from the path and the pi_activity_table to return the activity type.
 */
function get_activity_type_of_node_path($path) {
  $inquiry_details = get_inquiry_details_of_node_path ($path);

  $query = "SELECT activity_type FROM {pi_activity} WHERE activity_id = %d";
  $result = db_result(db_query($query, $inquiry_details->inquiry_id));

  return $result ;
}

/**
 * Returns FALSE if this is not the activity_type for the activity type
 * associated with the node path, otherwise returns TRUE.
 */
function check_activity_type_of_node_path ($path, $activity_type) {
  $temp = get_activity_type_of_node_path ($path);

  if ($temp == NULL) {
    $result = NULL;
  }
  elseif ($temp == $activity_type) {
    $result = TRUE;
  }
  else {
    $result = FALSE;
  };

  return $result;

}

/**
 * Adds the specified data to the pi_activity_node table.
 * This tracks that a node has been created for that particular activity.
 */
function add_pi_activity_node($inquiry_id, $activity_id, $nid) {
  $query = "INSERT INTO {pi_activity_node} SET inquiry_id = %d, activity_id = %d, nid_id = %d";

  db_query($query, $inquiry_id, $activity_id, $nid_id);
}

// ------------------------------------------------------------------------------------

/**
 * Functions in this block are for visible_status and node_function.
 * Primarily they refer to values of pi_activity_status.
 */

/**
 * Returns visible_status from pi_activity_status (can be start, in_progress, view or unavailable).
 */
function pi_activity_get_visible_status_of_activity($activity_id, $uid) {
  $activity_status = pi_activity_load_status($activity_id, $uid);
  $result = $activity_status['visible_status'];
  return $result ;
}

/**
 * $status can be either start, in_progress, view or unavailable.
 * Returns FALSE if that user does not have that permission for the
 * specified activity, otherwise it returns TRUE.
 */
function pi_activity_check_visible_status_of_activity ($activity_id, $uid, $status) {
  $visible_status = pi_activity_get_visible_status_of_activity($activity_id, $uid);
  if ($visible_status == $status)
  {
    $result = TRUE;
  }
  else
  {
    $result = FALSE;
  }
  return $result;
}

/**
 * $status can be either start, in_progress, view or unavailable.
 * Updates the existing visible_status to value $status.
 * This will set dependent on the audience in pi_activity.
 */
function set_visible_status_of_activity($inquiry_id, $activity_id, $uid, $status)
{
  $activity_status = pi_activity_load_status($activity_id, $uid);
  $activity_status['visible_status'] = $status;
  pi_activity_set_status($activity_id, $activity_status->gid, $status, $activity_status['node_function']);
}

/**
 * Updates the visible status for all of the activities visible to that user of the specified type.
 * This will be set dependent on the audience in pi_activity.
 */
function set_visible_status_of_activity_type($inquiry_id, $activity_type, $uid, $status) {
  $query = "SELECT {pi_activity_status}.pas_id FROM {pi_activity_status} "
  . "LEFT JOIN {pi_activity} ON {pi_activity_status}.activity_id = {pi_activity}.activity_id "
  . "LEFT JOIN {pi_activity_actor} ON {pi_activity_actor}.inquiry_id = {pi_activity_status}.inquiry_id "
  . "AND {pi_activity_actor}.activity_id = {pi_activity_status}.activity_id "
  . "LEFT JOIN {og_uid} ON {og_uid}.nid = {pi_activity_actor}.gid "
  . "WHERE {pi_activity_status}.inquiry_id = %d "
  . "AND {pi_activity_status}.activity_id = %d "
  . "AND {pi_activity}.activity_type = '%s' "
  . "AND ( {pi_activity_actor}.uid = %d OR {og_uid}.uid =  %d )";

  $items = db_query($query, $inquiry_id, $activity_id, $activity_type, $uid, $uid);

  while ($item = db_fetch_object($items))
  {
    $query = "UPDATE {pi_activity_status} SET visible_status = '%s' WHERE pas_id = %d";
    db_query($query, $status, $item->pas_id);
  }
}

/**
 * Returns node_function from pi_activity_status (can be add, edit or view).
 * @param $inquiry_id
 * @param $activity_id
 * @param $uid
 * @return unknown_type
 */
function pi_activity_get_node_function($activity_id, $uid)
{
  $activity_status = pi_activity_load_status($activity_id, $uid);
  $result = $activity_status['node_function'];
  return $result ;
  /*
   $query = "SELECT pi_activity_status.node_function ";
  $query .= "FROM pi_activity_status ";
  $query .= "LEFT JOIN pi_activity_actor ";
  $query .= "ON pi_activity_actor.inquiry_id = pi_activity_status.inquiry_id ";
  $query .= "AND pi_activity_actor.activity_id = pi_activity_status.activity_id ";
  $query .= "LEFT JOIN og_uid ON og_uid.nid = pi_activity_actor.gid ";
  $query .= "WHERE pi_activity_status.inquiry_id = '" . $inquiry_id . "' ";
  $query .= "AND pi_activity_status.activity_id = '" . $activity_id . "' ";
  $query .= "AND (pi_activity_actor.uid =  '" . $uid. "' OR og_uid.uid =  '" . $uid . "')";

  $result = db_fetch_object(db_query($query));

  return $result->node_function;*/

}

/**
 * Returns FALSE if that node function for that activity and user is not the specified
 * function, otherwise it returns TRUE.
 * @param uint $activity_id ID of pi_acitvity to check node function with
 * @param uint $uid ID of user to check node function with
 * @param string $function can Either 'add', 'edit' or 'view'.
 * @return bool allowed or not
 */
function pi_activity_check_node_function($activity_id, $uid, $function) {
  $node_function = pi_activity_get_node_function($activity_id, $uid);
  pi_debug_message("pi_activity_check_node_function: function=" . $function . " node function=" . $node_function);
  if ($node_function == $function)
  {
    $result = TRUE;
  }
  else
  {
    $result = FALSE;
    //if($activity_id==78)
    //  $result=TRUE;
  }
  return $result;
}

/**
 * $function can be either add, edit or view.
 * Updates the exisitng node_function to the value of $function.
 * This will be set dependent on the audience in pi_activity.
 * @param $inquiry_id
 * @param $activity_id
 * @param $uid
 * @param $function
 * @return unknown_type
 */
function set_node_function_of_activity($inquiry_id, $activity_id, $uid, $function) {
  $activity_status = pi_activity_load_status($activity_id, $uid);
  $activity_status['node_function'] = $function;
  pi_activity_set_status($activity_id, $activity_status->gid, $activity_status['visible_status'], $function);
}

/**
 * Updates the node function for all of the activities visible to that user of the specified type.
 * This will be set dependent on the audience in pi_activity.
 * @param $inquiry_id
 * @param $activity_type
 * @param $uid
 * @param $function
 * @return unknown_type
 */
function set_node_function_of_activity_type($inquiry_id, $activity_type, $uid, $function) {

  $query = "SELECT {pi_activity_status}.pas_id FROM {pi_activity_status} "
  . "LEFT JOIN {pi_activity} ON {pi_activity_status}.activity_id = {pi_activity}.activity_id "
  . "LEFT JOIN {pi_activity_actor} ON {pi_activity_actor}.inquiry_id = {pi_activity_status}.inquiry_id "
  . "AND {pi_activity_actor}.activity_id = {pi_activity_status}.activity_id "
  . "LEFT JOIN {og_uid} ON {og_uid}.nid = {pi_activity_actor}.gid "
  . "WHERE {pi_activity_status}.inquiry_id = %d "
  . "AND {pi_activity_status}.activity_id = %d "
  . "AND {pi_activity}.activity_type = '%s' "
  . "AND ( {pi_activity_actor}.uid = %d OR {og_uid}.uid =  %d )";

  $items = db_query($query, $inquiry_id, $activity_id, $activity_type, $uid, $uid);

  while ($item = db_fetch_object($items))
  {
    $query = "UPDATE {pi_activity_status} SET node_function = '%s' WHERE pas_id = %d";
    db_query($query, $function, $item->pas_id);
  }
}

/**
 * Returns array from pi_activity_actor table.
 * @param uint $inquiry_id
 * @param uint $activity_id
 * @param uint $uid
 * @param uint $gid
 * @return array of activity_actor table with keys: "paa_id", "inquiry_id", "activity_id", "uid", "gid"
 */
function load_activity_actor($inquiry_id, $activity_id = NULL, $uid = NULL, $gid = NULL) {
  $query = "SELECT actor FROM {pi_activity} WHERE nid=%d";

  $items = db_query($query, $activity_id);

  $activities = array();

  while ($item = db_fetch_object($items))
  {
    $activity = array();
    $activity["paa_id"] = $item->paa_id;
    $activity["inquiry_id"] = $item->inquiry_id;
    $activity["activity_id"] = $item->activity_id;
    $activity["uid"] = $item->uid;
    $activity["gid"] = $item->gid;
    $activities[] = $activity;
  };
  return $activities;
}

/**
 * This will insert.
 * $activity_actor_array is a keyed array comprising:
 * $activity_actor_array ->inquiry_id
 * $activity_actor_array ->activity_id
 * $activity_actor_array ->uid
 * $activity_actor_array ->gid
 */
function insert_activity_actor($activity_actor_array)
{


  $query = "INSERT INTO {pi_activity_actor} SET inquiry_id = %d, activity_id = %d ";

  $parameters[] = $activity_actor_array["inquiry_id"];
  $parameters[] = $activity_actor_array["activity_id"] ;

  if (isset($activity_actor_array["uid"]))
  {
    $query .= ", uid = %d ";
    $parameters[] = $activity_actor_array["uid"];
  }
  if (isset($activity_actor_array["gid"]))
  {
    $query .= ", gid = %d ";
    $parameters[] = $activity_actor_array["gid"];
  }

  db_query($query, $parameters);
}

/**
 * This will update.
 * $activity_actor_array is a keyed array comprising:
 * $activity_actor_array ->inquiry_id
 * $activity_actor_array ->activity_id
 * $activity_actor_array ->uid
 * $activity_actor_array ->gid
 */
function update_activity_actor($actor) {

  $query = "UPDATE {pi_activity_actor} SET inquiry_id = %d , activity_id = %d , uid = %d , gid = %d WHERE paa_id = %d";

  db_query($query, $actor["inquiry_id"], $actor["activity_id"], $actor["uid"], $actor["gid"], $actor["paa_id"]);
}

/**
 * Deletes all activities of that type that are visible to the specified user.
 * Will delete from pi_activity, pi_activity_actor, pi_activity_status,
 * pi_phase_activity, pi_activity_node where activity_id field = $activity_id.
 * This will be dependent on the audience in pi_activity.
 * @param $activity_type
 * @param $user
 * @return unknown_type
 */
function delete_activity_type_for_user($activity_type, $user) {

  //get all activity id's for $activity_type
  $query = "SELECT * FROM {pi_activity} WHERE activity_type = '%s'";
  $results = db_result(db_query($query, $activity_type));

  foreach($results as $r)
  {
    $query = "DELETE FROM {pi_activity} WHERE activity_id = %d";
    db_query($query, $r->activity_id );

    //delete from pi_activity_actor
    $query = "DELETE FROM {pi_activity_actor} WHERE activity_id = %d";
    db_query($query,$r->activity_id );

    //delete from pi_activity_status
    $query3 = "DELETE FROM {pi_activity_status} WHERE activity_id = %d";
    db_query($query,$r->activity_id );

    //delete from pi_phase_activity
    $query4 = "DELETE FROM {pi_phase_activity} WHERE activity_id = %d'";
    db_query($query,$r->activity_id );

    //delete from pi_activity_node
    $query5 = "DELETE FROM {pi_activity_node} WHERE activity_id = %d";
    db_query($query,$r->activity_id );
  }
}


/**
 * Create a link to a particular activity with associated icon
 * @param uint $inquiry_id
 * @param uint $stage_id
 * @param uint $phase_id
 * @param uint $activity_id
 * @param uint $uid
 */
function pi_activity_build_link($activity_id, $uid, $class = 'menu', $title = NULL)
{
  $link = pi_activity_get_link($activity_id, $uid);

  if( $title )
  {
    $link['title'] = $title;
  }

  $options['html'] = TRUE;
  $options['query'] = $link['destination'];
  $options['attributes'] = array( 'class' => $class);

  $visible_status = pi_activity_get_visible_status_of_activity($activity_id, $uid);
  if($visible_status == 'unavailable')
  {
    $output = t("@link<br/>", array('@link' => $link['title']));
  }
  else
  {
    $output = l($link['title'], $link['url'], $options );
  }
  return $output;
}


/**
 * Generates add activity link
 */
function make_add_activity_link($inquiry_id, $stage_id, $phase_id, $activity_id, $uid)
{
  $add_activity_link = '';

  $add_activity_id = pi_activity_get_add_activity_for_sort_activity($inquiry_id, $activity_id, $uid);

  if ($add_activity_id)
  {
    $activity_link = pi_activity_build_link($add_activity_id, $uid);
    $add_activity_link = '<div class="add_activity_link">' . $activity_link . '</div>';
  }

  return $add_activity_link;
}


?>
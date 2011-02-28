<?php 
/** @file pi_activity_extra.inc
 *  This is an include file that contains all the extra functions for pi_actitivy, that do not 
 *  confirm to the drupal function naming convention "modulename"_name() etc
 */
 
 /**
 * Return and object with a nid property which is the phase activity resides is in in a
 * particular inquiry for a given user.
 * @param uint $inquiry_id The inquiry id of the activity
 * @param uint $activity_id The activity id
 * @param uint $uid The uid of the user that activity belongs to
 */
function get_phase_of_inquiry_and_activity($inquiry_id, $activity_id, $uid) {
  $query = "SELECT {pi_phase}.nid as nid FROM {pi_phase} "
  . "LEFT JOIN {pi_phase_actor} ON {pi_phase_actor}.phase_id = {pi_phase}.nid "
  . "LEFT JOIN {og_uid} ON {og_uid}.nid = {pi_phase_actor}.gid "
  . "LEFT JOIN {pi_stage_phase} ON {pi_stage_phase}.phase_id = {pi_phase}.nid "
  . "LEFT JOIN {pi_stage_phase_activity} ON {pi_stage_phase_activity}.psp_id = {pi_stage_phase}.psp_id "
  . "WHERE {pi_phase}.inquiry_nid = %d "
  . "AND ( {pi_phase_actor}.uid = %d OR {og_uid}.uid = %d ) "
  . "AND {pi_stage_phase_activity}.activity_id = %d";
  $result = db_fetch_object(db_query($query, $inquiry_id, $uid, $uid, $activity_id));
  return $result;
}


/**
 * get_nid_for_inquiry_activity_and_user
 **/
function get_nid_for_inquiry_activity_and_user($inquiry_id, $activity_id, $uid) {
  //find if uid is author of the node
  $query1 = "SELECT {pi_activity_node}.nid FROM {pi_activity_node} "
  . "LEFT JOIN {node} ON {node}.nid = {pi_activity_node}.nid "
  . "WHERE {pi_activity_node}.inquiry_id = %d "
  . "AND {pi_activity_node}.activity_id = %d AND {node}.uid = %d "
  . "ORDER BY {pi_activity_node}.pan_id DESC";

  /*	$query1 = "SELECT pi_activity_node.nid FROM pi_activity_node, node";
   $query1 .= " WHERE  pi_activity_node.inquiry_id = " . $inquiry_id;
   $query1 .= " AND pi_activity_node.activity_id = " . $activity_id;
   $query1 .= " AND node.nid = pi_activity_node.nid";
   $query1 .= " AND node.uid = '" . $uid . "' ";
   $query1 .= " ORDER BY pan_id DESC";*/


  $result = db_fetch_object(db_query($query1, $inquiry_id, $activity_id, $uid));
  if (isset($result))
  {
    if(isset($result->nid))
    {
      return $result->nid;
    }
  }

  //find if uid is member of a group for the node
  $query2 = "SELECT {pi_activity_node}.nid FROM {pi_activity_node}, {node_access}, {og_uid} "
  . "WHERE  {pi_activity_node}.inquiry_id = %d "
  . "AND {pi_activity_node}.activity_id = %d "
  . "AND {node_access}.nid = {pi_activity_node}.nid "
  . "AND ( ( {node_access}.gid = gid AND {node_access}.gid = og_uid.nid AND {og_uid}.uid = %d ) "
  . "OR {node_access}.realm = 'all')";

  $result = db_fetch_object(db_query($query2, $inquiry_id, $activity_id, $uid));
  return $result->nid;
}


/**
 * Returns the phase id.
 * Assumes 1-1 mapping of phases to activities
 * If more than one returns the one with the highest unique key in pi_stage_phase
 */
function get_phase_id_for_activity_id($activity_nid) {
  $activity = node_load($activity_nid);
  $return_value = NULL;
  if($activity == false)
  {
    if($activity->type = 'pi_activity')
    {
      $return_value = $activity->phase_nid;
    }
    else
    {
      drupal_set_message(t("Can't get phase nid for node @nid, as it is not a pi_activity node", 
        array('nid'=>$activity_nid)), 'error');
    }
  }
  else
  {
          drupal_set_message(t("Can't get phase nid for node @nid, as it does not exist", 
        array('nid'=>$activity_nid)), 'error');    
  }
  return $return_value;
}

/**
 * Returns the inquiry id.
 * Assumes 1-1 mapping of activities to phases
 */
function get_inquiry_id_for_activity_id($activity_id) {
  $phase_id = get_phase_id_for_activity_id($activity_id);
  $inquiry_id = get_inquiry_id_for_phase_id ($phase_id);
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

function get_activity_id_for_nid($nid) {
  $query = "SELECT activity_id FROM {pi_activity_node} WHERE nid = %d";
  $result = db_fetch_object(db_query($query, $nid));
  return $result->activity_id;
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
function get_visible_status_of_activity($inquiry_id, $activity_id, $uid) {
  $activity_status = load_activity_status($activity_id, $uid);
  $result = $activity_status->visible_status;
  return $result ;
}

/**
 * $status can be either start, in_progress, view or unavailable.
 * Returns FALSE if that user does not have that permission for the
 * specified activity, otherwise it returns TRUE.
 */
function check_visible_status_of_activity ($inquiry_id, $activity_id, $uid, $status) {


  $visible_status = get_visible_status_of_activity($inquiry_id, $activity_id, $uid);

  if ($visible_status == $status) {
    $result = TRUE;
  }
  else {
    $result = FALSE;
  };

  return $result;

}

/**
 * $status can be either start, in_progress, view or unavailable.
 * Updates the existing visible_status to value $status.
 * This will set dependent on the audience in pi_activity.
 */
function set_visible_status_of_activity($inquiry_id, $activity_id, $uid, $status) {
  $activity_status = load_activity_status($activity_id, $uid);

  $activity_status->visible_status = $status;

  save_activity_status($activity_status);
  /*
   $query = "SELECT pi_activity_status.pas_id ";
   $query .= "FROM pi_activity_status ";
   $query .= "LEFT JOIN pi_activity_actor ";
   $query .= "ON pi_activity_actor.inquiry_id = pi_activity_status.inquiry_id ";
   $query .= "AND pi_activity_actor.activity_id = pi_activity_status.activity_id ";
   $query .= "LEFT JOIN og_uid ON og_uid.nid = pi_activity_actor.gid ";
   $query .= "WHERE pi_activity_status.inquiry_id = '" . $inquiry_id . "' ";
   $query .= "AND pi_activity_status.activity_id = '" . $activity_id . "' ";
   $query .= "AND (pi_activity_actor.uid = '" . $uid. "' OR og_uid.uid =  '" . $uid . "')";

   $pas_id = db_result(db_query($query));

   $query = "UPDATE pi_activity_status ";
   $query .= "SET visible_status = '" . $status . "' ";
   $query .= "WHERE pas_id = '" . $pas_id . "'";

   db_query($query);*/

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
function get_node_function_of_activity($inquiry_id, $activity_id, $uid) {
  $activity_status = load_activity_status($activity_id, $uid);
  $result = $activity_status->node_function;
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
 * @param $inquiry_id
 * @param uint $activity_id ID of pi_acitvity to check node function with
 * @param uint $uid ID of user to check node function with
 * @param string $function can Either 'add', 'edit' or 'view'.
 * @return unknown_type
 */
function check_node_function_of_activity($inquiry_id, $activity_id, $uid, $function) {
  $node_function = get_node_function_of_activity($inquiry_id, $activity_id, $uid);
  if ($node_function == $function)
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
  $activity_status = load_activity_status($activity_id, $uid);
  $activity_status->node_function = $function;
  save_activity_status($activity_status);
  /*
   $query = "SELECT pi_activity_status.pas_id ";
   $query .= "FROM pi_activity_status ";
   $query .= "LEFT JOIN pi_activity_actor ";
   $query .= "ON pi_activity_actor.inquiry_id = pi_activity_status.inquiry_id ";
   $query .= "AND pi_activity_actor.activity_id = pi_activity_status.activity_id ";
   $query .= "LEFT JOIN og_uid ON og_uid.nid = pi_activity_actor.gid ";
   $query .= "WHERE pi_activity_status.inquiry_id = '" . $inquiry_id . "' ";
   $query .= "AND pi_activity_status.activity_id = '" . $activity_id . "' ";
   $query .= "AND (pi_activity_actor.uid = '" . $uid. "' OR og_uid.uid =  '" . $uid . "')";

   $pas_id = db_result(db_query($query));

   $query = "UPDATE pi_activity_status ";
   $query .= "SET node_function = '" . $function . "' ";
   $query .= "WHERE pas_id = '" . $pas_id . "'";

   db_query($query);*/

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
 * This will insert or update depending on whether the activity_id is set in the array and
 * exists in the pi_activity table. $activity_array is a keyed array comprising:
 * $activity_array->activity_id
 * $activity_array->name
 * $activity_array->description
 * $activity_array->activity_type
 * $activity_array->node_type
 * $activity_array->parameters
 * $activity_array->destination
 * $activity_array->audience
 */
function save_activity($activity_array)
{
  if ($activity_array["activity_id"] != NULL)
  {
    //update
    $query = "UPDATE {pi_activity} SET name = '%s', description = '%s', activity_type = '%s',
						node_type = '%s', parameters = '%s', destination = '%s', 
						audience = '%s' WHERE activity_id = %d";
    db_query($query, $activity_array["name"], $activity_array["description"],
    $activity_array["activity_type"], $activity_array["node_type"],
    $activity_array["parameters"], $activity_array["destination"],
    $activity_array["audience"], $activity_array["activity_id"]);
  }
  else
  {
    //insert
    $query = "INSERT INTO {pi_activity} SET activity_id = %d, name = '%s', description = '%s',
							activity_type = '%s', node_type = '%s', parameters = '%s', 
							destination = '%s', audience = '%s' ";

    db_query($query, $activity_array["nid"], $activity_array["name"], $activity_array["description"],
    $activity_array["activity_type"], $activity_array["node_type"],
    $activity_array["parameters"], $activity_array["destination"],
    $activity_array["audience"]);
  }
}

/**
 * Deletes an activity. Will delete from pi_activity, pi_activity_actor,
 * pi_activity_status, pi_phase_activity, pi_activity_node
 * where activity_id field = $activity_id.
 * @param unknown_type $activity_id
 * @return unknown_type
 */
function delete_activity($activity_id)
{
  //delete from pi_activity
  $query = "DELETE FROM {pi_activity} WHERE activity_id = %d";
  db_query($query, $activity_id );

  //delete from pi_activity_actor
  $query = "DELETE FROM {pi_activity_actor} WHERE activity_id = %d";
  db_query($query, $activity_id );

  //delete from pi_activity_status
  $query = "DELETE FROM {pi_activity_status} WHERE activity_id = %d";
  db_query($query, $activity_id );

  //delete from pi_phase_activity
  $query = "DELETE FROM {pi_stage_phase_activity} WHERE activity_id = %d";
  db_query($query, $activity_id );

  //delete from pi_activity_node
  $query = "DELETE FROM {pi_activity_node} WHERE activity_id = %d";
  db_query($query, $activity_id );

  //$node = node_load( $activity_id );
  //echo "Activity ID: " . $activity_id;
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
  $query = "SELECT * FROM {pi_activity_actor} WHERE inquiry_id = %d ";
  $parameters[] = $inquiry_id;

  if ($activity_id)
  {
    $query .= " AND activity_id = %d";
    $parameters[] = $activity_id;
  }
  if ($uid)
  {
    $query .= " AND uid = %d";
    $parameters[] = $uid;
  }
  if ($gid)
  {
    $query .= " AND gid = %d";
    $parameters[] = $gid;
  }

  $items = db_query($query, $parameters);

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
function insert_activity_actor($activity_actor_array) {

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
 * Returns array from pi_activity_status table objects.
 * @param uint $activity_id
 * @param uint $uid
 * @return array(activity_id, uid, status)
 */
function load_activity_status($activity_id, $uid = NULL)
{
  $return_data = array();
  if($activity_id==NULL)
  {
    drupal_set_message('load_activity_status: null value for activity_id passed', 'warning');
    $query = 'SELECT * FROM {pi_activity_status}';
  }  
  else
  {
    $activity = node_load($activity_id);
    if ($activity!=NULL)
    {
      if($activity->type == 'pi_activity')
      {
        $activity_node_found=FALSE;
      }
    }        
  }
  
  $newitems = 0;

  $query = "SELECT {pi_activity_status}.* FROM {pi_activity_status} "
  . "LEFT JOIN {og_uid} ON {og_uid}.nid = {pi_activity_status}.gid ";

  $parameters[] = $inquiry_id;

  if ($activity_id)
  {
    $query .= "AND activity_id = %d ";
    $parameters[] = $activity_id;
  }
  if ($uid)
  {
    $query .= "AND ( {pi_activity_status}.uid = %d OR {og_uid}.uid = %d ) ";
    $parameters[] = $uid;
    $parameters[] = $uid;
  }

  $query .= "ORDER BY pi_activity_status.pas_id DESC";

  $newitems = db_fetch_object(db_query($query, $parameters));

  return $newitems;
}

/**
 * $activity_status_array is a keyed array comprising:
 * $activity_status_array ->inquiry_id
 * $activity_status_array ->activity_id
 * $activity_status_array ->uid
 * $activity_status_array ->gid
 * $activity_status_array ->node_function
 * $activity_status_array ->visible_status
 */
function save_activity_status($activity_status) {

  $set = "inquiry_id = %d , activity_id = %d ";
  $parameters[] = $activity_status->inquiry_id;
  $parameters[] = $activity_status->activity_id;

  if ($activity_status->uid)
  {
    $set .= ", uid = %d ";
    $parameters[] = $activity_status->uid;
  }
  if ($activity_status->gid)
  {
    $set .= ", gid = %d ";
    $parameters[] = $activity_status->gid;
  }

  $set .= ", node_function = '%s' , visible_status = '%s' ";
  $parameters[] = $activity_status->node_function;
  $parameters[] = $activity_status->visible_status;

  $query = "";
  if ($activity_status->pas_id != NULL)
  {
    //update
    $query = "UPDATE {pi_activity_status} SET ";
    $query .= $set;
    $query .= "WHERE {pi_activity_status}.pas_id = %d";
    $parameters[] = $activity_status->pas_id;
  }
  else
  {
    //insert
    $query .= "INSERT INTO {pi_activity_status} SET ";
    $query .= $set;
  }
  db_query($query, $parameters);
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
function build_activity_link($inquiry_id, $stage_id, $phase_id, $activity_id, $uid, $class = 'menu')
{
  $link = pi_activity_get_link($inquiry_id, $stage_id, $phase_id, $activity_id, $uid);

  $options['html'] = TRUE;
  $options['query'] = $link['destination'];
  $options['attributes'] = array( 'class' => $class);

  $visible_status = get_visible_status_of_activity($inquiry_id, $activity_id, $uid);
  if($visible_status == 'unavailable') {
    $output = t($link['title'].'<br/>');
  }
  else {
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
    $activity_link = build_activity_link($inquiry_id, $stage_id, $phase_id, $add_activity_id, $uid);
    $add_activity_link = '<div class="add_activity_link">' . $activity_link . '</div>';
  }

  return $add_activity_link;
}
 
 
 ?>
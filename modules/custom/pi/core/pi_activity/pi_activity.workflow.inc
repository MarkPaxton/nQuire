<?php

/**
 * @file
 * Implements Activity Guide workflow.
 */
/*this is  the new non-database workflow rules*/


function find_unique_node_type_for_uid_and_inquiry($node_type, $uid, $inquiry_id)
{
	$query = "SELECT {%s}.nid FROM {%s} LEFT JOIN {node_access} ON {node_access}.nid = {%s}.nid "
	. "LEFT JOIN {node} ON {node}.nid = {%s}.nid "
	. "LEFT JOIN {pi_activity_node} ON {pi_activity_node}.nid = {%s}.nid "
	. "LEFT JOIN {og_uid} ON {node_access}.gid = {og_uid}.nid "
	. "WHERE ( {og_uid}.uid = %d OR {node}.uid = %d ) AND {pi_activity_node}.inquiry_id = %d ORDER BY nid DESC";
	$result = db_result(db_query($query, $node_type, $node_type, $node_type, $node_type, $node_type, $uid, $uid, $inquiry_id));

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

function find_default_measure_filter_nid()
{
	$result = db_result(db_query('SELECT {pi_default_measure_filter}.nid FROM {pi_default_measure_filter} ORDER BY nid DESC LIMIT 1;'));
	return $result;
}

/*
 This needs to be done differently for measure filter
 Measure filter is no longer an activity but inquiry id is stored with the node
 Therefore use inquiry id in the node for selecting rather than via pi_activity_node
 */
function find_measure_filter_nid_for_uid_and_inquiry($inquiry_id)
{
	$result = db_result(db_query('SELECT {pi_measure_filter}.nid FROM {pi_measure_filter} WHERE {pi_measure_filter}.inquiry_id = %d ORDER BY nid DESC;', $inquiry_id));
	return $result;
}

function find_selected_measures_nid_for_uid_and_inquiry($uid, $inquiry_id)
{
	return find_unique_node_type_for_uid_and_inquiry('pi_sort_measures', $uid, $inquiry_id);
}

function find_selected_measures_nid_for_uid ($uid)
{
	$query = "SELECT {pi_sort_measures}.nid FROM {pi_sort_measures}, {node_access}, {og_uid} "
	. "WHERE {pi_sort_measures}.nid = {node_access}.nid "
	. "AND {node_access}.gid = {og_uid}.nid AND {og_uid}.uid = %d ORDER BY nid DESC";
	$result = db_result(db_query($query, $uid));

	return $result;
}

/**
 * This function updates the title of the node in the node and node_revision table
 * @param $node
 */
function update_node_title($node, $text=NULL)
{
  if ($text === NULL) {
    $text = $node->body;
  }
//get activity title from node body
$length = 250;
if(strlen($text) > $length)
{
$short_body = substr(check_plain($text), 0, $length);
$last_space = strrpos($short_body, " ");
$short_body = substr($short_body, 0, $last_space);
$text = $short_body."...";
}

db_query("UPDATE node SET title='%s' WHERE vid=%d ", $text, $node->vid);
db_query("UPDATE node_revisions SET title='%s' WHERE vid= %d", $text, $node->vid);
}

function update_activity_when_activity_node_updated($node, $activity_title)
{
	global $user;

	$uid = $user->uid;
	$details = get_inquiry_details_from_current_path();
	$loaded_activity = load_activity($details->activity_id);

	$activity_array = array();
	$activity_array["name"] = check_plain($activity_title);
	$activity_array["activity_id"] = $loaded_activity->activity_id;
	$activity_array["description"] = $loaded_activity->description;
	$activity_array["activity_type"] = $loaded_activity->activity_type;
	$activity_array["node_type"] = $loaded_activity->node_type;
	$activity_array["parameters"] = $loaded_activity->parameters;
	$activity_array["destination"] = $loaded_activity->destination;
	$activity_array["audience"] = $loaded_activity->audience;

	save_activity($activity_array);
}

/**
 *
 * Enter description here ...
 * @param unknown_type $node
 * @param unknown_type $node_function
 * @param unknown_type $visible_status
 * @param unknown_type $activity_title
 * @param unknown_type $details
 */
function add_activity_when_activity_node_created($node, $node_function, $visible_status, $activity_title, $details, $activity_node = NULL)
{
	global $user;
	$uid = $user->uid;

	//load the defined "activity"
	if( $activity_node !== NULL && is_object($activity_node) )
	{
		//$loaded_activity = $activity_node;
		$loaded_activity = load_activity($details->activity_id);
		$loaded_activity->activity_type = $activity_node->activity_type;
		$loaded_activity->node_type = $activity_node->node_type;
	}
	else
	{
		$loaded_activity = load_activity($details->activity_id);
	}

	
	//copy over current except for the activity_id
	$activity_array = array();
	$activity_array["name"] = check_plain($activity_title);
	$activity_array["description"] = $loaded_activity->description;
	$activity_array["activity_type"] = $loaded_activity->activity_type;
	$activity_array["node_type"] = $loaded_activity->node_type;
	$activity_array["parameters"] = $loaded_activity->parameters;
	$activity_array["destination"] = $loaded_activity->destination;
	$activity_array["audience"] = $loaded_activity->audience;
	
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
	save_activity($activity_array);
	
	$query2 = "SELECT MAX(activity_id) FROM {pi_activity}";
	$new_activity_id = db_result(db_query($query2));

	//****new details object
	$new_details = new stdClass;
	$new_details->inquiry_id = $details->inquiry_id;
	$new_details->stage_id = $details->stage_id;
	$new_details->phase_id = $details->phase_id;
	$new_details->activity_id = $new_activity_id;

	//set status for the new activity
	$activity_status = load_activity_status($details->inquiry_id, $details->activity_id, $user->uid);

	$new_activity_status = new stdClass;
	$new_activity_status->inquiry_id = $activity_status->inquiry_id;
	$new_activity_status->activity_id = $new_activity_id;
	$new_activity_status->uid = $activity_status->uid;
	$new_activity_status->gid = $activity_status->gid;
	$new_activity_status->node_function = $activity_status->node_function;
	$new_activity_status->visible_status = $activity_status->visible_status;

	save_activity_status($new_activity_status);

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

	//set the stage phase activity
	$psp_id = get_psp_id_for_stage_and_phase($details->stage_id, $details->phase_id);

	if(!$psp_id)
	{
		$psp_id = get_psp_id_for_stage_and_phase(NULL, $details->phase_id);
	}

	$query = "SELECT position FROM {pi_stage_phase_activity} WHERE psp_id = %d ORDER BY position DESC";
	$current_pos = db_result(db_query($query, $psp_id));
	$new_pos = $current_pos + 1;

	insert_stage_phase_activity($psp_id, $new_activity_id, $new_pos);

	//set pi activity node
	add_new_pi_activity_node($node, $node_function, $visible_status, $new_details);

	//set og if necessary for new node
	if($loaded_activity->audience != "individual")
	{
		$users_group = get_group_of_type($uid, $loaded_activity->audience, $new_details->inquiry_id, $new_details->stage_id, $new_details->phase_id, $new_details->activity_id);
		$context['groups'] = array($users_group);
		og_add_group_action($node, $context);
	}
}

function add_new_pi_activity_node($node, $node_function, $visible_status, $details)
{
	global $user;

	$uid = $user->uid;
	//set the PI activity node
	$query = "INSERT INTO {pi_activity_node} SET inquiry_id = %d, activity_id = %d, nid = %d";
	db_query($query, $details->inquiry_id, $details->activity_id, $node->nid);

	$activity = load_activity($details->activity_id);

	$activity_status = load_activity_status($details->inquiry_id, $details->activity_id, $user->uid);

	//if the status was not set specifically for this user and it is an individual activity
	if ($activity->audience == "individual")
	{
		if ($activity_status->uid != $uid)
		{
			$activity_status->uid = $uid;
			unset($activity_status->gid);
			unset($activity_status->pas_id);
		}
	}
	//set the correct group
	else
	{
		$users_group = get_group_of_type($uid, $activity->audience, $details->inquiry_id, $details->stage_id, $details->phase_id, $details->activity_id);
		$context['groups'] = array($users_group);
		og_add_group_action($node, $context);

		if ($users_group != $activity_status->gid)
		{
			$activity_status->gid = $users_group;
			unset($activity_status->pas_id);
			unset($activity_status->uid);
		}
	}

	$loaded_activity_actor  = load_activity_actor ($details->inquiry_id, $details->activity_id);
	$actor_gid = $loaded_activity_actor[0]["gid"];
	$actor_uid = $loaded_activity_actor[0]["uid"];
	$actor_group_type = pi_get_type_of_group($actor_gid);

	$audience_gid = $users_group;
	$audience_group_type = $activity->audience;


	//find out if the audience is wider than the actor for the activity
	//if so, set the status for the audience to view
	if(($audience_group_type == 'class' && $actor_group_type != 'class' && $actor_group_type != 'multiclass' ) || ($audience_group_type == 'group' && !$actor_group_type))
	{
		$activity_status->node_function = 'view';
		$activity_status->visible_status = 'view';
		save_activity_status($activity_status);
			
		//extend activity actor to size of audience
		$loaded_activity_actor[0]["gid"] = $audience_gid;
		update_activity_actor($loaded_activity_actor[0]);
			
		//add extra status for the original actor
		$actor_activity_status = $activity_status;
		unset($activity_status->pas_id);
		$activity_status->gid = $actor_gid;
		$activity_status->uid = $actor_uid;
		$activity_status->node_function = $node_function;
		$activity_status->visible_status = $visible_status;
		save_activity_status($activity_status);
	}
	elseif (($actor_group_type == 'class' && $audience_group_type != 'class' && $audience_group_type != 'multiclass' ) || ($actor_group_type == 'group' && $audience_group_type == 'individual'))
	{
		$activity_status->node_function = $node_function;
		$activity_status->visible_status = $visible_status;
		save_activity_status($activity_status);
	}
	else
	{
		$activity_status->node_function = $node_function;
		$activity_status->visible_status = $visible_status;
		save_activity_status($activity_status);
	}
}

?>
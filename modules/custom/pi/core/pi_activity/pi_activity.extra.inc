<?php
/** @file pi_activity_extra.inc
 *  This is an include file that contains all the extra functions for pi_actitivy, this
 *  has been work in progress as some of the functions are old and to be removed - this is now
 *  mostly done, but some are still out of date/lacking checks
 */

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
 * Returns node_function from pi_activity_status (can be add, edit or view).
 * @param $inquiry_id
 * @param $activity_id
 * @param $uid
 * @return unknown_type
 */
function pi_activity_get_node_function($activity_id, $uid)
{
	$activity_status = pi_activity_get_activity_status_for_user($activity_id, $uid);
	$result = $activity_status['node_function'];
	return $result ;
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
	if ($node_function == $function)
		$result = TRUE;
	else
		$result = FALSE;
	return $result;
}


/**
 * Returns array from pi_activity_actor table.
 * @param uint $inquiry_id
 * @param uint $activity_id
 * @param uint $uid
 * @param uint $gid
 * @return array of activity_actor table with keys: "paa_id", "inquiry_id", "activity_id", "uid", "gid"
 */
function pi_activity_load_activity_actor($activity_id) {
	$query = "SELECT actor FROM {pi_activity} WHERE nid=%d";
	$items = db_query($query, $activity_id);
	$activities = array();
	while ($item = db_fetch_object($items))
	{
		$activity = array();
		$activity["gid"] = $item->actor;
		$activities[] = $activity;
	};
	return $activities;
}


/**
 * Create a link to a particular activity with associated icon
 * @param uint $inquiry_id
 * @param uint $stage_id
 * @param uint $phase_id
 * @param uint $activity_id
 * @param uint $uid
 */
function pi_activity_build_link($activity_id, $uid, $class = 'menu', $title=NULL)
{
	$link = pi_activity_get_link($activity_id, $uid);
	if($title!=NULL)
		$link['title'] = $title;

	$options['html'] = TRUE;
	$options['query'] = $link['destination'];
	$options['attributes'] = array( 'class' => $class);

	$visible_status = pi_activity_get_visible_status_of_activity($activity_id, $uid);
	if($visible_status == 'unavailable')
		$output = t("@link<br/>", array('@link' => $link['title']));
	else
		$output = l($link['title'], $link['url'], $options);
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
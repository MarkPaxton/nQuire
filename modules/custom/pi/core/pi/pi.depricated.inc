<?php
/**
 * This file contains all the functions that should be depricated but have not yhet had ther functionality refactored in dependant modules
 * 
 */

/**
 * Returns an array containing the gids for the groups of a grouping.
 */
function get_groups_from_grouping($grouping_id) {
	$query = "SELECT gid FROM {pi_grouping_group} WHERE grouping_id = %d";
	$result = db_query($query, $grouping_id);
	return $result;
}

/**
 * Returns a grouping_id.
 * @param unknown_type $inquiry_id
 * @param unknown_type $stage_id
 * @param unknown_type $phase_id
 * @param unknown_type $activity_id
 * @return unknown
 */
function get_grouping($inquiry_id, $stage_id = NULL, $phase_id = NULL, $activity_id = NULL)
{
	$query = "SELECT grouping_id FROM {pi_inquiry_grouping} WHERE pi_inquiry_grouping.inquiry_id = %d ";

	$parameters[] = $inquiry_id;

	if($stage_id != NULL)
	{
		$query .= "AND ( {pi_inquiry_grouping}.stage_id = %d OR {pi_inquiry_grouping}.stage_id IS NULL) ";
		$parameters[] = $stage_id;
	}
	if($phase_id != NULL)
	{
		$query .= "AND ( {pi_inquiry_grouping}.phase_id = %d OR {pi_inquiry_grouping}.phase_id IS NULL) ";
		$parameters[] = $phase_id;
	}
	if($activity_id != NULL)
	{
		$query .= "AND ( {pi_inquiry_grouping}.activity_id = %d OR {pi_inquiry_grouping}.activity_id IS NULL) ";
		$parameters[] = $activity_id;
	}

	$result = db_result(db_query($query, $parameters));

	return $result;
}


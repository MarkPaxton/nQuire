<?php


function pi_activity_get_activity_nid_for_content_nid($content_nid) {
	$activity_nid = null;
	$query = "SELECT `activity_id` FROM {pi_activity_node} WHERE `nid` = %d";
	$result = db_fetch_object(db_query($query, $content_nid));
	if ($result) {
		$activity_nid = $result->activity_id;
	}

	return $activity_nid;
}

function _pi_activity_get_children_content_nids_for_activity_and_actor($activity_nid, $collaboration, $actor_nid) {
	$query = "SELECT {pi_activity_node}.nid FROM {pi_activity} LEFT JOIN {pi_activity_node} ON {pi_activity}.nid={pi_activity_node}.activity_id "
					. "WHERE {pi_activity}.parent_activity=%d AND {pi_activity_node}.collaboration='%s' AND {pi_activity_node}.actor=%d";

	$nids = array();
	$result = db_query($query, $activity_nid, $collaboration, $actor_nid);
	while ($row = db_fetch_array($result)) {
		$nids[] = $row['nid'];
	}

	return $nids;
}

function pi_activity_get_children_content_nids_for_content_nid($parent_content_nid) {
	$query = "SELECT activity_id, collaboration, actor FROM {pi_activity_node} WHERE nid=%d";
	$row = db_fetch_array(db_query($query, $parent_content_nid));
	if ($row) {
		return _pi_activity_get_children_content_nids_for_activity_and_actor($row['activity_id'], $row['collaboration'], $row['actor']);
	} else {
		return NULL;
	}
}




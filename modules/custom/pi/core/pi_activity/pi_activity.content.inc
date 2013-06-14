<?php

function pi_activity_get_activity_nid_for_content_nid($content_nid) {
	$activity_nid = null;
	$query = "SELECT `activity_id` FROM {pi_activity_node} WHERE `nid` = %d";
	$result = db_fetch_object(db_query($query, $content_nid));
	if ($result) {
		$activity_nid = $result->activity_id;
	} else {
		$query = "SELECT node.nid FROM {node} LEFT JOIN {pi_activity} ON node.vid=pi_activity.vid WHERE pi_activity.referenced_node = %d";
		$result = db_fetch_object(db_query($query, $content_nid));
		if ($result) {
			$activity_nid = $result->nid;
		}
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

/* TOREMOVE */
function pi_activity_get_children_content_nids_for_content_nid($parent_content_nid) {
	$query = "SELECT activity_id, collaboration, actor FROM {pi_activity_node} WHERE nid=%d";
	$row = db_fetch_array(db_query($query, $parent_content_nid));
	if ($row) {
		return _pi_activity_get_children_content_nids_for_activity_and_actor($row['activity_id'], $row['collaboration'], $row['actor']);
	} else {
		return NULL;
	}
}

/* TOREMOVE */
function pi_activity_get_parent_content_for_child_nid($child_content_nid) {
	$query = "SELECT activity_id, collaboration, actor FROM {pi_activity_node} WHERE nid=%d";
	$row = db_fetch_array(db_query($query, $child_content_nid));
	if ($row) {
		$activity_node = node_load($row['activity_id']);
		$query2 = "SELECT nid FROM {pi_activity_node} WHERE activity_id=%d AND  collaboration='%s' AND actor=%d";
		$row2 = db_fetch_array(db_query($query2, $activity_node->parent_activity, $row['collaboration'], $row['actor']));
		if ($row2) {
			return $row2['nid'];
		}
	}
	return NULL;
}


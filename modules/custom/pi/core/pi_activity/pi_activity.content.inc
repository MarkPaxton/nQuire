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

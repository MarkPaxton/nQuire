<?php

/** Functions dealing with activity content */
function pi_activity_get_content_nid_for_activity($activity_nid, $try_to_create = TRUE) {
	$access_data = pi_inquiry_groupings_get_access_data();
	$activity_data = $access_data->get_activity($activity_nid);

	if ($activity_data['script_activity'] && $activity_data['node']->referenced_node) {
		return $activity_data['node']->referenced_node;
	}

	switch ($activity_data['collaboration']) {
		case 'individual':
			$query = "SELECT nid FROM {pi_activity_node} WHERE activity_id=%d AND collaboration='individual' AND actor=%d";
			$args = array($activity_nid, $access_data->get_uid());
			break;
			break;
		case 'group':
			$query = "SELECT nid FROM {pi_activity_node} WHERE activity_id=%d AND collaboration='group' AND actor=%d";
			$args = array($activity_nid, $access_data->get_subgroup());
			break;
		default:
			$query = "SELECT nid FROM {pi_activity_node} WHERE activity_id=%d AND collaboration='all'";
			$args = array($activity_nid);
			break;
	}

	$row = db_fetch_array(db_query($query, $args));
	if ($row) {
		$nid = $row['nid'];
		return $nid;
	} else if ($try_to_create) {
		$new_node = pi_activity_create_content_on_first_visit($activity_data['node'], $access_data->get_uid());
		if ($new_node) {
			return $new_node->nid;
		}
	}


	return FALSE;
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

function pi_activity_link_content_with_activity($content_node, $activity_nid) {
	$access_data = pi_inquiry_groupings_get_access_data();
	$activity_data = $access_data->get_activity($activity_nid);

	$actor = $activity_data['collaboration'] === 'individual' ? $access_data->get_uid() :
					($activity_data['collaboration'] === 'group' ? $access_data->get_subgroup() : $access_data->get_inquiry_node()->actor_gid);

	$activity_node = array(
			'activity_id' => $activity_nid,
			'nid' => $content_node->nid,
			'collaboration' => $activity_data['collaboration'],
			'actor' => $actor,
	);

	drupal_write_record('pi_activity_node', $activity_node);

	if (!$activity_data['script_activity']) {
		$query = "UPDATE pi_activity SET referenced_node=%d WHERE nid=%d";
		db_query($query, $content_node->nid, $activity_nid);
	}
}

function pi_activity_create_content_on_first_visit($activity, $uid) {
	$types = array(
			'pi_methodology',
			'pi_data_spreadsheet',
			'pi_sort_data',
			'pi_sort_key_answers',
			'pi_sort_key_questions',
			'pi_sort_result_presentations',
	);
	if (in_array($activity->activity_type, $types)) {
		$node = new stdClass();
		$node->uid = $uid;
		$node->type = $activity->activity_type;
		$node->body = '';
		node_save($node);

		pi_activity_link_content_with_activity($node, $activity->nid);

		return $node;
	}

	return NULL;
}




function theme_pi_activity_shared_content_list($content_nids, $display_creator) {
}

function theme_pi_activity_shared_content_node($content_nid, $display_creator) {
	
	
}
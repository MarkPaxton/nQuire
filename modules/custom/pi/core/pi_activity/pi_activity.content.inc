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


/*
 * Used by content nodes that depend on other activities. E.g.: key answers depend on key questions.
 */

function pi_activity_aggregate_shared_content_for_sort_activity_nid_for_use_in_content_nid($content_nid, $parent_activity_nid, $content_type) {
	$access_data = pi_inquiry_groupings_get_access_data();

	if (!$content_nid) { // node is being created
		$using_activity_nid = $access_data->get_current_activity_nid();
		$using_activity_data = $access_data->get_activity($using_activity_nid);
		switch ($using_activity_data['collaboration']) {
			case 'individual':
				$using_actor = $access_data->get_uid();
				break;
			case 'group':
				$using_actor = $access_data->get_subgroup();
				break;
			default:
				$using_actor = $access_data->get_inquiry_node()->actor_gid;
				break;
		}
	} else {
		$using_activity_nid = pi_activity_get_activity_nid_for_content_nid($content_nid);
		$using_activity_data = $access_data->get_activity($using_activity_nid);
		$query = "SELECT actor FROM pi_activity_node WHERE nid=%d";
		$actor_row = db_fetch_array(db_query($query, $content_nid));
		$using_actor = $actor_row ? $actor_row['actor'] : FALSE;
	}

	if ($using_actor) {
		$parent_activity_data = $access_data->get_activity($parent_activity_nid);
		return _pi_activity_get_shared_content_for_activity($access_data->get_inquiry_node(), $parent_activity_data['node'], $parent_activity_data['collaboration'], $using_activity_data['collaboration'], $using_actor, $content_type);
	}

	return NULL;
}

function _pi_activity_get_shared_content_for_activity($inquiry_node, $activity_node, $activity_collaboration, $viewer_collaboration, $viewer_actor, $content_type) {
	if ($viewer_collaboration === $activity_collaboration) {
		$mode = 'single_actor';
		$single_actor = $viewer_actor;
	} else if ($activity_collaboration === 'all') {
		$mode = 'single_actor';
		$single_actor = $inquiry_node->actor_gid;
	} else if ($viewer_collaboration === 'individual' && $activity_collaboration === 'group') {
		$mode = 'single_actor';
		$single_actor = pi_inquiry_groupings_get_user_subgroup($inquiry_node, $viewer_actor);
	} else if ($viewer_collaboration === 'group' && $activity_collaboration === 'individual') {
		$mode = 'shared_within_group';
		$group_members = nquire_group_get_member_uids($viewer_actor, TRUE);
	} else if ($viewer_collaboration === 'all' && $activity_collaboration === 'individual') {
		$mode = 'shared_with_inquiry';
		$all_share_requisite = 'individual_all';
	} else if ($viewer_collaboration === 'all' && $activity_collaboration === 'group') {
		$mode = 'shared_with_inquiry';
		$all_share_requisite = 'group_all';
	}


	$sharing_actors = array();
	switch ($mode) {
		case 'single_actor';
			$sharing_actors[] = $single_actor;
			break;
		case 'shared_within_group';
			$share_query = "SELECT share FROM pi_activity_share WHERE activity_id=%d AND actor=%d";
			foreach ($group_members as $uid) {
				$row = db_fetch_array(db_query($share_query, $activity_node->nid, $uid));
				$share = !$row || $row['share'] !== 'individual_no';
				if ($share) {
					$sharing_actors[] = $uid;
				}
			}
			break;
		case 'shared_with_inquiry':
			$share_query = "SELECT actor FROM pi_activity_share WHERE activity_id=%d AND share='%s'";
			$result = db_query($share_query, $activity_node->nid, $all_share_requisite);
			while ($row = db_fetch_array($result)) {
				$sharing_actors[] = $row['actor'];
			}
	}

	$nids = array();

	if ($content_type === 'parent') {
		$query = "SELECT filtered_pi_activity_node.nid FROM "
						. "(SELECT * FROM pi_activity_node WHERE actor=%d) AS filtered_pi_activity_node "
						. "INNER JOIN (SELECT node.nid as activity_node_nid FROM node LEFT JOIN pi_activity ON node.vid=pi_activity.vid WHERE pi_activity.nid=%d) AS activity_node_table "
						. "ON filtered_pi_activity_node.activity_id=activity_node_table.activity_node_nid ";
		foreach ($sharing_actors as $actor) {
			$result = db_query($query, $actor, $activity_node->nid);
			while ($row = db_fetch_array($result)) {
				$nids[] = $row['nid'];
			}
		}
	} else {
		$parent_type = $activity_node->activity_type;
		$type_data = node_get_types('type', $parent_type);
		$child_type = $type_data->add_node_type;

		$query = "SELECT filtered_pi_activity_node.nid FROM "
						. "(SELECT * FROM pi_activity_node WHERE actor=%d) AS filtered_pi_activity_node "
						. "INNER JOIN (SELECT node.nid as activity_node_nid FROM node LEFT JOIN pi_activity ON node.vid=pi_activity.vid WHERE pi_activity.script_activity=0 AND pi_activity.parent_activity=%d AND pi_activity.activity_type='%s') AS activity_node_table "
						. "ON filtered_pi_activity_node.activity_id=activity_node_table.activity_node_nid ";

		foreach ($sharing_actors as $actor) {
			$result = db_query($query, $actor, $activity_node->nid, $child_type);
			while ($row = db_fetch_array($result)) {
				$nids[] = $row['nid'];
			}
		}
	}

	return $nids;
}


function theme_pi_activity_shared_content_list($content_nids, $display_creator) {
}

function theme_pi_activity_shared_content_node($content_nid, $display_creator) {
	
	
}
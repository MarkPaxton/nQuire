<?php

/** Functions dealing with activity content */
function pi_activity_get_content_nid_for_activity($activity_nid, $try_to_create = TRUE) {
	$access_data = pi_inquiry_groupings_get_access_data();
	$activity_data = $access_data->get_activity($activity_nid);

	if ($activity_data['node']->referenced_node) {
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

function _pi_activity_get_children_content_nids_for_access_data($access_data, $activity_data) {
	$nids = array();
	if ($activity_data) {

		$query = "SELECT {pi_activity_node}.nid FROM {pi_activity} LEFT JOIN {pi_activity_node} ON {pi_activity}.nid={pi_activity_node}.activity_id WHERE {pi_activity}.parent_activity=%d ";
		$args = array($activity_data['node']->nid);
		switch ($activity_data['collaboration']) {
			case 'individual':
				$query .= "AND {pi_activity_node}.collaboration='individual' AND {pi_activity_node}.actor=%d";
				$args[] = $access_data->get_uid();
				break;
			case 'group':
				$query .= "AND {pi_activity_node}.collaboration='group' AND {pi_activity_node}.actor=%d";
				$args[] = $access_data->get_subgroup();
				break;
			default:
				$query .= "AND {pi_activity_node}.collaboration='all'";
				break;
		}

		$result = db_query($query, $args);
		while ($row = db_fetch_array($result)) {
			$nids[] = $row['nid'];
		}
	}
	return $nids;
}

function pi_activity_get_children_content_nids_for_sort_activity_type($sort_activity_type) {
	$access_data = pi_inquiry_groupings_get_access_data();
	$activity_data = $access_data->get_activity_by_type($sort_activity_type);
	return _pi_activity_get_children_content_nids_for_access_data($access_data, $activity_data);
}

function pi_activity_get_children_content_nids_for_sort_activity_nid($sort_activity_nid) {
	$access_data = pi_inquiry_groupings_get_access_data();
	$activity_data = $access_data->get_activity($sort_activity_nid);
	return _pi_activity_get_children_content_nids_for_access_data($access_data, $activity_data);
}

function pi_activity_link_content_with_activity($content_node, $activity_nid) {
	$access_data = pi_inquiry_groupings_get_access_data();
	$activity_data = $access_data->get_activity($activity_nid);

	$actor = $activity_data['collaboration'] === 'individual' ? $access_data->get_uid() :
					($activity_data['collaboration'] === 'group' ? $access_data->get_subgroup() : NULL);

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

/**
 * Returns a list of nids of activity nodes generated for the given pi_activity
 * @param uint $inquiry_id the id of a pi_inquiry to get all related nodes for the activity (n.b. this is probably not needed)
 * @return array(uint) an array of all the activity content nids for the given activity_id
 */
function pi_activity_get_activity_content_nids($activity_id) {
	$activity_content_nids = array();
	$query = "SELECT `nid` FROM {pi_activity_node} WHERE `activity_id` = %d";
	$result = db_query($query, $activity_id);
	while ($row = db_fetch_array($result)) {
		$activity_content_nids[] = $row['nid'];
	}
	return $activity_content_nids;
}

/**
 * Set the OG Audience 'view' permission for the activity content node, e.g. audience will be the user's
 * associated class if the actor was 'class', or do nothing if the audience is individual
 * @param $node The node that was just created/needs to be changed
 * @param $activity_id The associated activity id that created the activity content.
 */
function _pi_activity_set_activity_content_audience(&$node, $activity_id) {
	// Add group audience data for the activity content by getting the user's associated group
	// of the given audience type
	$activity = node_load($activity_id);
	if (pi_check_is_node_of_type($activity, 'pi_activity')) {
		$audience_gids = pi_groups_get_users_audience_groups($node->uid, $activity->actor, $activity->audience);
		$context['groups'] = $audience_gids;
		//pi_debug_message("Adding audience group: " . $context['groups']);
		og_add_group_action($node, $context);
	}
	else
		drupal_set_message(t('Activity @nid is a not an activity.', array('@nid' => $activity_id), 'error'));
}

/**
 * Set the OG Audience 'edit' permission for the activity content node, e.g. audience will be the user's
 * associated class if the actor was 'class', or do nothing if the audience is individual
 * @param unknown_type $node The node that was just created/needs to be changed
 * @param unknown_type $activity_id The associated activity id that created the activity content.
 */
function _pi_activity_set_activity_content_actor(&$node, $activity_id) {
	// Add group audience data for the activity content by getting the user's associated group
	// of the given audience type
	$activity = node_load($activity_id);
	if (pi_check_is_node_of_type($activity, 'pi_activity')) {
		$context['groups'] = pi_groups_get_users_actor_groups($node->uid, $activity->actor, $activity->audience);
		//pi_debug_message("Adding actor group: " . $context['groups']);
		og_add_group_action($node, $context);
	} else {
		drupal_set_message(t('Activity @nid is not an activity.', array('@nid' => $details->activity_id), 'error'));
	}
}

?>
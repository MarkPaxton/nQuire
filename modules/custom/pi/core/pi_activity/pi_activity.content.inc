<?php
/** Functions dealing with activity content */


/**
 * Get the activity content node id for an activity and user content is accessed with the following rules of precedence
 *  1) Directly referenced node from the pi_activity node
 *  2) If activity audience is individual, join pi_activity -> pi_activity_node -> node
 *  	where node.uid = user.uid and return 1st found (should only ever be 0 or 1)
 *  3) If activity is a shared group setting, then check all the group membership of the uuser and get content
 * @param $activity_id
 * @param $uid
 * @return NULL or uint nid
 */
function pi_activity_get_activity_content_nid_for_activity_and_user($activity_id, $uid) {
	$nid_to_return = null;

	$activity = node_load($activity_id);

	// Uuse directly referenced node if set in the activity...
	if($activity->referenced_node != null)
		$nid_to_return = $activity->referenced_node;

	// If there isn't a content nid directly referenced, then work out if it's an individual or shared advicity
	// then check the user's nodes owned by this user, or check the actor and audience group ownerships
	if($nid_to_return==null)
	{
		$nids_to_return = array();
		if($activity->audience == 'individual')
		{
			// individual activity content
			$query = "SELECT node.nid FROM {pi_activity_node} AS pi_activity_node " .
					"LEFT JOIN {node} AS node ON node.nid = pi_activity_node.nid " .
					"WHERE pi_activity_node.activity_id = %d " .
					"AND node.uid = %d";

			$result = db_query($query, $activity_id, $uid);
			while($row = db_fetch_array($result))
				$nids_to_return[] = $row;

			if(count($nids_to_return)>0) {
				if( count($nids_to_return) > 1 )
					drupal_set_message(t("Multiple content for activity for this user found"), 'warning');
			}
		}
		else
		{
			//Shared activity content
			$content_actor_groups = pi_groups_get_users_actor_groups($uid, $activity->actor, $activity->audience);
				
			$users_content_nids = array();
			// get all /activity content nids/ and compare with /all content nids of the users activity actor groups/
			$all_content_nids = pi_activity_get_activity_content_nids($activity_id);
			foreach($content_actor_groups as $actor_gid)
			{
				$group_content_nids = og_group_child_nids($actor_gid);
				$users_content_nids[$actor_gid] = array_intersect($all_content_nids, $group_content_nids);
			}
			// Ideally the user should only have one actor group and one content associated with it
			// so return the first nid in the first group with content.
			if(count($users_content_nids)>1)
			{
				drupal_set_message(t("Multiple actor groups returned for content for group activity @id and user @user - using first group @gid",
						array("@id" => $activity_id, "@user" => $uid, "@gid" => $content_actor_groups[0])), 'warning');
			}

			$count = 0;
			while(count($content_actor_groups[$count])<1)
				$count++;

			$actor_group_nids = array_values($users_content_nids[$content_actor_groups[$count]]);
			$nids_to_return = (is_array($actor_group_nids) ? $actor_group_nids : array());

			if(count($nids_to_return)>1)
			{
				drupal_set_message(t("Multiple nids returned for content for group activity @id and user !user - using first nid @nid",
						array("@id" => $activity_id, "@user" => $uid, "@nid" => $nids_to_return[0])), 'warning');
			}

		}
		if(count($nids_to_return)>0)
			$nid_to_return = $nids_to_return[0];
	}
	return $nid_to_return;
}

/**
 * Returns a list of nids of activity nodes generated for the given pi_activity
 * @param uint $inquiry_id the id of a pi_inquiry to get all related nodes for the activity (n.b. this is probably not needed)
 * @return array(uint) an array of all the activity content nids for the given activity_is
 */
function pi_activity_get_activity_content_nids($activity_id)
{
	$activity_content_nids = array();
	$query = "SELECT `nid` FROM {pi_activity_node} WHERE `activity_id` = %d";
	$result = db_query($query, $activity_id);
	while($row = db_fetch_array($result))
	{
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
function _pi_activity_set_activity_content_audience(&$node, $activity_id)
{
	// Add group audience data for the activity content by getting the user's associated group
	// of the given audience type
	$activity = node_load($activity_id);
	if(pi_check_is_node_of_type($activity, 'pi_activity'))
	{
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
function _pi_activity_set_activity_content_actor(&$node, $activity_id)
{
	// Add group audience data for the activity content by getting the user's associated group
	// of the given audience type
	$activity = node_load($activity_id);
	if(pi_check_is_node_of_type($activity, 'pi_activity'))
	{
		$context['groups'] = pi_groups_get_users_actor_groups($node->uid, $activity->actor, $activity->audience);
		//pi_debug_message("Adding actor group: " . $context['groups']);
		og_add_group_action($node, $context);
	}
	else
	{
		drupal_set_message(t('Activity @nid is not an activity.', array('@nid' => $details->activity_id), 'error'));
	}
}



/**
 * Link an activity content node to a given activity, setting the status, and adding view status to wider audience group
 * @param $node
 * @param $node_function
 * @param $visible_status
 * @param $activity_nid_to_link
 */
function pi_activity_link_activity_content($node, $node_function, $visible_status, $activity_nid_to_link)
{
	// First check the linked activity is valid
	$activity = node_load($activity_nid_to_link);
	if(pi_check_is_node_of_type($activity, 'pi_activity'))
	{
		// First create the link in pi_activity_node
		$activity_node = array();
		$activity_node['activity_id'] = $activity_nid_to_link;
		$activity_node['nid'] = $node->nid;
		drupal_write_record('pi_activity_node', $activity_node);

		// Set the actor group status - get and use existing actor status if it exists otherwise
		// create one. Update is done by passing the key column name in drupal write record, or left
		// as an empty array to do an insert
		if($activity->audience == 'individual') {
			$activity_status_node = pi_activity_get_user_status($activity_nid_to_link, $node->uid);
			$activity_status_node['gid'] == null;
			$activity_status_node['uid'] == $node->uid;
		}
		else {
			$activity_status_node = pi_activity_get_activity_status_for_user($activity_nid_to_link, $node->uid);
			$activity_status_node['gid'] = $activity->actor;
			$activity_status_node['uid'] == null;
		}

		if($activity_status_node['gid']!=null)
			$actor_status_key = 'pas_id';
		else
			$actor_status_key=null;

		$activity_status_node['activity_id'] = $activity->nid;
		$activity_status_node['visible_status'] = $visible_status;
		$activity_status_node['node_function'] = $node_function;
		drupal_write_record('pi_activity_status', $activity_status_node, $actor_status_key);

		// Set group audience to audience of the activity
		$users_group = pi_groups_get_users_audience_groups($node->uid, $activity->audience, $activity->actor);
		$context['groups'] = $users_group;
		//pi_debug_message($context['groups']);
		og_add_group_action($node, $context);

		if(!in_array($activity_status_node->gid, $users_group))
		{
			$activity_status_node['gid'] = $users_group;
			unset($activity_status_node['pas_id']);
		}

		//set the status for the audience to view
		$audience_group_nids = pi_groups_get_activity_audience($activity->actor, $audience_group_type);
		foreach($audience_group_nids as $audience_group_nid)
			pi_activity_set_status($activity_node->nid, $audience_group_nid, 'view', 'view');

		//extend activity actor to size of audience
		// @TODO - change the activity view to get activities with a status set for the user rather than by actor
		if(count($audience_group_nids)>0)  {
			$activity->actor = $audience_gid[0];
			node_save($activity);
		}
			
		//add extra status for the original actor
		$actor_groups =  pi_groups_get_users_actor_groups($node->uid, $activity->audience, $activity->actor);
		foreach($actor_groups as $group_nid)
			pi_activity_set_status($activity_nid_to_link, $group_nid, $node_function, $visible_status);
	}
	else
	{
		drupal_set_message(t('Trying to link an activity content to a node that does not exist or that is not a pi_activity (nid @nid).', array('@nid'=>$activity_nid_to_link)), 'error');
	}
}



?>
<?php 


/**
 * Returns activity visible status and node function, prioritising specific user status (set as uid in table) rather than group ids.
 * @param $activity_id
 * @param $uid
 * @return array(visible_status, node_function)
 */
function pi_activity_get_activity_status_for_user($activity_id, $uid)
{
	$return_status = array();
	$activity  = node_load($activity_id);
	if(pi_check_is_node_of_type($activity, 'pi_activity'))
	{
		// First see if there's a status set specifically for this user
		$return_status = pi_activity_get_user_status($activity_id, $uid);
		if(count($return_status)>1)
		{
			drupal_set_message(t("More than one actor status set for activity !activity for user !uid - and no actor group status nid set, so using first entry: !status, !function.",
					array('!activity' => $activity_id, '!uid' => $uid,
							'!status' => $return_status[0]['visible_status'], '!function' => $return_status[0]['node_function'])), 'warning');
		}
		if(count($return_status)==0)
		{
			// If there's no user status set, then check the activity's Actor group for a status
			// for an individual activity
			if($activity->audience == 'individual')
			{
				if(pi_group_check_membership($uid, $activity->actor))
					$return_status += pi_activity_get_group_status($activity_id, $activity->actor);
			}
			else
			{
				if(count($return_status==0))
				{
					$actor_groups = pi_groups_get_users_actor_groups($uid, $activity->actor, $activity->audience);
					// For group activities, find the actor group for this user and this activity and
					// check the status there
					$users_actor_groups = pi_groups_get_users_actor_groups($uid, $activity->actor, $activity->audience);

					if(count($users_actor_groups)>0)
					{
						foreach($users_actor_groups as $gid)
							$return_status += pi_activity_get_group_status($activity_id, $gid);
					}
					if(count($return_status)>1)
					{
						drupal_set_message(t("More than one actor status set for activity !activity for group(s) !gid - and no actor group status nid set, so using first entry group: !status, !function.",
								array('!activity' => $activity_id, '!gid' => implode(', ', array_keys($return_status)),
										'!status' => $return_status['visible_status'], '!function' => $return_status['node_function'])), 'warning');
					}
					// If there's no specific actor group sed for this user, then check the default actor group
					if(count($return_status==0))
					{
						$return_status += pi_activity_get_group_status($activity_id, $activity->actor);
					}
					if(count($return_status)>1)
					{
						drupal_set_message(t("More than one actor status set for activity !activity for group(s) !gid - and no actor group status nid set, so using first entry group: !status, !function.",
								array('!activity' => $activity_id, '!gid' => implode(', ', array_keys($return_status)),
										'!status' => $return_status['visible_status'], '!function' => $return_status['node_function'])), 'warning');
					}
					if(count($return_status==0))
					{
						// If there's no actor group valid for this user and activity
						// then check the audience status instead
						$users_actor_groups = pi_groups_get_users_audience_groups($uid, $activity->actor, $activity->audience);
						if(count($users_actor_groups)>0)
						{
							foreach($users_actor_groups as $gid)
								$return_status += pi_activity_get_group_status($activity_id, $gid);
						}
						if(count($return_status)>1)
						{
							drupal_set_message(t("More than one audience status set for activity !activity for group(s) !gid - and no actor group status nid set, so using first entry group: !status, !function.",
									array('!activity' => $activity_id, '!gid' => implode(', ', array_keys($return_status)),
											'!status' => $return_status['visible_status'], '!function' => $return_status['node_function'])), 'warning');
						}
					}
				}
			}
		}
	}
	else
	{
    /* FIX (eloy) warning temporary removed */
		//drupal_set_message('pi_activity_get_activity_status_for_user: invalid value for activity_id passed', 'warning');
	}
	if(count($return_status)==0)
		$return_status = NULL;
	else
		$return_status = $return_status[0];
	return $return_status;
}


/**
 *
 * Gets the activity status for an activity for the given group or user.
 * @param unknown_type $activity_nid
 * @param unknown_type $id Either a group id or a user id as indicated by $user_or_group parameter
 * @param unknown_type $user_or_group 'user' or 'group' to indicated what type of status query it is
 * @return array() of activity statuses
 */
function _pi_activity_get_status_query($activity_nid, $id, $user_or_group='user')
{
	if($user_or_group=='group')
		$find_column = 'gid';
	else
		$find_column = 'uid';

	$query = "SELECT * FROM {pi_activity_status} WHERE (`$find_column`=%d) AND (`activity_id`=%d);";
	$result = db_query($query, $id, $activity_nid);
	$results = array();
	while($one_result = db_fetch_array($result))
		$results[] = $one_result;
	return $results;
}

/**
 * Get the status of an activity for a given user.
 *
 * @param uint $activity_nid
 * @param uint $actor
 * @return array() of activity statuses
 */
function pi_activity_get_user_status($activity_nid, $actor)
{
	return _pi_activity_get_status_query($activity_nid, $actor, 'user');
}


/**
 * Get the status of an activity for a given group.
 *
 * @param uint $activity_nid
 * @param uint $actor
 * @return array() of activity statuses
 */
function pi_activity_get_group_status($activity_nid, $actor)
{
	return _pi_activity_get_status_query($activity_nid, $actor, 'group');
}


/**
 * Sets the status of an activity for actor(god, updating if needed
 *
 * @param uint $activity_nid
 * @param uint $actor Group ID
 * @param String $visible
 * @param String $function
 * @returns result of drupal_write_record()
 */
function pi_activity_set_status($activity_nid, $actor, $visible, $function) 
{
  /* FIX (eloy) */
	$existing_status = _pi_activity_get_status_query($activity_nid, $actor, 'group');
	$update = false;
	if($existing_status!=NULL)
	{
		$update=true;
		$status = $existing_status;
	}
	else
	{
		$status = array();
		$status['gid'] = $actor;
		$status['activity_id'] = $activity_nid;
	}
	$status['visible_status'] = $visible;
	$status['node_function'] = $function;
	$return = null;
	if($update)
		$return = drupal_write_record('pi_activity_status', $status, "pas_id");
	else
		drupal_write_record('pi_activity_status', $status);
	return $return;
}


/**
 * Returns visible_status from pi_activity_status (can be start, in_progress, view or unavailable).
 */
function pi_activity_get_visible_status_of_activity($activity_id, $uid) {
	$activity_status = pi_activity_get_activity_status_for_user($activity_id, $uid);
	$result = $activity_status['visible_status'];
	return $result ;
}

/**
 * $status can be either start, in_progress, view or unavailable.
 * Returns FALSE if that user does not have that permission for the
 * specified activity, otherwise it returns TRUE.
 */
function pi_activity_check_visible_status_of_activity($activity_id, $uid, $status) {
	$visible_status = pi_activity_get_visible_status_of_activity($activity_id, $uid);
	if ($visible_status == $status)
	{
		$result = TRUE;
	}
	else
	{
		$result = FALSE;
	}
	return $result;
}

?>
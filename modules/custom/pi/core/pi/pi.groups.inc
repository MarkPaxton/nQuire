<?php
/**
 * @file General functions for dealing with groups in nQuire
 */

/**
 * 
 * Return an array indexed by nid eith value the title of the node, or NULL
 * if the node is not found
 * @param array uint $nids Array of node ids
 * @return  array('nid' => 'title', ...)
 */
function pi_groups_get_descriptions_from_nids($nids)
{
	$return_data = array();
	if(is_array($nids))
	{
		foreach($nids as $nid)
		{
			$n = node_load($nid);
			if($n!=NULL)
			{
				$return_data[$nid] = $n->og_description;
			}
			else
			{
				$return_data[$nid] = NULL;
			}
		}
	}
	return $return_data;
} 


/**
 * 
 * Fill the given parameters with arrays, one containing all groups, with title, parent, children and type data
 * the second an array of all the top level 'tree root' groups in sub arrays buy group type
 * @param $all_groups_by_id array(nid=>group data, ...)
 * @param $tree_roots array($type=>array(nid, nid, nid), ...) containing the ids of all the groups with no parents grouped 
 * by group type, with 'none' for null
 */
function pi_get_group_data_and_root_groups(&$all_groups_by_id, &$tree_roots)
{
	// Get all the groups that are organised in pi_group_hierarchy
	//array('gid' => array('parent'=>'gid', 'children'=>array('gid' ... ), 'group_type'=> string)
	$groups_with_hierarchy = pi_get_all_group_hierarchy_and_types();

	// Get all the groups that are defined as a type in pi_group_types
	//  array(
	// 'class' => 
	//  	array('gid' => array('gid'=>'gid', 'group_type'=> string) ...)
	// 'group' => 
	//  	array('gid' => array('gid'=>'gid', 'group_type'=> string) ...)
	//  .... etc
	$groups_by_type = pi_get_defined_group_nids();
	
	//Create an array to hold pointers to all group info
	$all_groups_by_id = array();
	
	// Organise the groups into ones with parents and no parents and sort by type
	// also add the names of nodes/groups if they're available 
	$tree_roots = array();	
	foreach(array_keys($groups_by_type) as $group_type)
	{
		// this will be an array of group type, then an array of nids and a pointer to the group details
		$tree_roots[$group_type] = array();
		foreach(array_keys($groups_by_type[$group_type]) as $gid)
		{
			$all_groups_by_id[$gid] = &$groups_by_type[$group_type][$gid];
		}
	}
		
	// Create list of organised groups with pointers so groups_by_type and groups_by_id
	//  work with the same data. 
	// if a group had no type information (will be NULL) then set to 'none'
	// Sort ids of all tree root organised groups (ones with no parents) into another array for listing
	foreach($groups_with_hierarchy as $group_id => $group)
	{
		$group_type = $group['group_type'];
		if($group_type==NULL)
		{
			$group_type = 'none';
			$groups_with_hierarchy[$group_id]['group_type'] = $group_type;			
		}
		$groups_by_type[$group_type][$group_id] = &$groups_with_hierarchy[$group_id];	
		$all_groups_by_id[$group_id] = &$groups_with_hierarchy[$group_id];	
	}

	$groups_names = pi_util_get_names_from_nids(array_keys($all_groups_by_id));
	$groups_descriptions = pi_groups_get_descriptions_from_nids(array_keys($all_groups_by_id));
	
	foreach($all_groups_by_id as $gid => $group)
	{
		$group_users = pi_get_members_of_group($gid);
		$all_groups_by_id[$gid]['description'] = theme('pi_group_description', $groups_descriptions[$gid], $group_users);
		$all_groups_by_id[$gid]['title'] = $groups_names[$gid];
	}	
		
	foreach(array_keys($groups_by_type) as $group_type)
	{
		foreach($groups_by_type[$group_type] as $gid => $group)
		{
			if(!array_key_exists($gid, $groups_with_hierarchy))
			{				
				$tree_roots[$group_type][] = $gid;
			}
			else
			{
				if($groups_with_hierarchy[$gid]['parent'] == NULL)
				{
					$tree_roots[$group_type][] = $gid;
				}
			}
		}
	}
}

/**
 * Returns an array containing the uids for the members of the group.
 * The group could be a group, class or multiclass.
 * @param uint $gid Group id to get members of
 * @return array(user) array of db_fetch_array indeced by id
 */
function pi_get_members_of_group($gid) {
	$query = "SELECT uid FROM {og_uid} WHERE nid = %d";
	$result = db_query($query, $gid);
	$members = array();
	while($row = db_fetch_array($result))
	{
		$id = $row['uid'];
		$user = user_load($id);
		if($user)
		{
			$members[$id] = $user;
		}
	}
	return $members;
}


/**
 * 
 * Helper function to get a list of the names of all the users who are member of an og group
 * @param uint $og_id group nid /gid
 * @return string concatonated string of user names separated by a comma.
 */
function pi_group_get_name_list($og_id){

	$users = pi_get_members_of_group($og_id);
	$user_list = array();
	foreach($users as $user)
	{
		$user_list[] .= $user->name;
	}
	$user_string = implode(', ', $user_list);
	return $user_string;
}



/**
 * 
 * Return the type of the group as defined in pi_group_type
 * if the group type is not set it will return null
 * if the group has duplicate type definitions it will returnt the one with the lowest id
 * which is probably the original definition.
 * @param $gid group nid to get the type of
 * @return string group_type:{multiclass,class,group} or NULL
 */
function pi_get_type_of_group($gid) {
	$query = "SELECT type FROM {pi_group_type} WHERE gid = %d ORDER BY pgt_id ASC";
	$result = db_result(db_query($query, $gid));
	return $result;
}

/**
 * Returns an array containing the uids for the members of the group.
 * The group could be a group, class or multiclass.
 */
function get_members_of_group($gid) {
	$query = "SELECT uid FROM {og_uid} WHERE nid = %d";
	$result = db_query($query, $gid);
	return $result;
}

/* DEPRICATED in favour of pi_get_type_of_group
//this funciton needs to move to pi module
function get_type_of_group($gid) {
	$query = "SELECT type FROM {pi_group_type} WHERE gid = %d ";
	$result = db_result(db_query($query, $gid));
	return $result;
}
*/


/**
 * Type can be group, class or multiclass.
 * Returns an array of gid values.
 */
function get_group_of_type($uid, $group_type, $inquiry_id, $stage_id = NULL, $phase_id = NULL, $activity_id = NULL)
{
	$query = "SELECT {pi_group_type}.gid FROM {pi_group_type} "
	 		. "LEFT JOIN {og_uid} ON {og_uid}.nid = {pi_group_type}.gid "
			. "LEFT JOIN {pi_grouping_group} ON {pi_grouping_group}.gid = {pi_group_type}.gid "
			. "LEFT JOIN {pi_inquiry_grouping} ON {pi_inquiry_grouping}.grouping_id = {pi_grouping_group}.grouping_id "
			. "WHERE pi_group_type.type = '%s' AND og_uid.uid =  %d "
			. "AND ( {pi_inquiry_grouping}.inquiry_id = %d OR {pi_inquiry_grouping}.inquiry_id IS NULL) "
			. "AND ( {pi_inquiry_grouping}.stage_id = %d OR {pi_inquiry_grouping}.stage_id IS NULL) "
			. "AND ( {pi_inquiry_grouping}.phase_id = %d OR {pi_inquiry_grouping}.phase_id IS NULL) "
			. "AND ( {pi_inquiry_grouping}.activity_id = %d OR {pi_inquiry_grouping}.activity_id IS NULL)";
	$result = db_result(db_query($query, $group_type, $uid, $inquiry_id, $stage_id, $phase_id, $activity_id));
	// @fixme add warning for multiple groups being returned
	return $result;
}


/**
 * Returns TRUE or FALSE depending on whether user is member of group.
 * @param unknown_type $gid
 * @param unknown_type $uid
 * @return Ambigous <string, NULL>
 */
function check_member_of_group($gid, $uid) {
	$members = get_members_of_group ($gid);

	if ($members == NULL) {
		$result = NULL;
	}
	elseif (in_array($uid, $members)) {
		$result = TRUE;
	}
	else {
		$result = FALSE;
	};

	return $result;
}


/**
 * Returns all groups (student group, class, etc.) of which the user is a member.
 */
function get_groups_of_user($uid) {
	$query = "SELECT nid FROM {og_uid} WHERE uid = %d";
	$result = db_result(db_query($query, $uid));
	return $result;
}


/**
 * Returns the created gid.
 * @param unknown_type $name
 * @param unknown_type $uid_array
 * @param unknown_type $parent_gid
 * @return unknown
 */
function add_group($name, $uid_array, $parent_gid = NULL) {
	$query = "SELECT MAX(nid) FROM {og}";
	$nid = db_result(db_query($query));

	$query = "INSERT INTO {og} SET nid = %d, selective = '0', description = '%s', theme = NULL, register = '0', directory = '1', language = NULL, private = '0'";
	db_query($query, $nid, $name);

	// add admin user as active admin user
	$query = "INSERT INTO {og_uid} SET nid = %d, og_role = '0', is_active = '1', is_admin = '1', uid = '1', created = NOW(), changed = NOW()";
	db_query($query, $nid);

	// add users as active users
	foreach ($uid_array as $uid) 
	{
		$query = "INSERT INTO {og_uid} SET og_role = '0', is_active = '1', is_admin = '0', uid = %d, created = NOW(), changed = NOW()";
		db_query($query, $uid);
	};

	// tdc5 - not too sure about this next bit - looks dodgy??
	$query = "INSERT INTO {pi_group_hierarchy} SET gid = %d, ";
	
	if ($parent_gid == NULL) 
	{
		$query .= "parent_gid = NULL";
		db_query($query, $nid);
	}
	else 
	{
		$query .= "parent_gid = %d";
		db_query($query, $nid, $parent_gid);
	}
	
	return $nid;
}



/**
 * Returns group heirarchy as defined in the pi_group_hewirarchy table in an array indexed by group node id, with another
 * array containing key/values for 'parent' as nid, 'children' array of nids and group_type from pi_group_types if set
 * @return array('gid' => array('parent'=>'gid', 'children'=>array('gid' ... ), 'group_type'=> string)
 */
function pi_get_all_group_hierarchy_and_types()
{
	$return_data = array();
	$query = "SELECT pgh.*, pi_group_type.type FROM {pi_group_hierarchy} AS pgh " . 
			"LEFT JOIN {pi_group_type} as pi_group_type ON pi_group_type.gid = pgh.gid ORDER BY pgh.pgh_id ASC";
	$result = db_query($query);
	
	// initialise an array to hold the ids of any parent nodes that have been added
	// if the parent node isn'tr returned in the query it's type needs to be retreived afterwards	 
	$parents_added = array();
	
	while($row = db_fetch_array($result))
	{
		$gid = $row['gid'];
		if($gid!=null)
		{
			// Add the initial structure if it's not already been mentioned by another heirarchy entry
			if(!array_key_exists($gid, $return_data))
			{
				$return_data[$gid] = array('children'=>array());
			}
			
			// If this row has been pre-set from another row's parent data it can now be updated to contain the type
			// information and removed from parents added array 
			//(the remaining ones in here will be updated specifically at the end)
			if(array_key_exists($gid, $parents_added))
			{
				unset($parents_added[$gid]);
			}
			
			// if the group specifices a parent group nid that hasn't yet been seen, then add it to the results
			if($row['parent_gid']!= NULL)
			{
				if(array_key_exists('parent', $return_data[$gid]))
				{
					if($return_data[$gid]['parent']!= $row['parent_gid'])
					{
						$message = "Group #$gid has more than one parent setting, using first " .
								"one which is #" . $return_data[$gid]['parent'] . " and not #" . $row['parent_gid'];
					}
					else
					{
						$message = "Group #$gid has a duplicate parent setting for group #" . $return_data[$gid]['parent'];
					}
					drupal_set_message(t($message));
				}
				else
				{
					$return_data[$gid]['parent'] = $row['parent_gid'];

					// Add it as a child to parent's children array
					//Create the children array if needed
					if(!array_key_exists($row['parent_gid'], $return_data))
					{
						$parents_added[] = $row['parent_gid'];
						$return_data[$row['parent_gid']] = array('children'=>array());
					}

					if(in_array($gid, $return_data[$row['parent_gid']]['children']))
					{
						drupal_set_message(t("Group $gid has a duplicate hierarchy entry for belonging to " . $row['parent_gid']));
					}
					else
					{
						$return_data[$row['parent_gid']]['children'][] = $gid;
					}
				}
			}
			if(array_key_exists('group_type', $return_data[$gid]))
			{
				if($row['type']!= $return_data[$gid]['group_type'])
				{
					drupal_set_message(t("Group #$gid has conflicting type settings " .
						"one which is " . $return_data[$gid]['group_type'] . " and not " . $row['type']));	
				}
			}
			else
			{
				$return_data[$gid]['group_type'] = $row['type'];
			}
			foreach($parents_added as $gid)
			{
				$return_data[$gid]['group_type'] = pi_get_type_of_group($gid);
			}
		}
	}
	return $return_data;
}


/**
 * Gets the heirarchy data for a particular group and returns it in an array
 * @param uint $group_id nid of the group to get heirarchy data from 
 * @return array('parent'=>NULL/nid, 'children' => array(nids))
 */
function pi_get_group_hierarchy($group_id)
{
	$hierarchy = array('parent'=>NULL, 'children' => array());
	
	// First get the parent id and check for multiples
	$query = "SELECT pgh_id, parent_gid FROM {pi_group_hierarchy} WHERE gid = %d ORDER BY pgh_id ASC";
	$result = db_query($query, $group_id);
	$result_rows = array();
	$parent_id = NULL;
	while($result_row = db_fetch_array($result))
	{
		$result_rows[$result_row['pgh_id']] = $result_row['parent_gid'];	
		if($parent_id==NULL)
		{
			$parent_id = $result_row['parent_gid'];
		}
		else
		{
			$print_value = ($result_row['parent_gid'] == NULL) ? "NULL" : "#" . $result_row['parent_gid'];
			drupal_set_message(t("Group #$group_id has more than one parent set, using first " . 
				"one which is #$parent_id and not " . $print_value));			
		}
	}
	$hierarchy['parent'] = $parent_id;
	
	//now get the child ids and put them into an array
	$query = "SELECT pgh_id, gid FROM {pi_group_hierarchy} WHERE parent_gid = %d ORDER BY pgh_id ASC";
	//pi_debug_message(sprintf($query, $group_id));
	$result = db_query($query, $group_id);
	$children = array();
	$seen_children = array();
	while($result_row = db_fetch_array($result))
	{
		if(in_array($result_row['gid'], $seen_children))
		{
			drupal_set_message(t("Group " . $result_row['gid'] . " has a duplicate hierarchy entry (" . $result_row['pgh_id'] . ")"));
		}
		else
		{
			$children[$result_row['pgh_id']] = $result_row['gid'];
			$seen_children[] = $result_row['gid'];
		}	
	}
	$hierarchy['children'] = $children;	
	//pi_debug_message($children);
	return $hierarchy;	
}


/**
 * 
 * Returns an array of nids of groups of a given type
 * @param  string $type type of groups to find: class, multiclass, group
 * @return array of arrays, with keys equal to group nids and arrays containtin nid=>$nid and 'group_type'=>$type   
 */
function pi_get_group_nids_of_type($type)
{
	$groups = array();
	
	//First get inquiry actor group
	$query = "SELECT gid, type FROM {pi_group_type} WHERE type= '%s' ORDER BY gid ASC";
	$result = db_query($query, $type);
	while($id_row = db_fetch_array($result))
	{
		$id = $id_row['gid'];
		$group['nid'] = $id;
		$group['group_type'] = $id_row['type'];
		if(!array_key_exists($id, $groups))
		{
			$groups[$id] = $group;
		}
		else
		{
			drupal_set_message(t("Duplicate group definitions found for group #") . check_plain($id) . 
				t(" as type '") . check_plain($groups[$id]['group_type']) . "'.", 'warning');
		}
	}
	return $groups;	
}

/**
 * Functions in this block are for groups
 */

/**
 * @return array if nids of published groups
 */
function pi_get_all_group_nids() {
	$groups = array();
	$query = "SELECT og.nid FROM {og}";
	$result = db_query($query);
	while($id_row = db_fetch_array($result))
	{
		$id = $id_row['nid'];
		$group = node_load($id);
		if($group->status)
		{
			$groups[$id] = $id;
		}  		
	}
	return $groups;
}


/**
 * Get an array describing all groups defined as multiclass/class/group in nquire
 * array is indexed by group_type then nid (aka gid) and contains an array of elements for group id and type 
 * @return array[group_type][nid] => array('nid', 'group_type') An array of arrays with group nids 
 * and group type defiend as class, multiclass, group and indexed by group nid
 * group type is indexed as 'group_type' 
 */
function pi_get_defined_group_nids() {
	$groups = array();
	$groups_seen = array();
	
	//First get inquiry actor group
	$query = "SELECT DISTINCT type FROM {pi_group_type}";
	$result = db_query($query);
	while($type_row = db_fetch_array($result))
	{
		$type = $type_row['type'];
		if(!array_key_exists($type, $groups))
		{
			$groups[$type] = array();
		}
		$type_gids = pi_get_group_nids_of_type($type);
		foreach($type_gids as $id => $group_data)
		{
			$group = array();
			$group['nid'] = $id;
			$group['group_type'] = $type;
			
			if(array_key_exists($id, $groups_seen))
			{
				drupal_set_message(t("Conflicting group definitions found for group ") . check_plain($id) . 
				t(". Using first value only, which is '") . check_plain($groups_seen[$id]['group_type']) . t("' and not using type '") . check_plain($type) . "'.", 'warning');
			}
			else
			{
				$groups[$type][$id] = $group;
				$groups_seen[$id] = $group;
			}
		}
	}
	return $groups;
}

/**
 * Helper function to get all the users who are member of an og group
 * 
 * @param unknown_type $og
 * @return unknown
 */
function get_og_group_name($og_id){
	$query = "SELECT og_description FROM {og} WHERE nid = %d";
	$group = db_fetch_object(db_query($query), $og_id);
	return $group->og_description;
}



/**
 * Create a list of groups and subgroups ids using the group hierarchy in $defined groups
 * and the names of the nodes - groups with children will be recursively traversed
 * @param array $parents array of nids
 * @param array $groups_with_hierarchy pointer to array of group hierarchy data indexed by nid
 * @return array($nid=>array(...)) array with key parent nid and value null or array if it has children
 */
function pi_groups_generate_child_list($parents, &$group_data)
{
	$child_list = array();
	foreach($parents as $nid)
	{
		if(count($group_data[$nid]['children'])>0)
		{
			$grandchild_list =  pi_groups_generate_child_list($group_data[$nid]['children'], $group_data); 
			$child_list[$nid] = $grandchild_list;
		}
		else
		{
			$child_list[$nid] = NULL;
		}
	}
	return $child_list;
}

/**
 * 
 * Theme function to generate a group title with name (group type) or 'unknown' if the data isn't available 
 * @param $name
 * @param $group_type
 */
function theme_pi_group_title($name, $group_type)
{
	$output = '';
	if($group_type!=NULL)
	{
		$group_type_text = check_plain(ucfirst($group_type));
	}
	else
	{
		$group_type = t('None');
	}
	if($name!=NULL)
	{
		$output_name = $name;
	}
	else
	{
		$output_name = t('Unknown');
	}
	$output .= $output_name . " ($group_type_text)";
	return $output;
}


function theme_pi_group_description($description, $members=NULL)
{
	$user_names = array();
	if(is_array($members))
	{
		foreach($members as $user)
		{
			$user_names[] = l($user->name, '/user/' . $user->uid);
		}
	}
	$group_description = '<div>' . $description . '<br />'
		. implode(', ', $user_names) . '</div>';
	return $group_description;
}

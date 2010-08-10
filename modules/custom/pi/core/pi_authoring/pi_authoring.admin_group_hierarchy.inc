<?php
/**
 *  @file PI authoring functions - hierarchy editing pages
 */

function pi_authoring_admin_group_hierarchy_view() {
	$output = '<p>' . t('Use this page to assign your groups into their respective parents: e.g. all the Year 10 classes into the Year 10 group.  ' .
	"If there are no groups listed, you need to create some using the Groups menu item, then categorise them on the nQurire groups page.</p>");
	
	// Get all the groups that are organised in pi_group_hierarchy
	$organised_groups = pi_get_all_group_hierarchy_and_types();

	// Get all the groups that are defined as a type in pi_group_types
	$defined_groups = pi_get_defined_group_nids();
	
	//now organise them into ones with parents and no parents
	// and by type
	$tree_roots = array();
	
	$group_order = array(
		'multiclass'=>'Multiclass groups', 
		'class'=>'Classes not assigned to a multiclass', 
		'group'=>'Groups with no class');
	foreach(array_keys($group_order) as $group_type)
	{
		$tree_roots[$group_type] = array();
	}
	
	// Load all group names on mass first
	$organised_groups_names = pi_authoring_get_names_from_nids(array_keys($organised_groups));

	//add titles to these groups
	foreach(array_keys($defined_groups) as $group_type)
	{
		$defined_groups_names = pi_authoring_get_names_from_nids(array_keys($defined_groups[$group_type]));
		foreach(array_keys($defined_groups[$group_type]) as $gid)
		{
			$defined_groups[$group_type][$gid]['title'] = $defined_groups_names[$gid];
		}
	}
	
	// Add group titles to the defined groups array and get a list of top level 'orphan'
	// groups to use as tree roots for groups.
	// These may not all be correctly filed as multiclass, so check each type specifically
	// so that they can be grouped by type on screen and a warning can be generated for illogical
	// groups such as multiclass belonging to class, or (an in class) group with no class parent.
	foreach($organised_groups as $gid => $group)
	{
		
		$organised_groups[$gid]['title'] = $organised_groups_names[$gid];
		if($group['group_type']==NULL)
		{
			$groups_type = 'none';
		}
		else
		{
			$groups_type = $group['group_type'];
		}
		//Remove any group that is are already organised...
		unset($defined_groups[$groups_type][$gid]);
		
		if($group['parent'] == NULL)
		{
			$tree_roots[$groups_type][] = $gid;			
		}
	}
	// Render every orphan group by type for the tree root groupings
	$top_level_group_rows = array();
	foreach($group_order as $group_type_to_list => $group_description)	
	{
		$expanded_tree_roots = array();		
		// Check if there are any tree roots for this type of group
		if(count($tree_roots[$group_type_to_list])>0)
		{
			//Render each tree within an item list for the group description

			// First go through the trees and add nids for any branches/children 
			$full_tree_structures = pi_authoring_generate_child_list($tree_roots[$group_type_to_list], $organised_groups);
			// Now render all branches of the trees
			$expanded_tree_roots[] = pi_authoring_render_group_tree($full_tree_structures, $organised_groups);
		}
		else
		{
			// If there are no tree roots in this type then render a list with title and "none".
			$expanded_tree_roots[] = t("None");
		}
		
		// Now list the un-sorted groups
		//pi_debug_message(array_keys($defined_groups[$group_type_to_list]));
		pi_debug_message($defined_groups[$group_type_to_list]);
		if(count($defined_groups[$group_type_to_list])>0)
		{
			$expanded_tree_roots[] = pi_authoring_render_group_tree($defined_groups[$group_type_to_list], $defined_groups[$group_type_to_list]);
		}
		// Finally render the list 
		$output .= theme('item_list', $expanded_tree_roots, $group_description);
	}
	return $output;
}

function pi_authoring_admin_group_hierarchy_form($form_state)
{
	$form = array();
	$form['#prefix'] = '';
	
	// Get all the groups that were defined by heirarchy
	$organised_groups = pi_get_all_group_heararchy_and_types();
	
	//now organise them into ones with parents and no parents
	// and types
	$orphans = array();
	$groups_by_type = array();
	foreach($organised_groups as $gid => $group)
	{
		//pi_debug_message($groups);
		if($group['group_type']!=NULL)
		{
			$groups_by_type['none'][] = $gid;
		}
		else
		{
			$groups_by_type[$group['group_type']][] = $gid;								
		}
		
		if($group['parent_id'] == NULL)
		{
			$orphans[] = $gid; 
		}
	}
	
	foreach($groups_by_type['multiclass'] as $gid => $group_heirarchy)
	{
		$group = node_load($gid);
		if($group)
		{
			$output .= "<div><strong>" . $group->title . "</strong></div>";
			if($group_heirarchy['parent']!=NULL)
			{
				$parent = node_load($group_heirarchy['parent']);
				if($parent)
				{
					$output .= "<div>Parent: " . $parent->title . "</div>";
				}
				else
				{
					$output .= "<div>No parent</div>";					
				}
			}
			if(count($group_heirarchy['children']))
			{
				$output .= "<ul>";
				foreach($group_heirarchy['children'] as $gid)
				{
					$child = node_load($gid);
					$output .= "<li>" . $child->title . "</li>";
				}
				$output .= "</ul>";				
			}
		}		
	}
	$form['#prefix'] .= $output;
	return $form;
}


function pi_authoring_get_names_from_nids($nids)
{
	$return_data = array();
	if(is_array($nids))
	{
		foreach($nids as $nid)
		{
			$n = node_load($nid);
			if($n!=NULL)
			{
				$return_data[$nid] = $n->title;
			}
			else
			{
				$return_data[$nid] = NULL;
			}
		}
	}
	return $return_data;
} 

/*
 * Create a list of groups and subgroups ids using the group hierarchy in $defined groups
 * and the names of the nodes - groups with children will be recursively traversed
 * @param arrauy(uint) $children array of nids of child nodes
 * @param array() $organised_groups array of group hierarchy data indexed by nid.
 * @return array($nid=>array(...)) array with key parent nid and value null or array if it has children
 */
function pi_authoring_generate_child_list($parents, &$group_data)
{
	$child_list = array();
	foreach($parents as $nid)
	{
		if(count($group_data[$nid]['children'])>0)
		{
			$grandchild_list =  pi_authoring_generate_child_list($group_data[$nid]['children'], $group_data); 
			$child_list[$nid] = $grandchild_list;
		}
		else
		{
			$child_list[$nid] = NULL;
		}
	}
	return $child_list;
}

function theme_pi_authoring_group_title($name, $group_type)
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

/**
 * Expand an tree of nids by replacing each element with it's value if it's not an array
 * or another expanded tree if it's an array.
 * Expanded lists will be rendered using theme('item_list') with it's title taken from the key used as
 * a nid.   
 * @param $group_list array indexed by nid of a group
 * @param $group_data array containing the description data of groups with titles 
 */
function pi_authoring_render_group_tree($group_list, &$group_data)
{
	$rows_to_return = array();
	// Check everything in the list and return either the name or the 
	// expanded sub-list (if it's an array) 
	foreach($group_list as $group_id => $group_items) 
	{
		$item_name = theme('pi_authoring_group_title', l($group_data[$group_id]['title'], 'node/' . $group_id . '/edit', array('query' => drupal_get_destination()) ), $group_data[$group_id]['group_type']);
		if(is_array($group_items))
		{
			$expanded_group_list = pi_authoring_render_group_tree($group_items, $group_data);
			$rows_to_return[] =  theme('item_list', $expanded_group_list, $item_name);
		}
		else
		{
			$rows_to_return[] = $item_name;
		}
	}
	return $rows_to_return;
}


function theme_pi_authoring_admin_group_hierarchy_form($form)
{
	$output  ='';
	$output .= drupal_render($form);
	return $output;
}


?>
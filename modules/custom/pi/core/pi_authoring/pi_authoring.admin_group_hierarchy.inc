<?php
/**
 *  @file PI authoring functions - hierarchy editing pages
 */

function pi_authoring_admin_group_hierarchy_view() {
	$output = '<p>' . t('Use this page to assign your groups into their respective parents: e.g. all the Year 10 classes into the Year 10 group.  ' .
	"If there are no groups listed, you need to create some using the Groups menu item, then categorise them on the nQurire groups page.</p>");
	
	$tree_roots = array();	
	$all_groups_by_id = array();
	pi_get_group_data_and_root_groups($all_groups_by_id, $tree_roots);
	
	// Groups may not all be correctly filed as multiclass, so check each type specifically
	// so that they can be grouped by type on screen and a warning can be generated for illogical
	// groups such as multiclass belonging to class, or (an in class) group with no class parent.

	// Render every orphan group by type for the tree root groupings
	// Set up a print order and some default text
	$group_order = array(
		'multiclass'=>t('Multiclass groups'), 
		'class'=>t('Classes not assigned to a multiclass'), 
		'group'=>t('Groups with no class'),
		'none' => t('Groups not categorised')
	);
	
	$output .= theme('pi_authoring_admin_group_hierarchy_list', $group_order, $tree_roots, $all_groups_by_id);
	return $output;
}

function pi_authoring_admin_group_hierarchy_form($form_state)
{
	$form = array();
	$form['#prefix'] = t('Use this page to assign your groups into their respective parents: e.g. all the Year 10 classes into the Year 10 group.  ' .
	"If there are no groups listed, you need to create some using the Groups menu item, then categorise them on the nQurire groups page.</p>");
	
	$all_groups_by_id = array();
	$tree_roots = array();	

	pi_get_group_data_and_root_groups($all_groups_by_id, $tree_roots);
	
	// Groups may not all be correctly filed as multiclass, so check each type specifically
	// so that they can be grouped by type on screen and a warning can be generated for illogical
	// groups such as multiclass belonging to class, or (an in class) group with no class parent.

	// Render every orphan group by type for the tree root groupings
	// Set up a print order and some default text
	$group_order = array(
		'multiclass'=>t('Multiclass groups'), 
		'class'=>t('Classes not assigned to a multiclass'), 
		'group'=>t('Groups with no class'),
		'none' => t('Groups not categorised')
	);
	
	return $form;
}

/**
 * 
 * Return an array indexed by nid eith value the title of the node, or NULL
 * if the node is not found
 * @param array uint $nids Array of node ids
 * @return  array('nid' => 'title', ...)
 */
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
 * @param array() $groups_with_hierarchy array of group hierarchy data indexed by nid.
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
			//pi_debug_message("expanding $group_id :" . count($group_items));
			$expanded_group_list = pi_authoring_render_group_tree($group_items, $group_data);
			$rows_to_return[] =  theme('item_list', $expanded_group_list, $item_name);
		}
		else
		{
			//pi_debug_message("not expanding $group_id :" . $item_name);
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

/**
 * 
 * Theme function to render the group heirarchy
 * @param $group_order array with group names as key and values as desriptions of each group type 
 * @param $group_list_by_type
 * @param $all_groups
 */
function theme_pi_authoring_admin_group_hierarchy_list($group_order, $group_list_by_type, $all_groups)
{
	$output = '';
	foreach($group_order as $group_type_to_list => $group_description)	
	{
		$output .= '<h2>' . $group_description . '</h2>';
		
		// Check if there are any tree roots for this type of group
		if(count($group_list_by_type[$group_type_to_list])>0)
		{
			//Render each tree within an item list for the group description

			// First go through the trees and add nids for any branches/children 
			$full_tree_structures = pi_authoring_generate_child_list($group_list_by_type[$group_type_to_list], $all_groups);
			// Now render all branches of the trees
			$expanded_root = pi_authoring_render_group_tree($full_tree_structures, $all_groups);	
			asort($expanded_root);
			$output .= theme('item_list', $expanded_root);
		}
		
		if(count($group_list_by_type[$group_type_to_list])==0)
		{
			// If there are no tree roots in this type then render a list with title and "none".
			$output .= t("None");
		}
	}
	return $output;
}

?>
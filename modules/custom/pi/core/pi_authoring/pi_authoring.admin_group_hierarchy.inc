<?php
/**
 *  @file PI authoring functions - hierarchy editing pages
 */

/**
 * Generate a list of all groups defined in nQuire and show them in a tree based on group hierarchy.
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

function pi_authoring_admin_group_hierarchy_edit() {
	$output = ''; 
	$output .= drupal_get_form('pi_authoring_admin_group_hierarchy_form');
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
	
	$multiclass_groups = pi_get_group_nids_of_type('multiclass');
	$class_groups = pi_get_group_nids_of_type('class');
	$group_groups = pi_get_group_nids_of_type('group');
	$undefined_groups = array();
	
	// Collect together all groups that are not yet allocated group types
	foreach(array_keys($all_groups_by_id) as $gid)
	{
		if(!(array_key_exists($gid, $multiclass_groups) ||
			array_key_exists($gid, $class_groups) || 
			array_key_exists($gid, $group_groups)))
		{
			$undefined_groups[] = $gid;
		}
	}
	$undefined_names = pi_authoring_get_names_from_nids($undefined_groups);
	if(count($undefined_groups)>0)
	{
		drupal_set_message(t('Some groups have not yet been defined as multiclass, class or group; please do this before sorting them.'));
	}

	//Add 'none' groups for items that don't have parents (but should)
	$all_groups_by_id['multiclass-null'] = array(
		'title'=>'No multiclass', 
		'group_type'=>'multiclass',
		'parent' => null,
		'children' => $tree_roots['class']
	);
	
	$all_groups_by_id['class-null'] = array(
		'title'=>'No class',
		'group_type'=>'class',
		'parent' => 'multiclass-null',
		'children' => $tree_roots['group']
	);

	
	//This drops 'none' grouped items into the above placeholder multiclass/class items
	// and adds those placeholder items to the form data structure
	$form_data_structure = array();
	foreach($tree_roots['multiclass'] as $multiclass_group_id)
	{		
		$form_data_structure[] = $multiclass_group_id;
	}
	
	foreach($tree_roots['class'] as $class_group_id)
	{		
		$all_groups_by_id['multiclass-null']['children'][] = $class_group_id;
		$all_groups_by_id[$class_group_id]['parent'] = 'multiclass-null';
	}
	$form_data_structure[] = 'multiclass-null';

	$all_groups_by_id['multiclass-null']['children'][] = 'class-null';
	foreach($tree_roots['group'] as $group_group_id)
	{
		$all_groups_by_id['class-null']['children'][] = $group_group_id;
		$all_groups_by_id[$group_group_id]['parent'] = 'class-null';
	}

	
	// Generate the group hierarchy for the form data structure
	$groups_hierarchy = pi_authoring_generate_child_list($form_data_structure, $all_groups_by_id);
	
	// These are the region options for the respective subgroups (
	//  e.g multiclass groups for class groups to be put in)
	$multiclass_options = array();
	foreach(array_keys($multiclass_groups) as $nid)
	{
		$multiclass_options[$nid] = $all_groups_by_id[$nid]['title'];		
	}
	$multiclass_options['multiclass-null'] = $all_groups_by_id['multiclass-null']['title'];
	
	$class_options = array();
	foreach(array_keys($class_groups) as $nid)
	{
		$class_options[$nid] = $all_groups_by_id[$nid]['title'];		
	}
	$class_options['class-null'] = $all_groups_by_id['class-null']['title'];
	
	$form['class_hierarchy'] = array('#tree'=>TRUE);
	$form['group_hierarchy'] = array('#tree'=>TRUE);
	
	/* Top level : Multiclass */
	// Set up a region for each multiclass group
	foreach($groups_hierarchy as $multiclass_group_nid => $multiclass_decendants)
	{
		$form['class_hierarchy'][$multiclass_group_nid] = array();
		$form['class_hierarchy'][$multiclass_group_nid]['#title'] =  $all_groups_by_id[$multiclass_group_nid]['title'];
		
		/* Second level: Class */
		foreach($multiclass_decendants as $multiclass_child_nid => $multiclass_child_decendants)
		{
			$form['class_hierarchy'][$multiclass_group_nid][$multiclass_child_nid] = array();
			//Use a pointer to keep things readable
			$class_form_element = &$form['class_hierarchy'][$multiclass_group_nid][$multiclass_child_nid];
			
			$class_form_element['title'] = array(
				'#value' => $all_groups_by_id[$multiclass_child_nid]['title']
			);

			$class_form_element['description'] = array(
				'#value' => ''
			);
			
			$class_form_element['region'] = array(
				'#type'          => 'select',
				'#default_value' => $multiclass_group_nid,
				'#options' => $multiclass_options
			);
			
			$class_form_element['weight'] = array(
				'#type'          => 'weight',
				'#delta'         => count($class_groups),
				'#default_value' => count(element_children($form['class_hierarchy'][$multiclass_group_nid])),
			);
			
			/* Third (last) level: Groups */
			$form['group_hierarchy'][$multiclass_child_nid]['#title'] =  $all_groups_by_id[$multiclass_child_nid]['title'];
			foreach(array_keys($multiclass_child_decendants) as $class_child_nid)
			{
				$form['group_hierarchy'][$multiclass_child_nid][$class_child_nid] = array();
				//Another pointer to keep code readable
				$group_form_element = &$form['group_hierarchy'][$multiclass_child_nid][$class_child_nid];
				
				$group_form_element['title'] = array(
					'#value' => $all_groups_by_id[$class_child_nid]['title']
				);

				$group_form_element['description'] = array(
					'#value' => ''
				);
				
				$group_form_element['region'] = array(
					'#type'          => 'select',
					'#default_value' => $multiclass_child_nid,
					'#options' => $class_options
				);
				
				$group_form_element['weight'] = array(
					'#type'          => 'weight',
					'#delta'         => count($group_groups),
					'#default_value' => count(element_children($form['class_hierarchy'][$multiclass_child_nid])),
				);
				
			}
			
		}
	}
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Save'),
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

/**
 * Create a list of groups and subgroups ids using the group hierarchy in $defined groups
 * and the names of the nodes - groups with children will be recursively traversed
 * @param array $parents array of nids
 * @param array $groups_with_hierarchy pointer to array of group hierarchy data indexed by nid
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

function pi_authoring_admin_group_hierarchy_elements($group_list, &$group_data)
{
	$form_data = array();
	
	
	return $form_data;
}


function theme_pi_authoring_admin_group_hierarchy_form($form) {
	$output = '';
		
	// Region-sort form header
	$header = array(	array( 'theading' => t('Group'), 'class' => '' ),
						array( 'theading' => t('Description'), 'class' => '' ),
						array( 'theading' => t('Group Parent'), 'class' => 'sort_theading_region' ),
						array( 'theading' => t('Weight'), 'class' => 'sort_theading_weight' )
	);
	
	// Create the region and the rows for the region sort form
	$class_hierarchy_elements = &$form['class_hierarchy'];
	$group_hierarchy_elements = &$form['group_hierarchy'];
	
	$class_rows =  pi_authoring_convert_heirarchy_form_elements($class_hierarchy_elements);	
	$group_rows =  pi_authoring_convert_heirarchy_form_elements($group_hierarchy_elements);	

	if(count($class_rows) == 0)
	{
		drupal_set_message(t("No groups available to organise, check they have been set up correctly."));
	}
	else
	{
		
		$class_regions = pi_authoring_admin_group_heairarchy_form_generate_region_list($class_rows, $form['class_hierarchy']);
		$group_regions = pi_authoring_admin_group_heairarchy_form_generate_region_list($group_rows, $form['group_hierarchy']);
		//$output .= theme('table', $header, $rows, array('id' => 'sort_list'));
		$output .= theme('pi_region_sort_table', $header, $class_rows, $class_regions, array('id' => 'sort_list'));
		$output .= theme('pi_region_sort_table', $header, $group_rows, $group_regions, array('id' => 'sort_list'));
		//$output .= drupal_render($form);
		
		
		
	}
	return $output;
}


/**
 * 
 * Theme function to render the group hierarchy
 * @param $group_order array with group names as key and values as descriptions of each group type 
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

function pi_authoring_admin_group_heairarchy_form_generate_region_list($region_items, $form_elements)
{
	$regions = array();
	$names = pi_authoring_get_names_from_nids(element_children($region_items));
	foreach($names as $region_element_id => $details)
	{
		switch($region_element_id)
		{
			case 'multiclass-null':
				$regions[$region_element_id]['title'] =  $form_elements[$region_element_id]['#title'];
				$regions[$region_element_id]['description'] = '';
				break;
			case 'class-null':
				$regions[$region_element_id]['title'] = $form_elements[$region_element_id]['#title'];
				$regions[$region_element_id]['description'] = '';
				break;
			default:
				$item = node_load($region_element_id);
				if($item != NULL)
				{
					$regions[$region_element_id]['title'] = $item->title;
					$regions[$region_element_id]['description'] = $item->og_description;
				}
				else
				{
					$regions[$region_element_id]['title'] = $names[$region_element_id];
					$regions[$region_element_id]['description'] = '';
				}
		}
	}
	return $regions;
}

function pi_authoring_convert_heirarchy_form_elements($hierarchy_elements)
{
	$rows = array();
	foreach (element_children($hierarchy_elements) as $region)
	{
		foreach (element_children($hierarchy_elements[$region]) as $key)
		{
			$hierarchy_elements[$region][$key]['weight']['#attributes']['class'] = 'sort_list-weight sort_list-weight-'. $region;
			$hierarchy_elements[$region][$key]['region']['#attributes']['class'] = 'sort_list-region-select sort_list-region-'. $region;

			$rows[$region][$key]['title'] = drupal_render($hierarchy_elements[$region][$key]['title']);
			$rows[$region][$key]['description'] = drupal_render($hierarchy_elements[$region][$key]['description']);
			$rows[$region][$key]['region'] = drupal_render($hierarchy_elements[$region][$key]['region']);
			$rows[$region][$key]['weight'] = drupal_render($hierarchy_elements[$region][$key]['weight']);
		}
	}
	return $rows;
}

?>
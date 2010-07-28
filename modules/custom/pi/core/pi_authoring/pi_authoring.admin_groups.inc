<?php
/** @file Functions and templates for inquiry group definitions
 * 
 */


/** Generates the page to that contains the form to allow assigning the groups as multiclass/class/group 
 * @return string
 */
function pi_authoring_admin_groups() {
	$content = '';
	$content .= drupal_get_form('pi_authoring_admin_groups_form');	
	return $content;
}

/**
 * This form is a standard hook_form() function to generate the form to sort groups into 
 * multiclass, class, group and not used groups for nquire. They are sorted by group and 
 * then by group title alphabetically, so weight is only used during the form editing process
 * @param array $form_state form state as per drupal api
 */
function pi_authoring_admin_groups_form($form_state)
{
	// Build the form
	$form = array();

	$form['#prefix'] = '';
	
	// measures_list and tree used for draggable menu items
	$form['groups_list'] = array(
        '#tree'   => TRUE,
	);
		 	
	$group_order = array('multiclass', 'class', 'group', 'none');

	// Get the list of currently defined groups in the installation
	$defined_groups_unsorted = pi_get_defined_group_nids();
	$defined_groups = pi_sort_grouped_item_list($group_order, $defined_groups_unsorted, 'weight');

	// Get the list of available groups
	$all_groups = pi_get_all_group_nids();
	
    $sorted_groups = pi_merge_content_item_lists($defined_groups, $all_groups);

	// Prepare the form by adding an array for each expected group type
	// this is important as if there are no items in the group then it would not exist in the form
	// and the submitted form would not have this type defined even though it looks for it.
	foreach($group_order as $group_type)
	{
		$form['groups_list'][$group_type] = array();
	}
	
	// Now populate the groups elements with the sortable items
	$tree = &$form['groups_list'];
	$group_names = array();
	foreach ($sorted_groups as $gid => $group)
	{
		//load the measure properly to get the node data from drupal core db, 
		//rather than from queries used in pi_sort_measures_get_options()
		// if it doesn't exist and more then don't add it to the form!
		$group_node = node_load($gid);
		if($group_node)
		{
			// Set default group to none, if it's not specified already
			// if it's not specified '$group' will be an int (nid) rather than an array from the db			
			$group_type = 'none';
			if(is_array($group))
			{
				if(array_key_exists('group_type', $group))
				{
					$group_type = $group['group_type'];
				}
			}
			
			$group_name = l($group_node->title, 'node/' . $gid . '/edit', array('query' => drupal_get_destination()));
			
			//@todo change to some kind of theme function
			$users = pi_get_members_of_group($gid);
			$user_names = array();
			foreach($users as $user)
			{
				$user_names[] = l($user->name, '/user/' . $user->uid);
			}
			$group_description = '<div>' . $group_node->og_description . '<br />' 
								 . implode(', ', $user_names) . '</div>';

								 
			$tree[$group_type][$gid]['title'] = array(
				'#value' => $group_name,
			);
			// put group names into another array to allow them to be sorted by name
			// after all of them have been put into the form
			$group_names[$group_type][$gid] = $group_node->title;
			
			$tree[$group_type][$gid]['description'] = array(
				'#value' => $group_description,
			);
			
			$tree[$group_type][$gid]['region'] = array(
				'#type'          => 'select',
				'#default_value' => $group_type,
				'#options' => array('none' => 'none', 'group' => 'group', 'class' => 'class', 'multiclass'=>'multiclass'),
			);
			
			//Weight isn't saved in the db but only used for the 
			$tree[$group_type][$gid]['weight'] = array(
				'#type'          => 'weight',
				'#delta'         => count($sorted_groups),
				'#default_value' => $group['weight'],
			);
		}
	}
	
	// Sort the group arrays by name and then reset the weight values accordingly...
	$new_weight_count = 0;
	foreach($group_order as $region)
	{
		$names = &$group_names[$region];
		//check the region is populated...
		if(is_array($names))
		{
			asort($names);
			//save the form elements before rearranging
			$saved_group_form_elements = $tree[$region];
			// clear the region array for the grouped form elements for this type
			$tree[$region] = array();
			//rebuild the region one by one,
			foreach(array_keys($names) as $gid)
			{
				$tree[$region][$gid] = $saved_group_form_elements[$gid];
				$tree[$region][$gid]['weight']['#default_value'] = $new_weight_count;
				$new_weight_count++;
			}
		}

	}
	
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Save'),
	);
	return $form;
}



function theme_pi_authoring_admin_groups_form($form) {	
	$output = '';
		
	// Region-sort form header
	$header = array(	array( 'theading' => t('Group'), 'class' => '' ),
						array( 'theading' => t('Description'), 'class' => '' ),
						array( 'theading' => t('Group Type'), 'class' => 'sort_theading_region' ),
						array( 'theading' => t('Weight'), 'class' => 'sort_theading_weight' )
	);

	// Create the region and the rows for the region sort form
	$tree = &$form['groups_list'];
	
	$rows = array();		
	foreach (element_children($tree) as $region)
	{
		foreach (element_children($tree[$region]) as $key)
		{
			$tree[$region][$key]['weight']['#attributes']['class'] = 'sort_list-weight sort_list-weight-'. $region;
			$tree[$region][$key]['region']['#attributes']['class'] = 'sort_list-region-select sort_list-region-'. $region;

			$rows[$region][$key]['title'] = drupal_render($tree[$region][$key]['title']);
			$rows[$region][$key]['description'] = drupal_render($tree[$region][$key]['description']);
			$rows[$region][$key]['region'] = drupal_render($tree[$region][$key]['region']);
			$rows[$region][$key]['weight'] = drupal_render($tree[$region][$key]['weight']);
		}
	}
	
	if(count($rows) == 0)
	{
		drupal_set_message(t("No groups available to define; use the organic groups module to create them."));
	}
	else
	{
		$regions = array(
			'multiclass' => array('title'=>'Multi-class groups', 'description' => t('A group of classes.'), 'empty' => 'No multiclass groups defined.'),
			'class' => array('title'=>t('Classes'), 'description' => t('A class group.'), 'empty' => 'No classes defined.'),
			'group' => array('title'=>t('Groups'), 'description' => t('A group of students within a class.'), 'empty' => 'No in-class groups defined.'),
			'none' => array('title'=>t('Not used'), 'description' => t("These groups aren't used in nQuire."), 'empty' => 'No system groups are left over.'),
		);
		// $output .= theme('table', $header, $rows, array('id' => 'measures_list'));
		$output .= theme('pi_region_sort_table', $header, $rows, $regions, array('id' => 'sort_list'));
		$output .= drupal_render($form);
	}
	return $output;
}


function pi_authoring_admin_groups_form_submit($form, &$form_state) {
	$group_order = array('multiclass', 'class', 'group', 'none');	

	// After submitting all the elements have been changed but the region structure is 
	// still the same, so changing region
	foreach(element_children($form['groups_list']) as $group_type)
	{
		if(is_array($form_state['values']['groups_list'][$group_type]))
		{
			foreach($form_state['values']['groups_list'][$group_type] as $gid => $group)
			{
				$all_groups[$gid] = $group;
			}
		}
	}
	//Update the form in case the user gets bounced back to edit page
	$new_grouped_items = pi_create_grouped_item_list($all_groups, 'region');
	foreach($new_grouped_items as $group => $items)
	{
		$form_state['values']['groups_list'][$group] = $items;
	}
	
	// Save the data...
	foreach($all_groups as $gid => $group)
	{
		// check if a group has more than one definition...
		$query = "SELECT COUNT(gid) FROM {pi_group_type} WHERE gid = %d";
		$result = db_query($query, $gid);
		$group_count = db_result($result);
		// Check if the group type already exists
		if($group_count>0)
		{
			//if the group has an existing definition remove any duplicates, then update it   
			if(($group_count>1) || ($group['region']=='none'))
			{
				if($group['region']=='none')
				{
					$number_to_delete = $group_count;
				}
				else
				{
					$number_to_delete = $group_count-1;
				}
				$query = "DELETE FROM {pi_group_type} WHERE gid = %d  LIMIT %d";
				//pi_debug_message(sprintf($query, $gid, $number_to_delete));
				db_query($query, $gid, $number_to_delete);
			}
			if($group['region']!='none')
			{
				$query = "UPDATE {pi_group_type} SET type = '%s' WHERE gid = %d";
				//pi_debug_message(sprintf($query, $group['region'], $gid));
				db_query($query, $group['region'], $gid);
			}
		}
		else
		{
			if($group['region']!='none')
			{
				$query = "INSERT INTO {pi_group_type} (gid, type) VALUES (%d, '%s')";
				//pi_debug_message(sprintf($query, $gid, $group['region']));
				db_query($query, $gid, $group['region']);
			}
		}
	}
	drupal_set_message(t('When you have set up the group types, go to the ') . l('group heirarchy page', '/admin/nquire/group_heirarchy') . t(' to link groups and classes.'));
}


?>
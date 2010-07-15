<?php
/** @file Functions and templates for inquiry group definitions
 * 
 */


/** View function to assign groups to an inquiry
 * @return string
 */
function pi_authoring_admin_groups() {
	$content = '';
/*
	$content .= "<p>You now need to use the Drupal Admin ";
	$content .= "pages to create you Organic Groups and to add your users to these Organic Group";
	$content .= " Below is a list of Organic Groups and members</p><br>";
	
	//current og groups
	$groups = pi_get_all_groups();
	
	$header = array(t('OG Group Name'), t('Description and Members'), t('Type'));
	$rows = array();
	foreach($groups as $nid => $group)
	{
		$users = pi_get_members_of_group($nid);
		$user_names = array();
		foreach($users as $user)
		{
			$user_names[] = l($user->name, '/user/' . $user->uid);
		}
		$name = l($group->title, 'node/' . $nid . '/edit', array('query' => drupal_get_destination()));
		//@todo change to some kind of theme function		
		$row = array($name, '<div>' . $group->og_description . '</div>' .
									'<div>' . implode(', ', $user_names) . '</div>',
					$group->type);
		$rows[] = $row;
	}
	
	$content .= theme('table', $header, $rows);*/
	$content .= drupal_get_form('pi_authoring_admin_groups_form', $inquiry);	
	return $content;
}


function pi_authoring_admin_groups_form($form_state, $inquiry)
{
	pi_debug_message("form");
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


	// Get the list of available measures
	$all_groups = pi_get_all_group_nids();
	
    //pi_debug_message($defined_groups_unsorted);
	
    $sorted_groups = pi_merge_content_item_lists($defined_groups, $all_groups);

	// set $tree to point to the above $form['measures_list'] object
	$tree = &$form['groups_list'];

	foreach ($sorted_groups as $gid => $group)
	{
		//load the measure properly to get the node data from drupal core db, 
		//rather than from queries used in pi_sort_measures_get_options()
		// if it doesn't exist and more then don't add it to the form!
		$group_node = node_load($gid);
		if($group)
		{
			$group_type = $group['group_type'];
			$group_name = l($group->title, 'node/' . $nid . '/edit', array('query' => drupal_get_destination()));
			
			//@todo change to some kind of theme function
			$users = pi_get_members_of_group($nid);
			$user_names = array();
			foreach($users as $user)
			{
				$user_names[] = l($user->name, '/user/' . $user->uid);
			}
			$group_description = '<div>' . $group_node->og_description . '</div>' .
									'<div>' . implode(', ', $user_names) . '</div>';
			
			$tree[$group_type][$gid]['title'] = array(
				'#value' => $group_name,
			);
			
			$tree[$group_type][$gid]['description'] = array(
				'#value' => $group_description,
			);
	
			$tree[$group_type][$gid]['weight'] = array(
				'#type'          => 'weight',
				'#delta'         => count($sorted_measures),
				'#default_value' => $group['weight'],
			);
			
			$tree[$group_type][$gid]['group_type'] = array(
				'#type'          => 'select',
				'#default_value' => $group_type,
				'#options' => array('none' => 'none', 'group' => 'group', 'class' => 'class', 'multiclass'=>'multiclass'),
			);
		}
	}
	return $form;
}



function theme_pi_authoring_admin_groups_form($form) {	
	pi_debug_message("theme");
	$form['#prefix'] .= " Theme";	
	return drupal_render($form);
}

?>
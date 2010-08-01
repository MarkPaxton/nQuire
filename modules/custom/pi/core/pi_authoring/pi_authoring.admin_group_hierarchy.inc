<?php
/**
 *  @file PI authoring functions - hierarchy editing pages
 */

function pi_authoring_admin_group_hierarchy() {
	$content = '<p>' . t('Use this page to assign your groups into their respective parents: e.g. all the Year 10 classes into the Year 10 group.  ' .
	"If there are no groups listed, you need to create some using the Groups menu item, then categorise them on the nQurire groups page.</p>");
	$content .= drupal_get_form('pi_authoring_admin_group_hierarchy_form');	
	return $content;
}

function pi_authoring_admin_group_hierarchy_form($form_state)
{
	$form = array();
	$form['#prefix'] = '';
	
	// Get all the groups that were defined...
	$defined_groups = pi_get_defined_group_nids();
	$groups_by_type = array();
	//Now organise them into the groups based on hierarchy
	foreach($defined_groups as $group_type => $groups)
	{
		//pi_debug_message($groups);
		$groups_by_type[$group_type] = $groups;								
		foreach($groups as $gid => $group)
		{
			$group_hierarchy_data = pi_get_group_hierarchy($gid);
			$groups_by_type[$group_type][$gid] = $group_hierarchy_data;
			//pi_debug_message($group_hierarchy_data);
		}
	}
	$output='';
	foreach($groups_by_type['group'] as $gid => $group_heirarchy)
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

function theme_pi_authoring_admin_group_hierarchy_form($form)
{
	$output  ='';
	$output .= drupal_render($form);
	return $output;
}
?>
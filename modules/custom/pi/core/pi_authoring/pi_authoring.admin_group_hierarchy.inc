<?php
/**
 *  @file PI authoring functions - hierarchy editing pages
 */

function pi_authoring_admin_group_hierarchy_view() {
	$output = '<p>' . t('Use this page to assign your groups into their respective parents: e.g. all the Year 10 classes into the Year 10 group.  ' .
	"If there are no groups listed, you need to create some using the Groups menu item, then categorise them on the nQurire groups page.</p>");
	// Get all the groups that were defined...
	// Get all the groups that were defined by hierarchy
	$defined_groups = pi_get_all_group_hierarchy_and_types();
	
	//now organise them into ones with parents and no parents
	// and by type
	$orphans = array();
	$groups_by_type = array();
	foreach($defined_groups as $gid => $group)
	{
		if($group['type']==NULL)
		{
			$groups_type = 'none';
		}
		else
		{
			$groups_type = $group['type'];
		}
		$groups_by_type[$groups_type][] = $gid;								
		
		if($group['parent_id'] == NULL)
		{
			$orphans[$groups_type][] = $gid; 
		}
	}
	//pi_debug_message($groups_by_type['class']['119']);
	
	// First get every orphan group for the top level groupings
	$group_order = array(
		'multiclass'=>'Multiclass groups', 
		'class'=>'Class groups with no parent', 
		'group'=>'Groups with no class', 
		'none'=>'Uncategroised groups');

	foreach($group_order as $group_type_to_list => $group_description)	
	{
		foreach($orphans[$group_type_to_list] as $gid)
		{
			//Check that the group hasn't been deleted before processing it:
			$group = node_load($gid);
			if($group)
			{
				$group_title = $group->title;
				//Generate a list of members of the group (if it has children)
				$orphan_group_members = array();
				if(count($defined_groups[$gid]['children']) > 0)
				{		
					foreach($defined_groups[$gid]['children'] as $child_gid)
					{
						if($defined_groups[$child_gid]!='group')
						{
							$class_group_rows = pi_authoring_generate_class_group_members_rows($child_gid, $defined_groups[$child_gid]);
							$orphan_group_members[] = theme_item_list($class_group_rows['rows'], $class_group_rows['title']);
						}
						else
						{
							$g = load_node($child_gid);
							$orphan_group_members[] = $g->title;
						}
					}					
				}
				else
				{
					$orphan_group_members[] = t("None");
				}
			}
			else
			{
				drupal_set_message("Can't find data for group $gid - it may have been deleted");
			}
		}
		$output .= theme_item_list($orphan_group_members, $group_description);
	}
	//	$content .= drupal_get_form('pi_authoring_admin_group_hierarchy_form');	
	return $output;
}

function pi_authoring_admin_group_hierarchy_form($form_state)
{
	$form = array();
	$form['#prefix'] = '';
	
	// Get all the groups that were defined by heirarchy
	$defined_groups = pi_get_all_group_heararchy_and_types();
	
	//now organise them into ones with parents and no parents
	// and types
	$orphans = array();
	$groups_by_type = array();
	foreach($defined_groups as $gid => $group)
	{
		//pi_debug_message($groups);
		if($group['type']!=NULL)
		{
			$groups_by_type['none'][] = $gid;
		}
		else
		{
			$groups_by_type[$group['type']][] = $gid;								
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


function pi_authoring_generate_class_group_members_rows($class_gid, $class_hierarchy)
{
	// Check the group exists as a node and that it's got a defined type
	// and handle it if not.
	$class = node_load($class_gid);
	if($class != NULL)
	{
		$class_group_title = check_plain($class->title);
	}
	else
	{
		$class_group_title = "Unknown";
		drupal_set_message(t("The class with id $class_gid is referred to but does not exist."));
	}

	if($class_hierarchy['type']==NULL)
	{
		$class_group_type = 'not set';
	}
	else
	{
		$class_group_type = $class_hierarchy['type'];
	} 
	$class_group_title .= check_plain(' (' . ucfirst($class_group_type) . ')');
	
	
	// Check the class for sub groups and generate the list of class members:
	$class_members = array();
	foreach($class_hierarchy['children'] as $subgroup_gid)
	{
		$subgroup = node_load($subgroup_gid);
		$subgroup_group_type = pi_get_type_of_group($subgroup_gid);
		$subgroup_group_title = check_plain($subgroup->title . ' (' . ucfirst($subgroup_group_type) . ')');
		if($subgroup_group_type != 'group')
		{
			drupal_set_message("Group " . $subgroup_group_title . " is set as a member of " . $child_group_title . " and is not set as a group.");
		}
		$class_members[] = $subgroup_group_title;
	}
	
	if((count($class_group_children)==0) && ($class_hierarchy['type']!='group'))
	{
		$class_members[] = t("No sub-groups");
	}
	else
	{
		$class_members[] = $class_group_title;
		$class_group_title = null;
	}
	return array('title'=> $class_group_title, 'rows'=>$class_members);
} 

function theme_pi_authoring_admin_group_hierarchy_form($form)
{
	$output  ='';
	$output .= drupal_render($form);
	return $output;
}


?>
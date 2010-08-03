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
	
	$group_order = array(
		'multiclass'=>'Multiclass groups', 
		'class'=>'Classes not assigned to a multiclass', 
		'group'=>'Groups with no class');
	foreach(array_keys($group_order) as $group_type)
	{
		$orphans[$group_type] = array();
	}
	
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
		
		if($group['parent'] == NULL)
		{
			$orphans[$groups_type][] = $gid;			
		}
	}
	
	// First get every orphan group for the top level groupings
	$top_level_group_rows = array();
	foreach($group_order as $group_type_to_list => $group_description)	
	{
		$orphan_group_members = array();		
		if(count($orphans[$group_type_to_list])>0)
		{
			// Pre load all the node names for each orhpan group
			$node_names = pi_authoring_get_names_from_nids($orphans[$group_type_to_list]);
			//Now list orphan each group members or name as needed
			pi_debug_message($group_type_to_list);
			pi_debug_message($orphans[$group_type_to_list]);
			$child_data = pi_authoring_generate_child_list($orphans[$group_type_to_list], $defined_groups);
			pi_debug_message($child_data);
			if(is_array($child_data))
			{
				foreach($child_data as $child_gid => $child_row)
				{
					// get the name or set default
					if($node_names[$child_gid]!=NULL)
					{
						$child_group_name = $node_names[$child_gid] . ' (' . ucfirst($group_type_to_list) . ')';;
					}
					else
					{
						$child_group_name = t('Unknown name') . ' (' . ucfirst($group_type_to_list) . ')';;
					}

					pi_debug_message($child_gid . " - " . $child_group_name);
					
					// Now work out the members of the orphan groups
					if(is_array($child_row))
					{
						pi_debug_message($child_row);
						$orphan_group_members[] = theme_item_list($child_row, $child_group_name);
					}
					else
					{
						$orphan_group_members[] = $child_group_name;
					}
				}
			}
			else
			{
				$orphan_group_members[] = $child_data;
			}
		}
		else
		{
			$orphan_group_members[] = t("None");
		}
		$output .= theme_item_list($orphan_group_members, $group_description);
	}
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
 * Create a list of groups and subgroups using the group heirarchy in $defined groups
 * and the names of the nodes - groups with children will be recursively traversed
 * @param arrauy(uint) $children array of nids of child nodes
 * @param array() $defined_groups array of group hierarchy data indexed by nid.
 * @return array(string, array()) array containing child node name (or unknown if null) or an array
 * containing node names/child node arrays of the child nodes
 */
function pi_authoring_generate_child_list($children, &$defined_groups)
{
	$child_list = array();
	$names = pi_authoring_get_names_from_nids($children);
		
	foreach($children as $nid)
	{
		$group_type = $defined_groups[$nid]['type'];
		if($group_type==NULL)
		{
			$group_type=t('None');
		}
		if(count($defined_groups[$nid]['children'])>0)
		{
			$child_list[$nid] = pi_authoring_generate_child_list($defined_groups[$nid]['children'], $defined_groups);
		}
		else
		{
			if($names[$nid]!=NULL)
			{
				$child_list[$nid] = $names[$nid] . ' (' . ucfirst($group_type) . ')';
			}
			else
			{
				$child_list[$nid] = t('Unknown') . ' (' . ucfirst($group_type) . ')';
			}
		}
	}
	return $child_list;
}

function pi_authoring_render_group_list($group_list)
{
	if(is_array($group_list))
	{
		
		return pi_authoring_render_group_list($group_list);
	}
	else
	{
		return $group_list;
	}
}


function theme_pi_authoring_admin_group_hierarchy_form($form)
{
	$output  ='';
	$output .= drupal_render($form);
	return $output;
}


?>
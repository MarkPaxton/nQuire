<?php
/** @file Functions and templates for inquiry group definitions
 * 
 */


/** View function to assign groups to an inquiry
 * @return string
 */
function pi_authoring_inquiry_groups() {
	$content = '';

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
		$row = array($name, '<div>' . $group->og_description . '</div>' .
									'<div>' . implode(', ', $user_names) . '</div>',
					$group->type);
		$rows[] = $row;
	}
	
	$content .= theme('table', $header, $rows);
	
	return $content;
}

?>
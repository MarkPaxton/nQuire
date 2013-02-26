<?php
// $Id: pi.activity.inc, 2010/05/17 MCP $

/**
 * @file Provides view functions for pages under the url activity/*
 *
 */

/**
 * Generate the page for a pi activity, provides a custom header and footer for the content
 *  and also works out permissions and displayed the view for the correct activity function
 *  (add/view/edit) for the node/node_type the activity references
 *  If the user is the admin user, then redirect to the activity content admin page instead as admin
 *  can't create Activity Content (due to being in all og groups)
 * @param uint $activity_id The id of the pi_activity to work with
 * @param $arg_node_function The function to perform on the activity
 */
function pi_activity_content_view($activity, $arg_node_function)
{
	global $user;
	$activity_inquiry_id = pi_activity_get_inquiry_nid($activity->nid);
	$output = '';
	//If this is admin user, show activity content admin page instead
	if($user->uid==1)
	{
		drupal_set_message(t("Showing Activity Content as you are logged in as the Administrator user who cannot create Activity Content."), 'status');
		// Override any destination parameters since this breaks drupal_goto for this case
		unset($_REQUEST['destination']);
		unset($_GET['destination']);
		drupal_goto('activity/' . $activity->nid . '/admin/content');
	}
	else
	{
		// Check that the node function given in the URL is allowed by comparing it with the
		//   node function in the database for this inquiry and user
		//$node_function = pi_activity_get_node_function($activity->nid, $user->uid);
		// this is now handled with the access callback pi_activity_item_access
    
    /* FIX (eloy) */
    $output .= '<p>' . check_plain($activity->body) . '</p>';
    
		switch($arg_node_function)
		{
			case 'view':
				//get nid
				$nid = pi_activity_get_activity_content_nid_for_activity_and_user($activity->nid, $user->uid);
        
        if ($nid == NULL) {
          $nid = pi_activity_create_content_on_first_visit($activity, $user->uid);
        }
        
				if($nid!=NULL)
				{
					$loaded_node = node_load($nid);
					if($loaded_node==false)
					{
						drupal_not_found();
					}
					else
					{
						//build page
						if(!function_exists("node_view"))
						{
							include_once(drupal_get_path('module', 'node') . '/node.pages.inc');
						}
            $node_view = node_view($loaded_node, FALSE, TRUE, TRUE);
						$output .= $node_view;
					}
				}
				else
				{
          
					//drupal_set_message(t("Can't find the content this activity."), 'error');

					$activity_link = 'activity/' . $activity->nid . '/edit';
					$destination = drupal_get_destination();
					unset($_REQUEST['destination']);
					unset($_GET['destination']);
					drupal_goto($activity_link, 'destination=activity/' . $activity->nid . '/edit');
				}
				break;
			case 'add':
				// Add can have two outcomes, one is the actual add activity of this activity
				// The other is if the activity node exists, but its acts as a shortcut to another
				// activity (for example a shortcut to add a key question from the sort key question page.)
				// The conditions are determined by:
				// 1) If  "pi_activity_get_node_function" returns that it is in the "add" state, then it is condition one
				//		i.e. performing an add of the activity node_type as normal
				// 2) If "pi_activity_get_node_function" is not add, then this indicates an add shortcut COULD be used
				// 3) If an "Add x" activity of the correct node type exists, then the add function is available to be
				//		called, so forward to that page using drupal_goto().
				$node_function = pi_activity_get_node_function($activity->nid, $user->uid);
				if($node_function == 'add')
				{
					// Normal add activity
					$node_type = $activity->node_type;
					if(!function_exists("node_add"))
					{
						include_once(drupal_get_path('module', 'node') . '/node.pages.inc');
					}
					$output .= pi_activity_actor_audience_info($activity_inquiry_id, $activity->nid);
					$output .= node_add($node_type);
				}
				else
				{
					// Possible add shortcut, check if there's a valid shortcut target
					$add_activity_id = pi_activity_get_add_activity_for_sort_activity($activity_inquiry_id, $activity->nid, $user->uid);
					if($add_activity_id !== NULL)
					{
						$activity_link = 'activity/' . $add_activity_id . '/add';
						$destination = drupal_get_destination();
						unset($_REQUEST['destination']);
						unset($_GET['destination']);
						//drupal_goto($activity_link, 'destination=activity/' . $activity->nid . '/view');
						return '';
					}

				}
				break;
			case 'edit':
				// Edit existing content or create new
				$nid = pi_activity_get_activity_content_nid_for_activity_and_user($activity->nid, $user->uid);
				
				if(!function_exists("node_page_edit"))
				{
					include_once(drupal_get_path('module', 'node') . '/node.pages.inc');
				}
					
				$output .= pi_activity_actor_audience_info($activity_inquiry_id, $activity->nid);
				if($nid==NULL)
				{
				  $loaded_node = new stdClass;
				  $loaded_node->uid = $user->uid; 
				  $loaded_node->name = (isset($user->name) ? $user->name : ''); 
			    $loaded_node->type = $activity->node_type;
				  $loaded_node->language = '';
				  $output .= node_page_edit($loaded_node);
				}
				else
				{
					$loaded_node = node_load($nid);
					if(pi_check_is_node_of_type($loaded_node, $activity->node_type))
					{
						$output .= node_page_edit($loaded_node);
					}
					else
					{
						drupal_set_message(t('Error, Activity content is the wrong type or does not exist', 'error'));
					}
				}
				break;
		}
		drupal_set_title(check_plain($activity->title));

	}
	return $output;
}

/**
 * Generate the "To be done by me, will be seen by my group" type text for a pi_activity
 * @param $inquiry_id the id of the inquiry the activity belongs to
 * @param $activity_id the id of the activity to generate the text for
 */
function pi_activity_actor_audience_info($inquiry_id, $activity_id) {
	global $user;
	$output = "";

	if ($activity_id && $inquiry_id)
	{
		$loaded_activity = node_load($activity_id);
		$audience = $loaded_activity->audience;
		$loaded_activity_actor = $loaded_activity->actor;
		$activity_gid = $loaded_activity_actor;
		$activity_gid_type = pi_get_type_of_group($gid);
			
		$seen_by = $audience;
		if ($audience == 'individual')
		{
			$done_by = 'individual';
		}
		elseif ($activity_gid_type == 'class' && $audience == 'class')
		{
			$done_by = 'class';
		}
		else
		{
			$done_by = 'group';
		}

		$output .= '<div class="submitted">';
		$output .= t('Being done by ') . pi_actor_audience_translate($done_by) . '. ';
		$output .= t('Will be seen by ') . pi_actor_audience_translate($seen_by) . '. ';
		$output .= "</div>\n";
	}
	return $output;
}

/**
 * Turns "PI language" text individual/group/class into friendly text me/my group/my class
 * @param string $actor either individual/group/class
 * @return a string turning actor into friendly text, or $actor if it's not possible.
 */
function pi_actor_audience_translate($actor) {
	$translation = array();
	$translation['individual'] = t('me');
	$translation['group'] =  t('my group');
	$translation['class'] = t('my class');
	if(array_key_exists($actor, $translation))
	{
		return $translation[$actor];
	}
	else
	{
		return t($actor);
	}
}


/* FIX (eloy) This is needed for the spreadsheet activity */

function pi_activity_create_content_on_first_visit($activity, $uid) {
  
  if ($activity->activity_type === 'pi_data_spreadsheet') {
    $node = new stdClass();
    $node->uid = $uid;
    $node->type = 'pi_data_spreadsheet';
    $node->body = '';
    node_save($node);
    return $node->nid;
  }
  
  return NULL;
}

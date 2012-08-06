<?php
/** @file This file contains all the functions that display the phase level view.
 * This is the page where the activities will be listed
 */


/**
 * Produces the view of the activities, etc within a particular phase in an inquiry
 * @param node $phase A node of type pi_phase
 * @return String HTML output of the phase
 */
function pi_inquiry_phase_view($phase)
{
  global $user;

  $output = "";  
  
  drupal_set_title($phase->title);
  $output .= $phase->body;

  $activities = pi_get_activities_for_phase_and_user($phase->nid, $user->uid);
  $hidden_created_activity_types = get_hidden_summary_activities($phase->inquiry_nid);

  if (count($activities)>0)
  {
    $list = array();
    $active_activity_flag = 0;
    foreach($activities as $activity_nid)
    {
      $activity = node_load($activity_nid);
      //print_r( $activity );
      //echo "<br/>" . $activity->node_type;
       
      // only display activities that are not in the hidden list
      //if(!(in_array($activity->type, $hidden_created_activity_types)))
      if(!(in_array($activity->node_type, $hidden_created_activity_types)) )
      {
        $nid = pi_activity_get_activity_content_nid_for_activity_and_user($activity->nid, $user->uid);
        $activity_content_node = node_load($nid);
        // If no content node is associated, then check for child nodes... 
        if(!is_object($activity_content_node))
        {
          $activity_content_node = $activity;
          //When it does generate nodes, check what type those nodes are and get the nids of the content
          // of that type in the activity's phase.
          $generated_node_type = pi_activity_get_sorted_node_type($activity->node_type);
          if(!is_null($generated_node_type))
          {
          	$new_node = new stdClass();
            $new_node->type = $activity->node_type;
            $new_node->uid = $user->uid;
            $new_node->published=true;
            node_save($new_node);
            $activity_node = array('nid'=> $new_node->nid, 'activity_id'=>$activity->nid);
            drupal_write_record('pi_activity_node', $activity_node);
            $activity_content_node=$new_node;
          }
        }
        

        $loaded_activity_status = pi_activity_get_activity_status_for_user($activity->nid, $user->uid);
				if(count($loaded_activity_status)==NULL) {
					$loaded_activity_status['visible_status'] = $activity->visible_status;
					$loaded_activity_status['node_function'] = $activity->node_function;
				}

        // (By default) always open an existing node in view
        // if node_function for link is edit or view then set node function to view
        $link_node_function = $loaded_activity_status['node_function'];
        if(($link_node_function == 'edit') || ($link_node_function == 'view'))
          $link_node_function = 'view';

        $activity_link = pi_activity_build_link($activity_nid, $user->uid);
        if (($active_activity_flag == 0) && $activity)
          $active_activity_flag = 1;

        $activity_desc = "";
        $activity_desc .= pi_activity_teaser($activity_content_node, $activity, $link_node_function);

        $list[] = array('data' => $activity_link . $activity_desc, 'class' => 'activity');
      }
      else
        drupal_set_message( t( 'Activity %type is set to be hidden', array( '%type' => $activity->type ) ) );
    }

    $title = NULL;
    $type = 'ul';
    $attributes = array('class' => 'activity');

    $output .= theme_item_list($list, $title, $type, $attributes);

    if ($active_activity_flag == 0)
    {
      drupal_set_message(t('None of the above activities can be accessed at this stage.'));
    }
  }
  else
  {
    drupal_set_message(t('No activities are available in this phase at this stage.'));
  }
  return $output;
}


?>


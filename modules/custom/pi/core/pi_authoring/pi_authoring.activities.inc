<?php

function pi_authoring_activities_edit( $phase )
{
	global $user;
	
	//print_r( $phase );
	
	$output = '';
	
	//set the page title to be the inquiry name
	drupal_set_title("Editing " .  check_plain($phase->title));
	//$output .= theme('pi_authoring_inquiry_phases', $phase->nid);	
	$output .= theme('pi_authoring_inquiry_activities', $phase->inquiry_nid);
	
	$activity_nids = pi_get_activities_for_phase_and_user($phase->nid, $user->uid);
	
    $rows = array();
	if(!empty($activity_nids))
	{
		$activities = array();
		foreach($activity_nids as $activity_nid)
		{
			$activity = node_load($activity_nid);
			$activities[$activity_nid] = array(
				'title' => $activity->title,
				'body' => $activity->body,
				'weight' => $activity->weight
			);
		}
		$output .= drupal_get_form('pi_authoring_order_activities_form', $activities);
	}
	else
	{
		// Display a warning message to the user as there are no phases yet defined 
		drupal_set_message("No activities added so far in this phase.", 'warning', FALSE);
	}
	
	return $output;
}


function pi_authoring_inquiry_activity_add( $node ) 
{
	//print_r( $node );
	
	//See pi_activity_form_alter
	
	$output = 'pi_authoring_inquiry_activity_add';
	
	if( !function_exists("node_add"))
	{
		include_once(drupal_get_path('module', 'node') . '/node.pages.inc');
	}
	$output .= node_add('pi_activity');
	
	return $output;
}


/**
 * This function creates the form for draggable phase ordering on the edit inquiry page
 * @param $phases array of each of the phases in the inquiry created from db_fetch_object
 * @param db_result_object $inquiry the inquiry these phases belong to used to redirect user back to 
 *  inquiry phases page
 * @return array describing a form in drupal api format
 */
function pi_authoring_order_activities_form(&$form_state, $activities) {
	$form = array();
	$form['#theme'] = 'pi_authoring_order_activities_form';
	$form['#title'] = 'Inquiry Activities';
	
	$form['order_activities_activities'] = array(
		'#tree' => TRUE
	);
	
	$position = 0;
	foreach($activities as $nid => $activity)
	{
		$form['order_activities_activities'][$nid]['title'] =
		 	array(
		 		'#value' => $activity['title']
		 	);
		$form['order_activities_activities'][$nid]['body'] = 
			array(
				'#value' => $activity['body']
			);
		$form['order_activities_activities'][$nid]['weight'] = 
			array(
	            '#type'          => 'weight',
	            '#delta'         => count($activity),
	            '#default_value' => $position
	        );
        
        $position++;
	}
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Save'),
  	);
  	return $form;	
}

/**
 * This handles the data from the inquiry phase ordering form, it does not take the actual order
 * numbers provided by the form, but reorders them from 0..(n-1) so that the position data in the 
 * table is always consistent and not susceptible to errors introduced by bad post data.
 * @param array $form drupal api form data
 * @param array $form_state drupal api submitted form data
 */
function pi_authoring_order_activities_form_submit($form, &$form_state)
{
	$activity_order = array();
	
	//print_r( $form_state );
	
	//put all the phases in an array
	//foreach($form_state['values']['order_phases_phases'] as $inquiry_activity_nid => $new_order)
	foreach($form_state['values']['order_activities_activities'] as $inquiry_activity_nid => $new_order)
	{ 
		$activity_order[ $new_order['weight'] ] = $inquiry_activity_nid;
	}
	
	//sort the elements by the new order
	ksort($activity_order);
	$weight = 1;
	foreach($activity_order as $key => $activity_nid) // => $weight)
	{
		if($activity = node_load($activity_nid))
		{
			$activity->weight = $weight;
			node_save($activity);
			$weight++;
		}
		else
		{
			drupal_set_message( t("Can't find Activity %activity", array( '%activity' => $activity_nid ) ), 'error');
		}
	}
}


/**
 * Theme function for the inquiry phases page - provides the link to create a new phase in the inquiry
 * @param unknown_type $inquiry_id
 */
function theme_pi_authoring_inquiry_activities($inquiry_id)
{
	$output='';
	$link = l('here', "inquiry/$inquiry_id/activity/add");    	
	// Display some boilerplate text about defining activities...
	$output .="<div class='help'>\n";
	$output .= "<p>Each activity in an inquiry is allocated to a phase.  ";
	$output .= "A phase is a conceptual part of the inquiry model, for example, ";
	$output .= " <em>Find My Topic</em>, <em>Data Collection</em> and <em>Reflect</em> can all ";
	$output .= "be phases in an inquiry. ";
	$output .= "Click $link to add a new activity.</p>";
	$output .= "</div>\n"; // #help
	return $output;
}


/**
 * This creates the layout for the order phases form, with draggable elements 
 * @param array $form description of a form based on drupal api
 * @return string html output for the form
 */
function theme_pi_authoring_order_activities_form($form) {
	$output = '';
	$output .= '<div>Drag the arrows up and down to to change the order of the activity and click save to finish.</div>';
	drupal_add_tabledrag('pi-authoring-order-activities-sort', 'order', 'sibling', 'pi-authoring-order-activities-element');
	$header = array(array('data' => '', 'width' => '29'), 'Activity', 'Description', 'Position');

	$rows = array();
	foreach (element_children($form['order_activities_activities']) as $key)
	{
		// Add class to group weight fields for drag and drop.
		$form['order_activities_activities'][$key]['weight']['#attributes']['class'] = 'pi-authoring-order-activities-element';

		$row = array('');
		$row[] = l(drupal_render($form['order_activities_activities'][$key]['title']), "node/$key/edit");
		$row[] = drupal_render($form['order_activities_activities'][$key]['body']);
		$row[] = drupal_render($form['order_activities_activities'][$key]['weight']);

		$rows[] = array('data' => $row, 'class' => 'draggable');
	}
	$output .= theme('table', $header, $rows, array('id' => 'pi-authoring-order-activities-sort'));
	$output .= drupal_render($form);
	return $output;
}

?>
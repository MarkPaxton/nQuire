<?php 
/**
 * @file This file contains the page callback functions and any necessary others for 
 * the items under the menu inquiry/edit - excluding inquiry/view (Default)
 *  which is handled in pi.module
 */


/**
 * This page is the starting point for creating content for an inquiry
 * @param $inquiry
 */
function pi_authoring_inquiry_node_edit($inquiry)
{
	$output = '';
	if( !function_exists("node_page_edit"))
	{
		include_once(drupal_get_path('module', 'node') . '/node.pages.inc');
	}
	$output .= node_page_edit($inquiry);
	// Set the title here to override the title being set in node_page_edit
	drupal_set_title("Editing " .  check_plain($inquiry->title));
	return $output;
}


/**
 * This generates the page to allow the adding of phases and stages to an inquiry
 * @param uint $inquiry_id is a valid inquiry id.  Invalid inquiry_ids generate error message and
 *  redirect back to /piauthoring/.
 */
function pi_authoring_inquiry_phases($inquiry) {
	$output = '';
	
	//set the page title to be the inquiry name
	drupal_set_title(t("Editing @title", array( '@title' => $inquiry->title)));
	$output .= theme('pi_authoring_inquiry_phases', $inquiry->nid);	
	
	// Query for inquiry phases 
	$phase_nids = pi_phase_load_nids($inquiry->nid);

    $rows = array();
	if(!empty($phase_nids))
	{
		$phases = array();
		foreach($phase_nids as $phase_nid)
		{
			$phase = node_load($phase_nid);
			$phases[$phase_nid] = array(
				'title' => $phase->title,
				'body' => $phase->body,
				'weight' => $phase->weight
			);
		}
		$output .= drupal_get_form('pi_authoring_order_phases_form', $phases);
	}
	else
	{
		// Display a warning message to the user as there are no phases yet defined 
		drupal_set_message("No phases added so far in this inquiry.", 'warning', FALSE);
	}
	return $output;
}

/**
 * Theme function for the inquiry phases page - provides the link to create a new phase in the inquiry
 * @param unknown_type $inquiry_id
 */
function theme_pi_authoring_inquiry_phases($inquiry_id)
{
	$output='';
	$link = l('here', "inquiry/$inquiry_id/phase/add");    	
	// Display some boilerplate text about defining inquiries...
	$output .="<div class='help'>\n";
	$output .= "<p>Each activity in an inquiry is allocated to a phase.  ";
	$output .= "A phase is a conceptual part of the inquiry model, for example, ";
	$output .= " <em>Find My Topic</em>, <em>Data Collection</em> and <em>Reflect</em> can all ";
	$output .= "be phases in an inquiry. ";
	$output .= "Click $link to add a new phase.</p>";
	$output .= "</div>\n"; // #help
	return $output;
}

/**
 * This function creates the form for draggable phase ordering on the edit inquiry page
 * @param $phases array of each of the phases in the inquiry created from db_fetch_object
 * @param db_result_object $inquiry the inquiry these phases belong to used to redirect user back to 
 *  inquiry phases page
 * @return array describing a form in drupal api format
 */
function pi_authoring_order_phases_form(&$form_state, $phases) {
	$form = array();
	$form['#theme'] = 'pi_authoring_order_phases_form';
	$form['#title'] = 'Inquiry Phases';
	
	$form['order_phases_phases'] = array(
		'#tree' => TRUE
	);
	
	$position = 0;
	foreach($phases as $nid => $phase)
	{
		$form['order_phases_phases'][$nid]['title'] =
		 	array(
		 		'#value' => $phase['title']
		 	);
		$form['order_phases_phases'][$nid]['body'] = 
			array(
				'#value' => $phase['body']
			);
		$form['order_phases_phases'][$nid]['weight'] = 
			array(
	            '#type'          => 'weight',
	            '#delta'         => count($phases),
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
function pi_authoring_order_phases_form_submit($form, &$form_state)
{
	$phase_order = array();
	
	//put all the phases in an array
	foreach($form_state['values']['order_phases_phases'] as $inquiry_phase_nid => $new_order)
	{ 
		$phase_order[ $new_order['weight'] ] = $inquiry_phase_nid;
	}
	
	//sort the elements by the new order
	ksort($phase_order);
	$weight = 1;
	foreach($phase_order as $key => $phase_nid) // => $weight)
	{
		if($phase = node_load($phase_nid))
		{
			$phase->weight = $weight;
			node_save($phase);
			$weight++;
		}
		else
		{
			drupal_set_message(t("Can't find Phase $phase_nid"), 'error');
		}
	}
}

/**
 * This generates the page to edit the order of stages in an inquiry
 * @param uint $inquiry is a valid inquiry.
 */
function pi_authoring_inquiry_stages($inquiry) {
	// Use this to apply module specific css
	$output = '';
	
	//set the page title to be the inquiry name
	drupal_set_title(t("Editing @title", array('@title' => $inquiry->title )));
	$output .= theme('pi_authoring_inquiry_stages', $inquiry->nid);
	
	// Query for inquiry phases and list them if there are any
	$stage_nids = pi_stage_load_nids($inquiry->nid);
	
	if(!empty($stage_nids))
	{
		// Get the phases names to show what phases each stage has.
		$output .= drupal_get_form('pi_authoring_order_stages_form', $stage_nids, $inquiry->nid);
	}
	else
	{
		// Display a warning message to the user as there are no phases yet defined 
		drupal_set_message("No stages added so far in this inquiry; you need at least one stage in every inquiry.", 'warning', FALSE);
	}
	return $output;
}

/**
 * This generates the page to edit the order of activities in a phase
 * @param uint $phase is a valid phase.
 */
function pi_authoring_phase_activities($phase) {
	// Use this to apply module specific css
	$output = '';
	
	//set the page title to be the inquiry name
	drupal_set_title(t("Editing activities in @phase", array( '@phase' => check_plain($phase->title)) ));
	$output .= theme('pi_authoring_phase_activities', $phase->nid);
	
	// Query for inquiry phases and list them if there are any
	$activity_nids = array_keys(pi_activity_get_activities_of_phase($phase->nid));
	if(!empty($activity_nids))
	{
		// Get the phases names to show what phases each stage has.
		$output .= drupal_get_form('pi_authoring_order_activities_form', $activity_nids, $phase->nid);
	}
	else
	{
		// Display a warning message to the user as there are no phases yet defined 
		drupal_set_message("No activities added so far in this inquiry; you need at least one activity in every inquiry.", 'warning', FALSE);
	}
	
	return $output;
	
}

/**
 * Theme function for the inquiry stages page - provides the link to create a new phase in the inquiry
 * @param uint $inquiry_id
 */
function theme_pi_authoring_inquiry_stages($inquiry_id)
{
	$output='';
	$link = l('here', "inquiry/$inquiry_id/stage/add");    	
	// Display some boilerplate text about defining inquiries...
	$output .="<div class='help'>\n";
	$output .= "<p>An inquiry can organised into things that to be done at certain times. ";
	$output .= "A period of time is called a 'stage'; stages could be called things like ";
	$output .= " <em>Preparation</em>, <em>Fieldwork</em> and <em>Results</em>.  During each ";
	$output .= "stage the included phases will be higlighted on the main inquiry view. ";
	$output .= "To add a stage click $link.</p>";
	$output .= "</div>\n"; // #help
	return $output;
}


/**
 * This function creates the form for draggable stage ordering on the edit inquiry page
 * @param $stages array of each of the stages in the inquiry created from db_fetch_object
 * @param uint $inquiry_nid the inquiry nid te stages belong to.
 * @return array describing a form in drupal api format
 */
function pi_authoring_order_stages_form(&$form_state, $stages, $inquiry_nid) {
	
	$form = array();
	$form['#theme'] = 'pi_authoring_order_stages_form';
	$form['#title'] = 'Inquiry Stages';
	
	$form['order_stages_stages'] = array(
		'#tree' => TRUE
	);
	
	$phase_nids = pi_phase_load_nids($inquiry_nid);
	$phase_titles = array();
	foreach($phase_nids as $phase_nid)
	{
		$phase = node_load($phase_nid);
		$phase_titles[$phase_nid] = check_plain($phase->title);
	}

	$position = 0;
	foreach($stages as $stage_nid)
	{
		$stage = node_load($stage_nid);
		//ob_start();
		//var_dump($stage);
		//drupal_set_message('<pre>' . ob_get_clean() . '</pre>');
		
		$form['order_stages_stages'][$stage->nid]['title'] =
		 	array(
		 		'#value' => check_plain($stage->title)
		 	);
		$form['order_stages_stages'][$stage->nid]['body'] = 
			array(
				'#value' => $stage->body
			);
		$form['order_stages_stages'][$stage->nid]['phases'] = array('#tree'=>TRUE);
		foreach($stage->phases as $phase_nid)
		{
			$form['order_stages_stages'][$stage->nid]['phases'][$phase_nid]['#value'] = $phase_titles[$phase_nid];
		//	$form['order_stages_stages'][$stage->nid]['phases'][$phase_nid] = "Test";
		}
		$form['order_stages_stages'][$stage->nid]['weight'] = 
			array(
	            '#type'          => 'weight',
	            '#delta'         => count($stages),
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
 * This handles the data from the inquiry stage ordering form, it does not take the actual order
 * numbers provided by the form, but reorders them from 0..(n-1) so that the position data in the 
 * table is always consistant and not surceptable to errors introduced by bad post data.
 * @param array $form drupal api form data
 * @param array $form_state drupal api submitted form data
 */
function pi_authoring_order_stages_form_submit($form, &$form_state)
{
	$stage_order = array();
	
	//put all the stages in an array
	foreach($form_state['values']['order_stages_stages'] as $inquiry_stage_nid => $new_order)
	{
		$stage_order[$inquiry_stage_nid] = $new_order; 
	}
	//sort the elements by the new order
	asort($stage_order);
	
	// Now reorder from 0..n based on this order
	$count = 0;
	foreach($stage_order as $stage_nid => $order)
	{
		$stage_order[$stage_nid] = $count++;		
	}

	foreach($stage_order as $stage_nid => $weight)
	{
		if($stage = node_load($stage_nid))
		{
			$stage->weight = $weight;
			node_save($stage);
		}
		else
		{
			drupal_set_message(t("Can't find stage $stage_nid"), 'error');
		}
	}
}


/**
 * This function creates the form for draggable activity ordering on the edit inquiry page
 * @param $activities array of each of the activities in the inquiry created from db_fetch_object
 * @param uint $inquiry_nid the inquiry nid te activities belong to.
 * @return array describing a form in drupal api format
 */
function pi_authoring_order_activities_form(&$form_state, $activities, $phase_nid) {
	
	$form = array();
	$form['#theme'] = 'pi_authoring_order_activities_form';
	$form['#title'] = 'Inquiry activities';
	$form['#phase_nid'] = $phase_nid;
	
	$form['order_activities_activities'] = array(
		'#tree' => TRUE
	);
	
	$activity_nids = pi_activity_get_activities_of_phase($phase_nid);

	$position = 0;
	foreach($activities as $activity_nid)
	{
	  $activity = node_load($activity_nid);
	  // check th eactivity id valid and it's an activity...
	  if($activity!=null)
	  {
	    if($activity->type=='pi_activity')
	    {
          // Get only the parent activities and ignore child ones
	      if($activity->parent_activity==null)
	      {
	        $form['order_activities_activities'][$activity->nid]['title'] =
	        array(
		 		'#value' => check_plain($activity->title)
	        );
	        $form['order_activities_activities'][$activity->nid]['body'] =
	        array(
				'#value' => $activity->body
	        );
	        $form['order_activities_activities'][$activity->nid]['phases'] = array('#tree'=>TRUE);
	        $form['order_activities_activities'][$activity->nid]['#actor'] = $activity->actor;
	        $form['order_activities_activities'][$activity->nid]['#audience'] = $activity->audience;
	        $form['order_activities_activities'][$activity->nid]['weight'] =
	        array(
	            '#type'          => 'weight',
	            '#delta'         => count($activities),
	            '#default_value' => $position
	        );

	        $position++;
	      }
	    }
	    else
	    {
	      drupal_set_message(t('Activity @nid is a @type not an activity.', array('@nid' => $activity_nid, '@type' => $activity->type)), 'error');
	    }
	  }
	  else
	  {
	    drupal_set_message(t('Activity @nid not found.', array('@nid' => $activity_nid)), 'error');
	  }
	}
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Save'),
  	);
  	return $form;	
}

/**
 * This handles the data from the inquiry activity ordering form, it does not take the actual order
 * numbers provided by the form, but reorders them from 0..(n-1) so that the position data in the 
 * table is always consistant and not surceptable to errors introduced by bad post data.
 * @param array $form drupal api form data
 * @param array $form_state drupal api submitted form data
 */
function pi_authoring_order_activities_form_submit($form, &$form_state)
{
	$activity_order = array();
	
	//put all the activities in an array
	foreach($form_state['values']['order_activities_activities'] as $inquiry_activity_nid => $new_order)
	{
		$activity_order[$inquiry_activity_nid] = $new_order; 
	}
	//sort the elements by the new order
	asort($activity_order);
	
	// Now reorder from 0..n based on this order
	$count = 0;
	foreach($activity_order as $activity_nid => $order)
	{
		$activity_order[$activity_nid] = $count++;		
	}

	foreach($activity_order as $activity_nid => $weight)
	{
		if($activity = node_load($activity_nid))
		{
			$activity->weight = $weight;
			node_save($activity);
		}
		else
		{
			drupal_set_message(t("Can't find activity $activity_nid"), 'error');
		}
	}
}




/**
 * This creates the layout for the order inquiries form, with draggable elelemts 
 * @param array $form description of a form based on drupal api
 * @return string html output for the form
 */
function theme_pi_authoring_order_inquiries_form($form) {
	drupal_add_tabledrag('pi-authoring-order-inquiries-sort', 'order', 'sibling', 'pi-authoring-order-inquiries-element');
	$header = array(array('data' => '', 'width' => '29'), 'Inquiry', 'Description', '', 'Position');

	$rows = array();
	foreach (element_children($form['order_inquiries_inquiries']) as $key)
	{
		// Add class to group weight fields for drag and drop.
		$form['order_inquiries_inquiries'][$key]['weight']['#attributes']['class'] = 'pi-authoring-order-inquiries-element';

		$row = array('');
		$row[] = drupal_render($form['order_inquiries_inquiries'][$key]['title']);
		$row[] = drupal_render($form['order_inquiries_inquiries'][$key]['body']);
		$row[] = drupal_render($form['order_inquiries_inquiries'][$key]['delete']);
		$row[] = drupal_render($form['order_inquiries_inquiries'][$key]['weight']);

		$rows[] = array('data' => $row, 'class' => 'draggable');
	}
	$output = theme('table', $header, $rows, array('id' => 'pi-authoring-order-inquiries-sort'));
	$output .= drupal_render($form);
	return $output;
}


/**
 * This creates the layout for the order phases form, with draggable elements 
 * @param array $form description of a form based on drupal api
 * @return string html output for the form
 */
function theme_pi_authoring_order_phases_form($form) {
	$output = '';
	$output .= '<div>Drag the arrows up and down to to change the order of the phases and click save to finish.</div>';
	drupal_add_tabledrag('pi-authoring-order-phases-sort', 'order', 'sibling', 'pi-authoring-order-phases-element');
	$header = array(array('data' => '', 'width' => '29'), 'Phase', 'Description', 'Position');

	$rows = array();
	foreach (element_children($form['order_phases_phases']) as $key)
	{
		// Add class to group weight fields for drag and drop.
		$form['order_phases_phases'][$key]['weight']['#attributes']['class'] = 'pi-authoring-order-phases-element';

		$row = array('');
		$row[] = l(drupal_render($form['order_phases_phases'][$key]['title']), "phase/$key/edit");
		$row[] = drupal_render($form['order_phases_phases'][$key]['body']);
		$row[] = drupal_render($form['order_phases_phases'][$key]['weight']);

		$rows[] = array('data' => $row, 'class' => 'draggable');
	}
	$output .= theme('table', $header, $rows, array('id' => 'pi-authoring-order-phases-sort'));
	$output .= drupal_render($form);
	return $output;
}

/**
 * This creates the layout for the order stages form, with draggable elements 
 * @param array $form description of a form based on drupal api
 * @return string html output for the form
 */
function theme_pi_authoring_order_stages_form($form) {
	$output = '';
	$output .= '<div>Drag the arrows up and down to to change the order of the stages and click save to finish.</div>';

	drupal_add_tabledrag('pi-authoring-order-stages-sort', 'order', 'sibling', 'pi-authoring-order-stages-element');
	$header = array(array('data' => '', 'width' => '29'), 'Stage', 'Description', 'Position');

	$rows = array();
	foreach (element_children($form['order_stages_stages']) as $key)
	{
		// Add class to group weight fields for drag and drop.
		$form['order_stages_stages'][$key]['weight']['#attributes']['class'] = 'pi-authoring-order-stages-element';

		$row = array('');
		$row[] = l(drupal_render($form['order_stages_stages'][$key]['title']), "stage/$key/edit");
		$phase_description_row = '';
		$phase_description_row .= '<div>' . drupal_render($form['order_stages_stages'][$key]['body']) . '</div>';
		if(count($form['order_stages_stages'][$key]['phases'] > 0))
		{
			$phase_description_row .= '<div><strong>' . t('Phases in this stage') . ':</strong></div>';
			$phase_description_row .= '<div>';
			$phase_name_links = array();
			foreach(element_children($form['order_stages_stages'][$key]['phases']) as $phase_nid)
			{
				$phase_name_links[] = '<span>' . l(drupal_render($form['order_stages_stages'][$key]['phases'][$phase_nid]), "phase/$phase_nid/edit") . '</span>';
			}
			$phase_description_row .= implode($phase_name_links, ', ');
			$phase_description_row .= '</div>';
		}
		else
		{
			$phase_description_row .= '<div><strong>' . t('No phases in this stage yet.') . '</strong></div>';
		}
		 
		$row[] = $phase_description_row;
		$row[] = drupal_render($form['order_stages_stages'][$key]['weight']);

		$rows[] = array('data' => $row, 'class' => 'draggable');
	}
	$output = theme('table', $header, $rows, array('id' => 'pi-authoring-order-stages-sort'));
	$output .= drupal_render($form);
	return $output;
}

/**
 * Theme function for the inquiry stages page - provides the link to create a new phase in the inquiry
 * @param uint $inquiry_id
 */
function theme_pi_authoring_phase_activities($phase_id)
{
	$output='';
	$link = l('here', "phase/$phase_id/activity/add");    	
	// Display some boilerplate text about defining inquiries...
	$output .="<div class='help'>\n";
	$output .= "<p>A phase contains activities that describe work to do.  Work can be assigned to individuals or groups, and shared or kept private.  ";
	$output .= "To add an activity click $link.</p>";
	$output .= "</div>\n"; // #help
	return $output;
}


/**
 * This creates the layout for the order activities form, with draggable elements 
 * @param array $form description of a form based on drupal api
 * @return string html output for the form
 */
function theme_pi_authoring_order_activities_form($form) {
	$output = '';
	$output .= '<div>Drag the arrows up and down to to change the order of the activities and click save to finish.</div>';

	drupal_add_tabledrag('pi-authoring-order-activities-sort', 'order', 'sibling', 'pi-authoring-order-activities-element');
	$header = array(array('data' => '', 'width' => '29'), 'Activity', 'Description', 'Position');

	$rows = array();
	foreach (element_children($form['order_activities_activities']) as $key)
	{
		// Add class to group weight fields for drag and drop.
		$form['order_activities_activities'][$key]['weight']['#attributes']['class'] = 'pi-authoring-order-activities-element';

		$row = array('');
		$row[] = l(drupal_render($form['order_activities_activities'][$key]['title']), "phase/" . $form['#phase_nid'] . "/activity/$key/edit");
		$phase_description_row = '';
		$phase_description_row .= '<div>' . drupal_render($form['order_activities_activities'][$key]['body']) . '</div>';
		$phase_description_row .= '<div>' . _pi_activity_explain_actor_and_audience($form['order_activities_activities'][$key]['#actor'], $form['order_activities_activities'][$key]['#audience']) . '</div>';
		$row[] = $phase_description_row;
		$row[] = drupal_render($form['order_activities_activities'][$key]['weight']);

		$rows[] = array('data' => $row, 'class' => 'draggable');
	}
	$output = theme('table', $header, $rows, array('id' => 'pi-authoring-order-activities-sort'));
	$output .= drupal_render($form);
	return $output;
}


?>
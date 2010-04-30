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
	return t($inquiry->title);
}



/**
 * This generates the page to allow the adding of phases and stages to an inquiry
 * @param uint $inquiry_id is a valid inquiry id.  Invalid inquiry_ids generate error message and
 *  redirect back to /piauthoring/.
 */
function pi_authoring_inquiry_phases($inquiry) {
	// Use this to apply module specific css
	$output .= "<div id='pi_authoring'>\n";
	
	//set the page title to be the inquiry name
	drupal_set_title("Editing " .  t($inquiry->title));
	
	// Query for inquiry phases and list them if there are any, otherwise show 
	// placeholder text
	$phase_nids = pi_phase_load_nids($inquiry->nid);

	$output .="<h2>Phases of this inquiry</h2>\n";
	$header = array(
        t("Phase name"),
        t("Description")
    );

    $rows = array();
	if(!empty($phase_nids))
	{
		$output .= drupal_get_form('pi_authoring_order_phases_form', $phase_nids);
	}
	else
	{
		// Display a warning message to the user as there are no phases yet defined 
		$link = l("here", 'node/add/pi-phase');    	
		drupal_set_message("No phases added so far in this inquiry, to add phases click $link.", 'warning', FALSE);

		// Also display some boilerplate text about defining inquiries
		$output .="<div class='help'>\n";
		$output .= "<p>Each activity in an inquiry is allocated to a phase.  ";
		$output .= "A phase is a conceptual part of the inquiry model, for example, ";
		$output .= " <em>Find My Topic</em>, <em>Data Collection</em> and <em>Reflect</em> can all ";
		$output .= "be phases in an inquiry.</p>\n";
		$output .= "</div>\n"; // #help
		
	}
	//$output .= drupal_get_form('pi_authoring_inquiry_add_phase_form', $inquiry);
	//global $user;
	//$new_phase_node = array('uid' =>  $user->uid, 'name' => $user->name, 'type' => 'pi_phase');
	//$new_phase_form = drupal_retrieve_form('pi_phase_node_form', $new_phase_form_state, $new_phase_node);
    //drupal_prepare_form('pi_phase_node_form', $new_phase_form, $new_phase_form_state);
    //drupal_redirect_form($new_phase_form, url('nQuire/inquiry/' . $inquiry->nid));
	//$outout .= drupal_render_form('pi_phase_node_form', $new_phase_form);
	$output .= "</div>\n"; // #pi_authoring
	
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
	foreach($phases as $phase_nid)
	{
		$phase = node_load($phase_nid);
		$form['order_phases_phases'][$phase->nid]['title'] =
		 	array(
		 		'#value' => $phase->title
		 	);
		$form['order_phases_phases'][$phase->nid]['body'] = 
			array(
				'#value' => $phase->body
			);
		$form['order_phases_phases'][$phase->nid]['weight'] = 
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
		$phase_order[$inquiry_phase_nid] = $new_order; 
	}
	//sort the elements by the new order
	asort($phase_order);
	
	// Now reorder from 0..n based on this order
	$count = 0;
	foreach($phase_order as $phase_nid => $order)
	{
		$phase_order[$phase_nid] = $count++;		
	}
	
	foreach($phase_order as $phase_nid => $weight)
	{
		if($phase = node_load($phase_nid))
		{
			$phase->weight = $weight;
			node_save($phase);
		}
		else
		{
			drupal_set_error(t("Can't find Phase $phase_nid"), 'error');
		}
	}
}



/**
 * This generates the page to edit the order of stages in an inquiry
 * @param uint $inquiry_id is a valid inquiry id.
 */
function pi_authoring_inquiry_stages($inquiry) {
	// Use this to apply module specific css
	$output .= "<div id='pi_authoring'>\n";
	
	//set the page title to be the inquiry name
	drupal_set_title("Editing " .  t($inquiry->title));
	
	// Query for inquiry phases and list them if there are any, otherwise show 
	// placeholder text
	$stage_nids = pi_stage_load_nids($inquiry->nid);

	$output .="<h2>Stages of this inquiry</h2>\n";
	$header = array(
        t("Stage name"),
        t("Description")
    );

    $rows = array();
	if(!empty($stage_nids))
	{
		$output .= drupal_get_form('pi_authoring_order_stages_form', $stage_nids, $inquiry->nid);
	}
	else
	{
		// Display a warning message to the user as there are no phases yet defined 
		$link = l("here", 'node/add/pi-stage');
		drupal_set_message("No stages added so far in this inquiry. To create a stage click $link.", 'warning', FALSE);

		// Also display some boilerplate text about defining inquiries
		$output .="<div class='help'>\n";
		$output .= "<p>help text</p>\n";
		$output .= "</div>\n"; // #help
		
	}
	$output .= "</div>\n"; // #pi_authoring
	
	return $output;
}




/**
 * This function creates the form for draggable stage ordering on the edit inquiry page
 * @param $stages array of each of the stages in the inquiry created from db_fetch_object
 * @param uint $inquiry_nid the inquiry nid te stages belong to.
 * @return array describing a form in drupal api format
 */
function pi_authoring_order_stages_form(&$form_state, $stages, $inquiry_nid) {
	
	// Get the phases names to show what phases each stage has.
	$phase_nids = pi_phase_load_nids($inquiry_nid);
	$phase_titles = array();
	foreach($phase_nids as $phase_nid)
	{
		$phase = node_load($phase_nid);
		$phase_titles[$phase_nid] = $phase->title;
	}
	
	
	$form = array();
	$form['#theme'] = 'pi_authoring_order_stages_form';
	$form['#title'] = 'Inquiry Stages';
	
	$form['order_stages_stages'] = array(
		'#tree' => TRUE
	);
	
	$position = 0;
	foreach($stages as $stage_nid)
	{
		$stage = node_load($stage_nid);
		$form['order_stages_stages'][$stage->nid]['title'] =
		 	array(
		 		'#value' => $stage->title
		 	);
		$form['order_stages_stages'][$stage->nid]['body'] = 
			array(
				'#value' => $stage->body
			);
		$form['order_stages_stages'][$stage->nid]['phases'] = array('#tree'=>TRUE);
		foreach($stage->stage_phases as $phase_nid)
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
	//firep(var_dump($stage_order), "New order");
	
	// Now reorder from 0..n based on this order
	$count = 0;
	foreach($stage_order as $stage_nid => $order)
	{
		$stage_order[$stage_nid] = $count++;		
	}
	//firep(var_dump($stage_order), "New order");
	foreach($stage_order as $stage_nid => $weight)
	{
		if($stage = node_load($stage_nid))
		{
			$stage->weight = $weight;
			node_save($stage);
		}
		else
		{
			drupal_set_error(t("Can't find stage $stage_nid"), 'error');
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
  drupal_add_tabledrag('pi-authoring-order-phases-sort', 'order', 'sibling', 'pi-authoring-order-phases-element');
  $header = array(array('data' => '', 'width' => '29'), 'Phase', 'Description', 'Position');
  
  $rows = array();
  foreach (element_children($form['order_phases_phases']) as $key)
  {
    // Add class to group weight fields for drag and drop.
    $form['order_phases_phases'][$key]['weight']['#attributes']['class'] = 'pi-authoring-order-phases-element';

    $row = array('');
    $row[] = l(drupal_render($form['order_phases_phases'][$key]['title']), "node/$key/edit");
    $row[] = drupal_render($form['order_phases_phases'][$key]['body']);
    $row[] = drupal_render($form['order_phases_phases'][$key]['weight']);
    
    $rows[] = array('data' => $row, 'class' => 'draggable');
  }
  $output = theme('table', $header, $rows, array('id' => 'pi-authoring-order-phases-sort'));
  $output .= drupal_render($form);
  return $output;
}

/**
 * This creates the layout for the order stages form, with draggable elements 
 * @param array $form description of a form based on drupal api
 * @return string html output for the form
 */
function theme_pi_authoring_order_stages_form($form) {
  drupal_add_tabledrag('pi-authoring-order-stages-sort', 'order', 'sibling', 'pi-authoring-order-stages-element');
  $header = array(array('data' => '', 'width' => '29'), 'Stage', 'Description', 'Position');
  
  $rows = array();
  foreach (element_children($form['order_stages_stages']) as $key)
  {
    // Add class to group weight fields for drag and drop.
    $form['order_stages_stages'][$key]['weight']['#attributes']['class'] = 'pi-authoring-order-stages-element';

    $row = array('');
    $row[] = l(drupal_render($form['order_stages_stages'][$key]['title']), "node/$key/edit");
    $phase_description_row = '';
    $phase_description_row .= '<div>' . drupal_render($form['order_stages_stages'][$key]['body']) . '</div>';
    if(count($form['order_stages_stages'][$key]['phases'] > 0))
    {
    	$phase_description_row .= '<div><strong>Phases in this stage:</strong></div>';    
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
    	$phase_description_row .= '<div><strong>No phases in this stage yet.</strong></div>';    
    } 
   
    $row[] = $phase_description_row;
    $row[] = drupal_render($form['order_stages_stages'][$key]['weight']);
    
    $rows[] = array('data' => $row, 'class' => 'draggable');
  }
  $output = theme('table', $header, $rows, array('id' => 'pi-authoring-order-stages-sort'));
  $output .= drupal_render($form);
  return $output;
}



?>
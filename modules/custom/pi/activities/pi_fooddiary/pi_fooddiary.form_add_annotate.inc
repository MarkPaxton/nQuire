<?php

module_load_include('inc', 'pi_fooddiary', 'pi_fooddiary.form_render_functions');

/**
 * This function is called via javascript in order to dynamically add/remove
 * food information.
 */
function pi_fooddiary_add_annotate_js() 
{	
  	// Build our new form element.
  	$form_elements = _pi_fooddiary_add_js_form_element();
  	drupal_alter('form', $form_elements, array(), 'pi_fooddiary_node_form');

  	// Build the new form.
  	$form_state = array('submitted' => FALSE);
  	$form_build_id = $_POST['form_build_id'];
  	// Add the new element to the stored form. Without adding the element to the
  	// form, Drupal is not aware of this new elements existence and will not
  	// process it. We retreive the cached form, add the element, and resave.
  	if (!$form = form_get_cache($form_build_id, $form_state)) 
  	{
    	exit();
  	}
  	
  	$form['food_annotate'] = $form_elements;
  	
  	$food_choice =  $_POST['food_annotate']['food_choice'];
  	$food_portion = $_POST['food_annotate']['food_portion'] + 1;
  	$selected = $_POST['food_diary']['meal_type'];
	
	$form_state['storage']['food_diary']['meal_type']['#options'] = $selected;
	  	
	if($_POST['op'] === t('Remove choice') )
	{
		$form_state['storage']['food_annotate']['food_portion'][$food_choice] -= $food_portion;
		if( $form_state['storage']['food_annotate']['food_portion'][$food_choice] <= 0)
		{
			unset($form_state['storage']['food_annotate']['food_portion'][$food_choice]);
		}
	}
	else
	{
		$form_state['storage']['food_annotate']['food_portion'][$food_choice] += $food_portion;
	}
	
	//print_r( $form['food_annotate']['food_choice']['#options'] );
	//print_r( $form_state['storage']['food_annotate']['food_portion'] );
	
  	form_set_cache($form_build_id, $form, $form_state);
  	$form += array(	'#post' => $_POST,
    				'#programmed' => FALSE );

  	// Rebuild the form.
  	$form = form_builder('pi_fooddiary_node_form', $form, $form_state);
  	
  	// Render the new output.
  	$new_elements_form = array( $form['food_annotate']['food_choice'],
  								$form['food_annotate']['food_portion'],
  								$form['food_annotate']['add_submit'], 
  								$form['food_annotate']['remove_submit'] );
  	
  	// Prevent duplicate wrappers.
  	unset($new_elements_form['food_annotate']['food_choice']['#prefix'], $new_elements_form['food_annotate']['food_choice']['#suffix']);
  	unset($new_elements_form['food_annotate']['food_portion']['#prefix'], $new_elements_form['food_annotate']['food_portion']['#suffix']);
  	unset($new_elements_form['upload_submit']['#prefix'], $new_elements_form['upload_submit']['#suffix']);
  	unset($new_elements_form['remove_submit']['#prefix'], $new_elements_form['remove_submit']['#suffix']);
  	
  	$new_elements_form['food_annotate']['food_choice']['#prefix']['#attributes']['class'] = empty($new_elements_form) ? 'ahah-new-content' : $new_element_forms['food_annotate']['food_choice']['#prefix']['#attributes']['class'] .' ahah-new-content';
  	$new_elements_form['food_annotate']['food_portion']['#prefix']['#attributes']['class'] = empty($new_elements_form) ? 'ahah-new-content' : $new_element_forms['food_annotate']['food_portion']['#prefix']['#attributes']['class'] .' ahah-new-content';
  	$new_elements_form['food_annotate']['add_submit']['#attributes']['class'] = empty($new_elements_form) ? 'ahah-new-content' : $new_element_forms['food_annotate']['add_submit']['#attributes']['class'] .' ahah-new-content';
  	$new_elements_form['food_annotate']['remove_submit']['#attributes']['class'] = empty($new_elements_form) ? 'ahah-new-content' : $new_element_forms['food_annotate']['remove_submit']['#attributes']['class'] .' ahah-new-content';
  	
  	
  	$output  = '';
  	$output .= pi_fooddiary_render_annotate($form, $form_state);
  	$output .= drupal_render($new_elements_form);

	//return drupal_json(array('status' =>  TRUE, 'data' => $output));
	print drupal_to_js(array('status' => TRUE, 'data' => $output));
  	exit;
}

/**
 * This function generates the form which is dynamically added via JavaScript (AHAH helper)
 */
function _pi_fooddiary_add_js_form_element()
{	
	$form = array(
    	'#tree' => TRUE,
  	);
  	
	$food_choice = 	_pi_fooddiary_get_food_options();

	$food_portion = array( 	t('1'),
							t('2'),
							t('3'),
							t('4'),
							t('5'),
							t('6'),
							t('7'),
							t('8'),
							t('9'),
							t('10')
							 		);
	
	$form['food_choice'] = array(
			'#type' => 'select',
			'#title' => t('Food choice'),
			'#required' => FALSE,
			'#multiple' => FALSE,
		    '#options' => $food_choice,
			'#default_value' => ''
			);
	
	
	$form['food_portion'] = array(
			'#type' => 'select',
			'#title' => t('Food portion'),
			'#required' => FALSE,
			'#multiple' => FALSE,
		    '#options' => $food_portion,
			'#default_value' => ''
			);

  	$form['add_submit'] = array(
   		'#type' => 'submit',
    	'#value' => t('Add choice'),
    	'#submit' => array('node_form_submit_build_node'),//none JS version
    	'#ahah' => array(
      			'event' => 'click',
      			'path' => 'activity/pi_fooddiary_add_annotate/js',
      			'wrapper' => 'add_choice',
      			'method' => 'replace',
      			'effect' => 'fade',
      			'progress' => array(
        		'type' => 'throbber',
      				)     
   	 		)
   	 	);
   	 $form['remove_submit'] = array(
   		'#type' => 'submit',
    	'#value' => t('Remove choice'),
    	'#submit' => array('node_form_submit_build_node'),//none JS version
   	 	'#prefix' => "",
		'#suffix' => '</div>',
    	'#ahah' => array(
      			'event' => 'click',
      			'path' => 'activity/pi_fooddiary_add_annotate/js',
      			'wrapper' => 'add_choice',
      			'method' => 'replace',
      			'effect' => 'fade',
      			'progress' => array(
        		'type' => 'throbber',
      				)     
   	 		)
   	 	);		

  	return $form;
}

?>
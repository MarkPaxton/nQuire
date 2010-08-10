<?php

/**
 * 
 * Enter description here ...
 */
function pi_fooddiary_select_meal_type_js() 
{	
	$photo = NULL;
	//$output = 'Hello World says pi_fooddiary_select_meal_type_js';
	
	//drupal_alter('form', $form_element, array(), 'pi_fooddiary_select_meal_type_js');
  	drupal_alter('form', $form_element, array(), 'pi_fooddiary_node_form');

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
  	
  	$node = node_load($form['nid']['#value']);
  	
  	$options = $form['food_diary']['meal_type']['#options'];
	$selected = $_POST['food_diary']['meal_type'];
	
	
	$meal_type = strtolower($options[$selected]);
	
	$content = $node->content; 
	
	$food_info = $content['food'][$meal_type];
	
	if(substr_count($food_info['image'], 'pi_fooddiary/unknown.png') <= 0)
	{
		$file_info = pathinfo($food_info['image']);
		$photo = $file_info['basename'];
	}
	
	$form_state['storage']['food_diary']['meal_type']['#options'] = $selected;
	$form_state['storage']['food_diary']['date'] = $content['day'];
	
  	$form_element = _pi_fooddiary_select_meal_type_js_form_element( $options[$selected], $photo);
  	
  	$form['food_photo'] = $form_element;
  	
  	form_set_cache($form_build_id, $form, $form_state);
  	$form += array(	'#post' => $_POST,
    				'#programmed' => FALSE );

  	
  	// Rebuild the form.
  	$form = form_builder('pi_fooddiary_node_form', $form, $form_state);

  	// Render the new output.
  	if($photo)
  	{
  		$new_elements_form = array(	$form['food_photo']['photo'] ,
  									$form['food_photo']['upload'],
  									$form['food_photo']['upload_submit'] ,
  									$form['food_photo']['upload_submit_skip'] );
  	}
  	else
  	{
  		$new_elements_form = array(	$form['food_photo']['upload'],
  									$form['food_photo']['upload_submit']	);
  	}
  	
  	// Prevent duplicate wrappers.
  	unset($new_elements_form['upload']['#prefix'], $new_elements_form['upload']['#suffix']);
  	unset($new_elements_form['upload_submit']['#prefix'], $new_elements_form['upload_submit']['#suffix']);
  	unset($new_elements_form['upload_submit_skip']['#prefix'], $new_elements_form['upload_submit_skip']['#suffix']);
  	
  	$new_elements_form['upload']['#attributes']['class'] = empty($new_elements_form) ? 'ahah-new-content' : $new_element_forms['upload']['#attributes']['class'] .' ahah-new-content';
  	$new_elements_form['upload_submit']['#attributes']['class'] = empty($new_elements_form) ? 'ahah-new-content' : $new_element_forms['upload_submit']['#attributes']['class'] .' ahah-new-content';
  	$new_elements_form['upload_submit_skip']['#attributes']['class'] = empty($new_elements_form) ? 'ahah-new-content' : $new_element_forms['upload_submit_skip']['#attributes']['class'] .' ahah-new-content';
  	
  	$output .= theme('status_messages') . drupal_render($new_elements_form);
  	
  	print drupal_to_js(array('status' => TRUE, 'data' => $output));
  	exit;
}

/**
 * 
 * Enter description here ...
 * @param $name
 * @param $photo
 */
function _pi_fooddiary_select_meal_type_js_form_element($name, $photo = NULL) 
{	
	$form = array( '#tree' => TRUE );
	
	$form['upload'] = array(
			'#type' => 'file',
			'#title' => t('Food photo for ' . $name ),
			'#required' => FALSE,
	    	'#size' => 50,
			'#description' => "Upload a foto of the food",
		);
	
	if(!empty($photo))
	{
		$form['photo'] = array(
			'#type' => 'hidden',
			'#title' => t('Food photo'),
			'#required' => FALSE,
			'#disabled' => FALSE,
			'#weight' => -5,
	    	'#value' => $photo
		);
		
		$form['upload_submit_skip'] = array(
   				 	'#type' => 'submit',
    				'#value' => t('Skip upload and use existing'),
					'#weight' => -4,
    				'#submit' => array('node_form_submit_build_node'),//none JS version
    				'#ahah' => array(
      					'event' => 'click',
      					'path' => 'activity/pi_fooddiary_upload_photo/js',
      					'wrapper' => 'add_choice',
      					'method' => 'replace',
      					'effect' => 'fade',
      					'progress' => array(
        							'type' => 'bar',
        							'message' => t('Loading...')
      							)     
   	 					)
   	 				);
	}
	
	$form['upload_submit'] = array(
   			 	'#type' => 'submit',
    			'#value' => t('Upload photo'),
				'#weight' => 5,
    			'#submit' => array('node_form_submit_build_node'),//none JS version
    			'#ahah' => array(
      				'event' => 'click',
      				'path' => 'activity/pi_fooddiary_upload_photo/js',
      				//'path' => 'upload/js',
      				'wrapper' => 'add_choice',
      				'method' => 'replace',
      				'effect' => 'fade'    
   	 				)
   	 			);
   	 			
   	 return $form;	
}

?>
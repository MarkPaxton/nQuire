<?php

module_load_include('inc', 'pi_fooddiary', 'pi_fooddiary.form_render_functions');

define('FOODDIARY_FILE_PATH', 'fooddiary');

/**
 * 
 * Enter description here ...
 */
function pi_fooddiary_upload_photo_js()
{
	//print_r($_POST);
	$submit = $_POST['op'];

	_pi_fooddiary_check_upload_directory();
	$path = file_directory_path() . '/' . FOODDIARY_FILE_PATH;
	$file = file_save_upload('food_photo', array(), $path, FILE_EXISTS_REPLACE);

	if( $file )
	{
		$dot = strrpos($file->filename, '.');

		if($dot===FALSE)
		{
			$filename = $file->filename;
			$extension = '';
		}
		else
		{
			$filename = substr($file->filename, 0, $dot);
			$extension = substr($file->filename, $dot);
		}

		$thumb1 = $path . '/' . $filename . '_thumb1' . $extension;
		$thumb2 = $path . '/' . $filename . '_thumb2' . $extension;

		image_scale($file->filepath, $thumb1, 100, 100);
		image_scale($file->filepath, $thumb2, 50, 50);
		image_scale($file->filepath, $file->filepath, 240, 240);
	}

	if($submit == 'Upload photo' || $submit == 'Skip upload and use existing' )
	{
		//Build our new form element.
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

		if($submit == 'Skip upload and use existing')
		{
			$filepath = $path . '/' . $_POST['food_photo']['photo'];
			$form_state['storage']['food_photo']['filepath'] = $filepath;
			$form_state['storage']['food_photo']['fid'] = -1;
		}
		else
		{
			$form_state['storage']['food_photo']['filepath'] = $file->filepath;
			$form_state['storage']['food_photo']['fid'] = $file->fid;
			$filepath = $file->filepath;
		}

		$node = node_load($form['nid']['#value']);
		$content = $node->content;

		$selected = $_POST['food_diary']['meal_type'];
		$options = $form['food_diary']['meal_type']['#options'];

		$meal_type = strtolower($options[$selected]);

		//print_r($content['food'][$meal_type]['food']);

		$form_state['storage']['food_diary']['meal_type']['#options'] = $selected;
		$form_state['storage']['food_annotate']['food_portion'] = _pi_fooddiary_make_food_annotation_array($node, $meal_type);

		$form['food_annotate'] = $form_elements;
		$form['food_annotate']['file'] = array(	'#type' => 'hidden',
  												'#default_value' => $filepath );
			
		$form['food_diary']['form_build_id'] = array( '#type' => 'hidden' );
			
		form_set_cache($form_build_id, $form, $form_state);
		$form += array(	'#post' => $_POST,
    					'#programmed' => FALSE);

		// Rebuild the form.
		$form = form_builder('pi_fooddiary_node_form', $form, $form_state);
			
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
		$output .= pi_fooddiary_render_annotate($form, $form_state, FALSE);
		$output .= drupal_render($new_elements_form);

	}

	print drupal_to_js(array('status' => TRUE, 'data' => $output));
	exit;
}

/**
 * 
 * Enter description here ...
 */
function _pi_fooddiary_add_js_form_element()
{
	$form = array( '#tree' => TRUE );

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

/**
 * 
 * Enter description here ...
 */
function _pi_fooddiary_check_upload_directory()
{
	$path = file_directory_path() . '/' . FOODDIARY_FILE_PATH;
	file_check_directory( $path, TRUE);
}

/**
 * 
 * Enter description here ...
 * @param $node
 * @param $meal_type
 */
function _pi_fooddiary_make_food_annotation_array($node, $meal_type)
{
	$annotate = array();

	$foodoptions = _pi_fooddiary_get_food_options();
	$content = $node->content;

	//print_r($content);
	//echo $meal_type;

	$food_items = $content['food'][$meal_type]['food'];

	if(is_array($food_items))
	{
		foreach($food_items as $food_item)
		{
			$key = array_search($food_item['name'], $foodoptions);
				
			if($key !== FALSE)
			{
				$annotate[$key] = $food_item['amount'];
			}
		}
	}

	return $annotate;
}

?>
<?php

/**
 *
 * Enter description here ...
 */
function pi_fooddiary_select_day_js()
{
	$found = FALSE;
	$output = '';

	$date = $_POST['food_diary']['day'];

	$inquiry_id = $_POST['food_diary']['inquiry_id'];

	$foodinfos = _pi_fooddiary_get_all_fooddiarys_for_date( $date, $inquiry_id );

	if( is_array( $foodinfos ) && count( $foodinfos ) > 0 )
	{
		$found = TRUE;
		$nid = $foodinfos[0];
	}

	if( $found === FALSE )
	{
		// Build our new form element.
		$form_element = _pi_fooddiary_select_day_js_form_element();
		drupal_alter('form', $form_element, array(), 'pi_fooddiary_node_form');

		// Build the new form.
		$form_state = array('submitted' => FALSE);
		$form_build_id = $_POST['form_build_id'];
		// Add the new element to the stored form. Without adding the element to the
		// form, Drupal is not aware of this new elements existence and will not
		// process it. We retreive the cached form, add the element, and resave.
		if (!$form = form_get_cache($form_build_id, $form_state)) {
			exit();
		}
			
		//$form_state['storage']['food_diary']['day_select'] = $date;
		$form_state['storage']['food_diary']['day'] = $date;
		$form_state['storage']['food_diary']['date'] = $date;
			
		$form['food_diary'] = $form_element;
			
		form_set_cache($form_build_id, $form, $form_state);
			
		$form += array(	'#post' => $_POST,
    					'#programmed' => FALSE );

		// Rebuild the form.
		$form = form_builder('pi_fooddiary_node_form', $form, $form_state);

		// Render the new output.
		$new_element_form = $form['food_diary']['meal_type'];
		//$choice_form['#theme'] = 'pi_fooddiary_fooddiary_tmp';
			
		unset($new_element_form['#prefix'], $new_element_form['#suffix']); // Prevent duplicate wrappers.
		$new_element_form['#attributes']['class'] = empty($new_element_form) ? 'ahah-new-content' : $new_element_form['#attributes']['class'] .' ahah-new-content';
			
		$output .= drupal_render($new_element_form);
			
	}
	else
	{
		$activity_id = get_activity_id_for_nid($nid);
		$phase_id = get_phase_id_for_activity_id($activity_id);

		$output = "<script type=\"text/javascript\">
							<!--
							window.location = \"?q=activity/" . $activity_id . "/edit&destination=phase/" . $phase_id . "\" 
							//-->
						</script>";

		$output .=	"<div><p style=\"color: #FF0000; font-weigth: bold;\">Found existing data for " . $date_string . " " . $obj->nid . ". Reloading....</p></div>";
	}

	print drupal_to_js(array('status' => TRUE, 'data' => $output));
	exit;
}

/**
 *
 * Enter description here ...
 */
function _pi_fooddiary_select_day_js_form_element()
{
	$form = array( '#tree' => TRUE );

	$meal_options = array( 	t('Select meal type'), t('Breakfast'), t('Lunch'), t('Dinner'), t('Snack') );

	$meal_options_selected = 0;

	if( is_array($form_state) && array_key_exists( 'meal_type', $form_state) )
	{
		$meal_options_selected = $form_state['values']['fooddiary']['meal_type'];
	}

	$form['meal_type'] = array(
		'#type' => 'select',
		'#title' => t('Meal type'),
		'#required' => FALSE,
		'#multiple' => FALSE,
	    '#options' => $meal_options,
		'#default_value' => $meal_options_selected,
		'#ahah' => array(
	//'#bind' => array(
				'event' => 'change',
	//'path'    => ahah_helper_path(array('fooddiary')),
				'path'    => 'activity/pi_fooddiary_select_meal_type/js',
				'wrapper' => 'food-photo-wrapper',
	),
	//this fixes some layout issues with the ahah throbber
	//'#attributes' => array('style'=>'overflow:auto;border: thin solid black;'),
		'#prefix' => "<div id='meal-wrapper' style='min-height:40px;'>",
		'#suffix' => '</div>'
		);

		return $form;
}

?>
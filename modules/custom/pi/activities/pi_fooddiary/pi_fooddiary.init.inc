<?php

define( 'FOODDIARY_DATE_TYPE', 		'Fooddiary Date' );
define( 'FOODDIARY_FOODTYPE_TYPE', 	'Fooddiary Food type' );
define( 'FOODDIARY_FOODIMAGE_TYPE', 'Fooddiary Image' );
define( 'FOODDIARY_FOOD_TYPE', 		'Fooddiary Food' );
define( 'FOODDIARY_PORTION_TYPE', 	'Fooddiary Portion size' );

/**
 * 
 * Enter description here ...
 */
function _pi_fooddiary_add_available_measures()
{
	global $user;
	
	$new_node1 = _pi_fooddiary_add_available_measures_date();
	$new_node2 = _pi_fooddiary_add_available_measures_foodtype();
	$new_node3 = _pi_fooddiary_add_available_measures_foodimage();
	$new_node4 = _pi_fooddiary_add_available_measures_food();
	$new_node5 = _pi_fooddiary_add_available_measures_potion();
	
	$selected_measures[] = $new_node2->nid;
	$selected_measures[] = $new_node3->nid;
	$selected_measures[] = $new_node4->nid;
	$selected_measures[] = $new_node5->nid;
	
	$selected_measure_node = new stdClass();
	
	$selected_measure_node->type				= 'pi_sort_measures';
	$selected_measure_node->uid					= $user->uid;
	$selected_measure_node->required_measures	= $new_node1->nid;
	$selected_measure_node->selected_measures	= $selected_measures;
	
	_pi_fooddiary_add_selected_measures($selected_measure_node);
}

/**
 * 
 * Enter description here ...
 */
function _pi_fooddiary_add_available_measures_date()
{
	global $user;
	//Date
	$new_node = new stdClass();
	$new_node->type 							= 'pi_available_measures';
	$new_node->uid								= $user->uid;
	$new_node->title 							= FOODDIARY_DATE_TYPE;
	$new_node->symbol 							= FOODDIARY_DATE_TYPE;
	$new_node->units							= '';
	$new_node->units_short						= '';
	$new_node->field_type						= 'textfield';
	$new_node->data_type						= 'text';
	$new_node->list_options						= '';
	$new_node->content_options					= 0;
	$new_node->textfield_size					= 5;
	$new_node->textarea_cols					= 50;
	$new_node->textarea_rows					= 3;
	$new_node->measurement_device_name			= '';
	$new_node->measurement_device_description	= 'Anotate food photo (date)';
	
	node_save($new_node);
	
	return $new_node;
}

/**
 * 
 * Enter description here ...
 */
function _pi_fooddiary_add_available_measures_foodtype()
{
	global $user;
	//Food type e.x. Lunch
	$new_node = new stdClass();
	$new_node->type 							= 'pi_available_measures';
	$new_node->uid								= $user->uid;
	$new_node->title 							= FOODDIARY_FOODTYPE_TYPE;
	$new_node->symbol 							= FOODDIARY_FOODTYPE_TYPE;
	$new_node->units							= '';
	$new_node->units_short						= '';
	$new_node->field_type						= 'select';
	$new_node->data_type						= 'menu';
	$new_node->list_options						= "Breakfast\nLunch\nDinner\nSnack";
	$new_node->content_options					= 0;
	$new_node->textfield_size					= 5;
	$new_node->textarea_cols					= 50;
	$new_node->textarea_rows					= 3;
	$new_node->measurement_device_name			= '';
	$new_node->measurement_device_description	= 'Anotate food photo (food type)';
	
	node_save($new_node);
	
	return $new_node;
}

/**
 * 
 * Enter description here ...
 */
function _pi_fooddiary_add_available_measures_foodimage()
{
	global $user;
	//Image
	$new_node =  new stdClass();
	$new_node->type 							= 'pi_available_measures';
	$new_node->uid								= $user->uid;
	$new_node->title 							= FOODDIARY_FOODIMAGE_TYPE;
	$new_node->symbol 							= FOODDIARY_FOODIMAGE_TYPE;
	$new_node->units							= '';
	$new_node->units_short						= '';
	$new_node->field_type						= '_image_';
	$new_node->data_type						= 'text';
	$new_node->list_options						= "";
	$new_node->content_options					= 0;
	$new_node->textfield_size					= 5;
	$new_node->textarea_cols					= 50;
	$new_node->textarea_rows					= 3;
	$new_node->measurement_device_name			= '';
	$new_node->measurement_device_description	= 'Anotate food photo (image)';
	
	node_save($new_node);
	
	return $new_node;
}

/**
 * 
 * Enter description here ...
 */
function _pi_fooddiary_add_available_measures_food()
{
	global $user;
	//Food reference
	$new_node = new stdClass();
	$new_node->type 							= 'pi_available_measures';
	$new_node->uid								= $user->uid;
	$new_node->title 							= FOODDIARY_FOOD_TYPE;
	$new_node->symbol 							= FOODDIARY_FOOD_TYPE;
	$new_node->units							= '';
	$new_node->units_short						= '';
	$new_node->field_type						= 'select';
	$new_node->data_type						= 'menu';
	$new_node->list_options						= implode("\n",_pi_fooddiary_get_food_options());
	$new_node->content_options					= 0;
	$new_node->textfield_size					= 5;
	$new_node->textarea_cols					= 50;
	$new_node->textarea_rows					= 3;
	$new_node->measurement_device_name			= '';
	$new_node->measurement_device_description	= 'Anotate food photo (food)';
	
	node_save($new_node);
	
	return $new_node;
}

/**
 * 
 * Enter description here ...
 */
function _pi_fooddiary_add_available_measures_potion()
{
	global $user;
	//Portion size
	$new_node = new stdClass();
	$new_node->type 							= 'pi_available_measures';
	$new_node->uid								= $user->uid;
	$new_node->title 							= FOODDIARY_PORTION_TYPE;
	$new_node->symbol 							= FOODDIARY_PORTION_TYPE;
	$new_node->units							= '';
	$new_node->units_short						= '';
	$new_node->field_type						= 'select';
	$new_node->data_type						= 'menu';
	$new_node->list_options						= "1\n2\n3\n4\n5\n6\n7\n8\n9\n10";
	$new_node->content_options					= 0;
	$new_node->textfield_size					= 5;
	$new_node->textarea_cols					= 50;
	$new_node->textarea_rows					= 3;
	$new_node->measurement_device_name			= '';
	$new_node->measurement_device_description	= 'Anotate food photo (portion size)';
	
	node_save($new_node);
	
	return $new_node;
}

/**
 * 
 * Enter description here ...
 * @param $selected_measure_node
 */
function _pi_fooddiary_add_selected_measures($selected_measure_node)
{
	node_save($selected_measure_node);
}
?>
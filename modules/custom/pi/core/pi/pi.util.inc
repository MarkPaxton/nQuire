<?php
/**
 * @file nQuire general utility functions
 */

/**
 * Check if the given $node is a node of type given
 * @param $possible_node
 * @param string $desired_type
 * @return bool true if possible node is a node of desired type false if not 
 */
function pi_check_is_node_of_type($possible_node, $desired_type)
{
  $check = false;
  if(is_object($possible_node))
  {
    if($possible_node->type==$desired_type)
    {
      $check= true;
    }
  }
  return $check;
}

/**
 * Check if a node exists and is of the given type
 * @param uint $possible_nid
 * @param string $desired_type
 * @return bool true if possible node exists and is a node of desired type false if not 
 */
function pi_check_node_exists_and_is_of_type($possible_nid, $desired_type)
{
  return (is_numeric($possible_nid) ? pi_check_is_node_of_type(node_load($possible_nid), $desired_type) : false);
}


/**
 * Convert a result constant from drupal_write_record to a string for debug purposes
 * @param int $result Result variable as a defined consant from drupal_write_record, as defined in common.int
 * @return string A string representing the result status, or the number if it's an undefined value
 */
function _pi_write_record_result_string($result)
{
	$result_string = "";
	switch($result)
	{
		case(SAVED_NEW):
			$result_string = "SAVED_NEW";
			break;
		case SAVED_UPDATED:
			$result_string = "SAVED_UPDATED";
			break;
		case SAVED_UPDAED:
			$result_string = "SAVED_DELETED";
			break;
		default:
			$result_string = "$result";
	}
	return $result_string;
}



/**
 * This is the validation function to make sure that pi_phase.inquiry_nid is a valid inquiry
 * @param unknown_type $element
 * @param unknown_type $form_state
 */
function pi_util_inquiry_nid_validate($element, $form_state)
{
  if($element['#type']!= 'hidden' && !in_array($element['#value'], $element['#allowed_inquiry_nids']))
  {
    form_error($element, t('The selected inquiry is not valid, please choose an existing inquiry.'));
  }
}


/**
 * This is the validation function to make sure that pi_phase.inquiry_nid is a valid inquiry
 * @param unknown_type $element
 * @param unknown_type $form_state
 */
function pi_util_phase_nid_validate($element, $form_state)
{
	if($element['#type']!= 'hidden' && !in_array($element['#value'], $element['#allowed_phase_nids']))
	{
		form_error($element, t('The selected phase is not valid, please choose an existing phase from an inquiry.'));
	}
}

/**
 * This compares the ['#weight'] elements in two arrays
 * @param array $x
 * @param array $y
 * @return bool result of $x['#weight'] > $y['#weight']
 */
function my_weight_compare($x, $y) {
	$xw = (int) $x['#weight'];
	$yw = (int) $y['#weight'];
	return $xw > $yw;
}


function compare_phase($a, $b) {
	$aposition = $a->position;
	$bposition = $b->position;
	$aphase = $a->phase_id;
	$bphase = $b->phase_id;
	if ($aphase == $bphase) {
		if ($aposition == $bposition) {
			return 0;
		};
		if ($aposition < $bposition) {
			return -1;
		};
		if ($aposition > $bposition) {
			return 1;
		};
	}
	else {
		if ($aphase < $bphase) {
			return -1;
		};
		if ($aphase > $bphase) {
			return 1;
		};
	}
}

function compare_status($a, $b) {
	$aposition = $a->position;
	$bposition = $b->position;
	$status = array();
	$status["start"] = 0;
	$status["edit"] = 1;
	$status["view"] = 2;
	$status["unavailable"] = 3;
	$astatus = $status[$a->visible_status];
	$bstatus = $status[$b->visible_status];
	if ($astatus == $bstatus) {
		if ($aposition == $bposition) {
			return 0;
		};
		if ($aposition < $bposition) {
			return -1;
		};
		if ($aposition > $bposition) {
			return 1;
		};
	}
	else {
		if ($astatus < $bstatus) {
			return -1;
		};
		if ($astatus > $bstatus) {
			return 1;
		};
	}
}



/**
 *
 * Sort an array of arrays by one of their elements - e.g. a list of set result rows to be
 * sorted by a column.  Key and value associations are maintained.
 * @param $array Array of arrays to be sorted
 * @param $element the name of the array key to sort by
 */
function pi_sort_arrays_by_element(&$array, $element)
{
	$return_array = array();
	$element_array = array();
	foreach($array as $key => $value)
	{
		$element_array[$key] = $value[$element];
	}
	asort($element_array);
	foreach($element_array as $key)
	{
		$return_array[$key] = $array[$key];
	}
	$array = $return_array;
}


/**
 * This is a utility function to merge two content item lists which have '#weight' elements
 * e.g. item[x]['#weight'].  Weight elements in the second list are ignored and this list is
 * appended to the ordrered items in their default order.
 * If an unsorted item occurrs in the sorted item list, the sorted item weight takes precidence
 * Weight attributes are renumbered incrementally from 0
 * @param array(array) $sorted_items content item list with '#weight' elements
 * @param $unsorted_items  $sorted_items content item list - weight ignored
 * @return array of items sorted by weight
 */
function pi_merge_content_item_lists($sorted_items, $unsorted_items)
{
	$return_data = array();
	$weight_data = array();
	if ($sorted_items)
	{
		foreach($sorted_items as $key => $sorted_item)
		{
			$weight_data[$key] =  $sorted_item['#weight'];
		}
	}
	asort($weight_data);
	foreach(array_keys($weight_data) as $ordered_item_key)
	{
		$sorted_items[$ordered_item_key]['#weight'] = count($return_data);
		$return_data[$ordered_item_key] = $sorted_items[$ordered_item_key];
	}
	if( is_array( $unsorted_items ) )
	{
		foreach($unsorted_items as $unsorted_item_key => $unsorted_item)
		{
			if(!array_key_exists($unsorted_item_key, $return_data))
			{
				$unsorted_item['#weight'] = count($return_data);
				$return_data[$unsorted_item_key] = $unsorted_item;
			}
		}
	}
	return $return_data;
}

/**
 * This is a function to sort items by first array index then then by #weight property
 * The status order is determined by $group_order, which will set the order for the groups
 * the #weight parameter of the items will be reset to a sequential series starting from 0
 * any groups that are not listed are added to the end of the list in order of occurrence
 * @param array $group_order array of indexes to define what order the grouped items will be sorted, e.g. key, selected, none
 * @param $grouped_items array of arrays of items with #weight parameters.
 * 	e.g. array(
 * 		'key' =>
 * 			array(
 * 				'123'=> array('#weight' => 12),
 * 				'321'=>array('#weight' => 32),
 * 			),
 * 		'selected' => etc
 * @param string $weight_index this will usually be #weight for display items
 * 	but for form data it could be anything, e.g. 'weight' is often used
 * @return array Flattened array of items from $grouped_items with a sequential #weight value.
 */
function pi_sort_grouped_item_list($group_order, $grouped_items, $weight_index)
{
	$item_ids = array();
	$item_groups = array();
	$item_weights = array();
	$items = array();
	foreach($grouped_items as $group_name => $group)
	{
		if( is_array($item_ids) )
		{
			if( !array_key_exists($group_name, $item_ids) ||
			( array_key_exists($group_name, $item_ids) && !is_array($item_ids[$group_name]) ) )
			{
				$item_ids[$group_name] = array();
			}
		}

		if( is_array($item_weights) )
		{
			if( !array_key_exists($group_name, $item_weights) ||
			( array_key_exists($group_name, $item_weights) && !is_array($item_weights[$group_name]) ) )
			{
				$item_weights[$group_name] = array();
			}
		}

		foreach($group as $item_id => $item)
		{
			$item_ids[$group_name][] = $item_id;
			$item_weights[$group_name][] = $item[$weight_index];
			$items[$item_id] = $item;
		}
	}

	// If new groups appear that aren't in the default list, append them to the end of the list:
	foreach(array_keys($item_ids) as $other_group)
	{
		if(!in_array($other_group, $group_order))
		{
			$group_order[] = $other_group;
		}
	}
	$sorted_items = array();
	foreach($group_order as $group)
	{
		if(array_key_exists($group, $item_ids)!=NULL)
		{
			$sort_weights = $item_weights[$group];
			$sort_ids = $item_ids[$group];
			array_multisort($sort_weights, $sort_ids);
			foreach($sort_ids as $item_id)
			{
				$items[$item_id][$weight_index] = count($sorted_items);
				$sorted_items[$item_id] = $items[$item_id];
			}
		}
	}
	return $sorted_items;
}

/**
 * This function takes a flattened array of items and groups them into arrays accouting to
 * the values of group_field
 * @param array or array $items items to sort
 * @param string $group_field the field of the array by which to group items
 * @return a nested array, first index is the value of $items['group_field'] and the second index the current
 * key of $item.
 * array('group_field1' => array ('id'=>array(value=>'value, group_field=>'group_field1', .), .. )...)
 */
function pi_create_grouped_item_list($items, $group_field)
{
	$return_array=array();
	foreach($items as $key => $item)
	{
		$group_key = $item[$group_field];
		if(!array_key_exists($group_key, $return_array))
		{
			$return_array[$group_key]=array();
		}
		$return_array[$group_key][$key] = $item;
	}
	return $return_array;
}

/**
 *
 * This does the opposite of pi_create_grouped_item_list and takes all the elements of $item_list
 * and combines them into one array
 * n.b. if items in the sub-arrays have dupliucate keys, the rturn element will be overwritten by the value of the
 * later sub-list element.
 * @param array(array(),array(),array()) $item_list
 * @return array of contents of the above array elements in a single array
 */
function pi_flatten_grouped_item_list($item_list)
{
	$list = array();
	$all_defined_groups = array();
	foreach(array_keys($item_list) as $region)
	{
		foreach($item_list[$region] as $key => $item)
		{
			$list[$key] = $item;
		}
	}
	return $list;
}


/**
 *
 * This ensures that there is only one unique row in $table with $id_field equal to $id
 *  if there are no rows, it returns zero, otherwise it returns one.
 *  If more than one row exists then the lowest $id is kept and the rest are deleted.
 * @param unknown_type $table
 * @param unknown_type $id_field
 * @param int $id
 * @return bool true if one row is found false if no row exists;
 */
function pi_db_ensure_unique($table, $id_field, $id)
{
	$one_row = false;
	// See how many rows there are are
	$query = "SELECT COUNT(%s) FROM {%s} WHERE %s = %d";
	$result = db_query($query, $id_field, $table, $id_field, $id);
	$count = db_result($result);
	// Check if the group type already exists and leave only one definition left
	if($count>1)
	{
		// if the id has any duplicates remove them...
		$query = "DELETE FROM {%s} WHERE %s = %d  LIMIT %d";
		db_query($query, $table, $id_field, $id, $count-1);
		$one_row = true;
	}
	else
	{
		$one_row = $count;
	}
	return $one_row;
}

/**
 *
 * Return an array indexed by nid with value the title of the node, or NULL
 * if the node is not found
 * @param array uint $nids Array of node ids
 * @return  array('nid' => 'title', ...)
 */
function pi_util_get_names_from_nids($nids)
{
	$return_data = array();
	if(is_array($nids))
	{
		foreach($nids as $nid)
		{
			$n = node_load($nid);
			if($n!=NULL)
			{
				$return_data[$nid] = $n->title;
			}
			else
			{
				$return_data[$nid] = NULL;
			}
		}
	}
	return $return_data;
}



/**
 * This function updates the title of the node in the node and node_revision table
 * @param $node
 * @param $title 
 */
function pi_util_set_node_title($node, $title=NULL)
{
	if ($title === NULL)
	{
		$title = $node->body;
	}
	//get activity title from node body
	$length = 250;
	if(strlen($title) > $length)
	{
		$short_body = substr(check_plain($title), 0, $length);
		$last_space = strrpos($short_body, " ");
		$short_body = substr($short_body, 0, $last_space);
		$title = $short_body."...";
	}
    $node->title = $title;
    node_save($node);
}



?>
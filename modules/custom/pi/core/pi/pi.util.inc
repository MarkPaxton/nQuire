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
function pi_check_is_node_of_type($possible_node, $desired_type) {
	$check = false;
	if (is_object($possible_node)) {
		if ($possible_node->type == $desired_type) {
			$check = true;
		}
	}
	return $check;
}



/**
 * Convert a result constant from drupal_write_record to a string for debug purposes
 * @param int $result Result variable as a defined consant from drupal_write_record, as defined in common.int
 * @return string A string representing the result status, or the number if it's an undefined value
 */
function _pi_write_record_result_string($result) {
	$result_string = "";
	switch ($result) {
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
 * Shorten text to length given, trimmign whole words and add postfix text to the end if it's shortened
 * @param $text
 * @param $length
 * @param $postfix
 */
function pi_util_trim_text($text, $length = 250, $postfix = "...") {
	$return_text = $text;
	if (strlen($text) > $length) {
		$short_body = substr($text, 0, $length);
		$last_space = strrpos($short_body, " ");
		$short_body = substr($short_body, 0, $last_space);
		$return_text = $short_body . $postfix;
	}
	return $return_text;
}

/**
 * This function updates the title of the node in the node and node_revision table
 * @param $node
 * @param $title
 */
function pi_util_set_node_title($node, $title = NULL) {
	if ($title === NULL)
		$title = $node->body;
	$node->title = pi_util_trim_text(check_plain($title));
	node_save($node);
}

/**
 * Return an array of the drupal path elements in parts or null if there's a problem
 */
function pi_util_get_drupal_path_parts() {
	$drupal_path_parts = NULL;
	//Strip the request url of the preceeding drupal stuff base_url and ?q=/
	// so that it's possible to work out whether the page request was from /inquiry/%inquiry/phase/add
	// or from /node/%node/edit
	$drupal_path = str_replace(base_path(), '', request_uri());
	$drupal_path = str_replace('?q=', '', $drupal_path);
	// Split the string into parts
	$drupal_path_parts = explode('/', $drupal_path);
	return $drupal_path_parts;
}

/**
 * 
 * See if a url matches a given pattern, in the array specified. Each element in the array is compared with path parts
 * and if the element is NULL then it's considered a wildcard, size of the arrays is the minimum number of parts in the path, no maximum length can
 * be specified here
 * e.g. array("activity", NULL, "edit", NULL) would match the path
 * /activity/ * /edit/ *   
 *  URL parameters, and the drupal installation folder are ignored
 * @param String $url
 * @param array $matches
 */
function pi_util_match_url($matches = array()) {
	// Initially set it to true since we are only looking for one mismatch
	$return_value = true;
	// Split the string into parts 
	$path_parts = pi_util_get_drupal_path_parts();
	//See if there's the right number of path parts...
	if (count($path_parts) >= count($matches)) {
		// Now check all the parts for vaildity... 
		$count = 0;
		foreach ($matches as $match) {
			if ($match != NULL)
				if (strcasecmp($path_parts[$count], $match) !== 0)
					$return_value = false;
			$count++;
		}
	}
	else {
		$return_value = false;
	}
	return $return_value;
}

/**
 * Return the path part with the given index (see pi_util_get_drupal_path_parts)
 * @param uint $index Starting at 0
 */
function pi_util_get_path_element($index) {
	$parts = pi_util_get_drupal_path_parts();
	return $parts[$index];
}

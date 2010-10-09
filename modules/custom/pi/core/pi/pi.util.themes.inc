<?php 
/**
 * @file Utility theme functions for nQuire
 */


/**
 * 
 * This theme function takes all child elements of a fieldset/tree element and renders them
 *  in a div with float:left property making all the elements flow left to right rather than
 *  on new lines as normal  
 * @param $form
 * @param $form_state
 */
function theme_pi_util_form_elements_float($form)
{
	$output = '';
	$output .= '<div>';
	
	$field_content = '';
	foreach(element_children($form) as $stage_element)
	{
		$field_content .= "<div style='float:left;margin-right:8px;'>";
		$field_content .= drupal_render($form[$stage_element]);
		$field_content .= "</div>\n";		
	}
	$output .= $field_content;
	
	$output .= drupal_render($form);
	
	$output .= "</div>\n";
	return $output;	
}

?>
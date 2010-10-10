<?php 
/**
 * 
 *  @file Theme functions for pi_activity editing forms
 */

/**
 * 
 * This provides the theme function to display the 'actor/audience explanation' for the
 * activity editing form 
 * @param string $explanation The main explanation of the setting
 * @param array(string) $preview The preview examples listing the actual affects of the setting
 * @return html formatted string
 */
function theme_pi_activity_form_actor_audience_explanation($explanation, $preview)
{
	$output = '';
	$output .= '<strong>' . $explanation . '</strong>';
	foreach($preview as $item)
	{
		$output .= '<div>' . $item . '</div>' . "\n";
	} 
	return $output;	
}

?>
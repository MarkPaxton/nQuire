<?php
/** 
 * @file Theme functions for sortable regions forms within nQuire
 */


/**
 * Theme for generic region sortable list  
 * @param array $header column/field headings in drupal table format (string or array with data and other attributes 
 * @param array $rows The structure for each row in drupal table theme format 
 * @param array(string) $regions array with key of region keys and array with description and empty text eg:
 * 	'Selected' = array('title'=>'Selected', description'=>'Items you will use', 'empty'=>'There are no selected items'); 
 * @param $attributes attributes of the table
 */
function theme_pi_region_sort_table($header, $rows, $regions, $attributes) {

 // Add table javascript.

    drupal_add_js('misc/tableheader.js');
    drupal_add_js(drupal_get_path('module', 'pi') .'/lib/pi_region_sort.js');

	foreach (array_keys($regions) as $region)
	{
		drupal_add_tabledrag('sort_list', 'match', 'sibling', 'sort_list-region-select', 'sort_list-region-' . $region, NULL, FALSE);
		drupal_add_tabledrag('sort_list', 'order', 'sibling', 'sort_list-weight', 'sort_list-weight-' . $region, NULL, FALSE);
	}

	$output = "";
	$output .= '<table id="' . $attributes['id'] . '" class="sticky-enabled">';

	/* //don't show the table header
	$output .= '<thead>';
	$output .= '<tr>';
	foreach($header as $heading) {
    	$output .= '<th class="' . $heading['class'] . '">' . $heading['theading'] . '</th>';
	}
	$output .= '</tr>';
  	$output .= '</thead>';*/
  	
	
  	$output .= '<tbody>';
    foreach ($regions as $region => $region_data) {
		$output .= '<tr class="region region-' . $region . '">';
		$output .= '<td class="region" colspan="' . count($header) . '"><span style="font-size:130%;font-weight:bold;margin-right:15px;">' . $region_data['title'] . '</span>';
		$output .= '<span style="font-size:90%;font-weight:normal">' . $region_data['description'] . '</span></td>';
		$output .= '</tr>';
		
		$region_style = empty($rows[$region]) ? 'region-empty' : 'region-populated';
        $output .= '<tr class="region-message region-' . $region . '-message ' . $region_style . '">';
        $output .= '<td colspan="' . count($header) . '"><em>' . t($region_data['empty']) . '</em></td>';
        $output .= '</tr>';
      
		$row = 0;
		
		//print_r($rows[$region]);
		
		if (is_array($rows[$region]))
		{
			foreach ($rows[$region] as $key => $data)
			{
				$modified_description = str_replace(
					array('<div', '</div', 'class="submitted"'), 
					array('<span', '</span', 'class="submitted" style="display:none"'),  
					$data['description']);
				$style = $row % 2 == 0 ? 'odd' : 'even';
				$row_class = $data->row_class ? ' '. $data->row_class : '';
				$output .= '<tr class="draggable ' . $style . $row_class . '">';
				$output .= '<td class="sort_item_title"><span style="font-size:115%;font-weight:bold;">' . $data['title'] . '</td>';
				$output .= '<td class="sort_item_description">' .  $modified_description . '</td>';
				$output .= '<td class="sort_item_region">' . $data['region'] . '</td>';
				$output .= '<td class="sort_item_weight">' . $data['weight'] . '</td>';
				$output .= '</tr>';
				$row++;
			}
		}
	}
    $output .= '</tbody>';
	$output .= '</table>';

	return $output;
}


?>
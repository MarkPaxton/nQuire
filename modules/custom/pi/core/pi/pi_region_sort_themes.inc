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

	$output = '';

	// Add table javascript for draggable tables
    drupal_add_js('misc/tableheader.js');
    drupal_add_js(drupal_get_path('module', 'pi') .'/lib/pi_region_sort.js');

    // Set up the regions:
	foreach (array_keys($regions) as $region)
	{
		drupal_add_tabledrag('sort_list', 'match', 'sibling', 'sort_list-region-select', 'sort_list-region-' . $region, NULL, FALSE);
		drupal_add_tabledrag('sort_list', 'order', 'sibling', 'sort_list-weight', 'sort_list-weight-' . $region, NULL, FALSE);
	}

	//Set up table attributes
	// add the extra classes if classes are already set
	$table_attributes = $attributes;
	$class_to_add = 'sticky-enabled';
	if(array_key_exists('class', $table_attributes))
	{
		$table_attributes['class'] .= " $class_to_add"; 
	}
	else
	{
		$table_attributes['class'] = $class_to_add;	
	}

	// Copy the header given
	//don't show the table header
	//$table_header = $header;
	$table_header = NULL;
	
	//Now set up the individual rows for regions and elements	
	$table_rows[] = array();
    foreach ($regions as $region => $region_data) 
    {
    	//Add the region heading
    	$table_rows[] = array( 'class'=> "region region-$region",    		
			'data' => array(
    			array('class' => 'region', 'colspan'=> count($header), 'header' => true,
	    		'data'=> 
	    			'<span style="font-size:130%;font-weight:bold;margin-right:15px;">' . $region_data['title'] . '</span>' .
					'<span style="font-size:90%;font-weight:normal">' . $region_data['description'] . '</span>')
    		)
    	);
		
    	// Add the heading 'empty' message row - hidden or not as needed
		$region_style = empty($rows[$region]) ? 'region-empty' : 'region-populated';
    	$table_rows[] = array('class'=> "region-message region-$region-message $region_style",
        	'data' => array(
    			array('colspan'=> count($header), 'header' => true,
    				'data'=>'<em>' . $region_data['empty'] . '</em>')
    			)
    	);
        
      
		$row = 0;
		if (is_array($rows[$region]))
		{
			foreach ($rows[$region] as $key => $data)
			{
				$style = $row % 2 == 0 ? 'odd' : 'even';
				$row++;
				
				$row_class = 'draggable';
				if($data->row_class!=NULL)
				{	
					$row_class .= ' ' . $data->row_class;
				}
				
				// Set up the row
				$table_row = array('data'=>array(), 'class'=>$row_class, 'style'=>$style);

				// Add the cells
				$table_row['data'][] = array(
					'class'=>"sort_item_title", 
					'data' => '<span style="font-size:115%;font-weight:bold;">' 
							. $data['title'] . '</span>');

				$modified_description = str_replace(
					array('<div', '</div', 'class="submitted"'), 
					array('<span', '</span', 'class="submitted" style="display:none"'),  
					$data['description']);							
				$table_row['data'][] = array(
					'class'=>"sort_item_description", 
					'data' => $modified_description);

				$table_row['data'][] = array(
					'class'=>"sort_item_region", 
					'data' => $data['region']);
							
				$table_row['data'][] = array(
					'class'=>"sort_item_weight", 
					'data' => $data['weight']);
				
				$table_rows[] = $table_row;
			}
		}
	}
	
	$output .= theme('table', $table_header, $table_rows, $table_attributes);
	return $output;
}


?>
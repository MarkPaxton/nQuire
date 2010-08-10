<?php

module_load_include('module', 'pi_foodinfo');

/**
 * This function generates the HTML code for the food anotation in the food diary from.
 * @param array $form The drupal from variable. 
 * @param array $form_state The drupal form_state variable
 * @param boolean $show_table Boolean variable, indicates wether or not to generade HTML code for table
 * @see _pi_fooddiary_render_nutrinal_table
 */
function pi_fooddiary_render_annotate($form, $form_state, $show_table=TRUE)
{
	$food_item = get_food_array($form, $form_state);

	//print_r($food_item);
	
	$output .= "<div class=\"fooddiary_outer_div\">
					<div  class=\"fooddiary_images\">
						<img src=\"" . $form_state['storage']['food_photo']['filepath'] . "\">
					</div>";

	$output .= "	<div class=\"fooddiary_portions\">
						<div style=\"margin: 10px;\">";

	$nutritional = array();
	$nutritional = _pi_fooddiary_calcuate_nutritional_info($food_item['food']);

	$output .= "		<table style=\"width: 100%; margin: 0px 0px 10px 0px; border: 1px solid #000000; border-collapse:collapse;\">
							<caption>" . t($food_item['title']) . "</caption>
							<tr>
								<th class=\"fooddiary_portion_table_th\">" . t('Food') . "</th>
								<th class=\"fooddiary_portion_table_th\">". t('Portion Size') . "</th>
							</tr>";

	if(is_array($food_item['food']))
	{
		foreach( $food_item['food'] as $food_portion )
		{
			_pi_foodinfo_get_portion_info_for_chid;
			
			$output .= "			<tr>
										<td class=\"fooddiary_portion_table_td\">" . $food_portion['name'] . "</td>
										<td class=\"fooddiary_portion_table_td\">" . $food_portion['amount'] . " " . $food_portion['portion'] . "</td>
									</tr>";
		}
	}

	$output .= "		</table>";

	$output .= "		</div>
 					</div>
 				</div><div class=\"fooddiary_clear\">";

	if(is_array($nutritional))
	{
		$output .= _pi_fooddiary_render_nutrinal_table($nutritional);
	}
	
	$output .= '<br />';

	return $output;
}

/**
 * This function generates the HTML code for the nutrinal information.
 * @param array $nutritional An array containing the nutrinal information.
 * @return string $output Returns the HTML code
 */
function _pi_fooddiary_render_nutrinal_table($nutritional)
{
	$output = '';

	$output .= "<div class=\"food_table\">
					<table class=\"fooddiary_table_table\">
						<tr class=\"foodannotation_odd\">
							<td>Carbohydrates (g)</td><td>". $nutritional['carb'] ."</td>
						</tr>
						<tr class=\"foodannotation_even\">
							<td>Protein (g)</td><td>". $nutritional['protein'] ."</td>
						</tr>
						<tr class=\"foodannotation_odd\">
							<td>Fats (g)</td><td>". $nutritional['fats'] ."</td>
						</tr>
						<tr class=\"foodannotation_even\">
							<td>Fibre (g)</td><td>". $nutritional['fibre'] ."</td>
						</tr>
						<tr class=\"foodannotation_headline\">
							<td  colspan=\"2\" ><b>Minerals</b></td>
						</tr>
						<tr class=\"foodannotation_even\">							
							<td>Sodium (g)</td><td>". $nutritional['sodium'] ."</td>
						</tr>
						<tr class=\"foodannotation_odd\">
							<td>Calcium (g)</td><td>". $nutritional['calcium'] ."</td>
						</tr>
						<tr class=\"foodannotation_even\">
							<td>Iron (g)</td><td>". $nutritional['iron'] ."</td>
						</tr>
						<tr class=\"foodannotation_headline\">
							<td colspan=\"2\"><b>Vitamins</b></td>
						</tr>
						<tr class=\"foodannotation_even\">
							<td>Vitamins A (mg)</td><td>". $nutritional['vitamin_a'] ."</td>
						</tr>
						<tr class=\"foodannotation_odd\">
							<td>Vitamins B (mg)</td><td>". $nutritional['vitamin_b'] ."</td>
						</tr>
						<tr class=\"foodannotation_even\">
							<td>Vitamins C (mg)</td><td>". $nutritional['vitamin_c'] ."</td>
						</tr>
						<tr class=\"foodannotation_odd\">
							<td>Vitamins D (mg)</td><td>". $nutritional['vitamin_d'] ."</td>
						</tr>
						<tr class=\"foodannotation_even\">
							<td>Water(g)</td><td>". $nutritional['water'] ."</td>
						</tr>
					</table>
				</div>";
	
	return $output;
}

/**
 * 
 * @param array $form
 * @param array $form_state
 * @return array $food 
 */
function get_food_array($form, $form_state)
{
	$meal_types = $form['food_diary']['meal_type']['#options'];

	$selected_meal_type = $form_state['storage']['food_diary']['meal_type']['#options'];

	$meal_type = $meal_types[$selected_meal_type];

	$food_options = $form['food_annotate']['food_choice']['#options'];
	
	if(is_array($food_options))
	{
		foreach($food_options as $key => $food_option)
		{
			if($form_state['storage']['food_annotate']['food_portion'][$key] > 0)
			{
				$food_items[] = array(	'name' => $food_option,
										'amount' => $form_state['storage']['food_annotate']['food_portion'][$key],
										'portion' => _pi_foodinfo_get_portion_info_for_chid($food_option)
									);
			}
		}
	}

	$food = array(	'title' => t($meal_type),
					'image'	=> $form_state['storage']['food_photo']['filepath'],
					'food'	=> $food_items					
				);

	return $food;
}

?>
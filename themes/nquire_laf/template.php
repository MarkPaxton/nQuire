<?php

/**
 * @file
 * Contains theme override functions and preprocess functions for the theme.
 *
 * ABOUT THE TEMPLATE.PHP FILE
 *
 *   The template.php file is one of the most useful files when creating or
 *   modifying Drupal themes. You can add new regions for block content, modify
 *   or override Drupal's theme functions, intercept or make additional
 *   variables available to your theme, and create custom PHP logic. For more
 *   information, please visit the Theme Developer's Guide on Drupal.org:
 *   http://drupal.org/theme-guide
 *
 * OVERRIDING THEME FUNCTIONS
 *
 *   The Drupal theme system uses special theme functions to generate HTML
 *   output automatically. Often we wish to customize this HTML output. To do
 *   this, we have to override the theme function. You have to first find the
 *   theme function that generates the output, and then "catch" it and modify it
 *   here. The easiest way to do it is to copy the original function in its
 *   entirety and paste it here, changing the prefix from theme_ to nquire_laf_.
 *   For example:
 *
 *     original: theme_breadcrumb()
 *     theme override: nquire_laf_breadcrumb()
 *
 *   where nquire_laf is the name of your sub-theme. For example, the
 *   zen_classic theme would define a zen_classic_breadcrumb() function.
 *
 *   If you would like to override any of the theme functions used in Zen core,
 *   you should first look at how Zen core implements those functions:
 *     theme_breadcrumbs()      in zen/template.php
 *     theme_menu_item_link()   in zen/template.php
 *     theme_menu_local_tasks() in zen/template.php
 *
 *   For more information, please visit the Theme Developer's Guide on
 *   Drupal.org: http://drupal.org/node/173880
 *
 * CREATE OR MODIFY VARIABLES FOR YOUR THEME
 *
 *   Each tpl.php template file has several variables which hold various pieces
 *   of content. You can modify those variables (or add new ones) before they
 *   are used in the template files by using preprocess functions.
 *
 *   This makes THEME_preprocess_HOOK() functions the most powerful functions
 *   available to themers.
 *
 *   It works by having one preprocess function for each template file or its
 *   derivatives (called template suggestions). For example:
 *     THEME_preprocess_page    alters the variables for page.tpl.php
 *     THEME_preprocess_node    alters the variables for node.tpl.php or
 *                              for node-forum.tpl.php
 *     THEME_preprocess_comment alters the variables for comment.tpl.php
 *     THEME_preprocess_block   alters the variables for block.tpl.php
 *
 *   For more information on preprocess functions and template suggestions,
 *   please visit the Theme Developer's Guide on Drupal.org:
 *   http://drupal.org/node/223440
 *   and http://drupal.org/node/190815#template-suggestions
 */

/**
 * Implementation of HOOK_theme().
 */
function nquire_laf_theme(&$existing, $type, $theme, $path) {
	$hooks = zen_theme($existing, $type, $theme, $path);
	// Add your theme hooks like this:
	/*
	  $hooks['hook_name_here'] = array( // Details go here );
	 */
	// @TODO: Needs detailed comments. Patches welcome!
	return $hooks;
}

/**
 * Override or insert variables into all templates.
 *
 * @param $vars
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered (name of the .tpl.php file.)
 */
/* -- Delete this line if you want to use this function
  function nquire_laf_preprocess(&$vars, $hook) {
  $vars['sample_variable'] = t('Lorem ipsum.');
  }
  // */

/**
 * Override or insert variables into the page templates.
 *
 * @param $vars
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("page" in this case.)
 */
/* -- Delete this line if you want to use this function
  function nquire_laf_preprocess_page(&$vars, $hook) {
  $vars['sample_variable'] = t('Lorem ipsum.');

  // To remove a class from $classes_array, use array_diff().
  //$vars['classes_array'] = array_diff($vars['classes_array'], array('class-to-remove'));
  }
  // */

/**
 * Override or insert variables into the node templates.
 *
 * @param $vars
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("node" in this case.)
 */
/* -- Delete this line if you want to use this function
  function nquire_laf_preprocess_node(&$vars, $hook) {
  $vars['sample_variable'] = t('Lorem ipsum.');

  // Optionally, run node-type-specific preprocess functions, like
  // nquire_laf_preprocess_node_page() or nquire_laf_preprocess_node_story().
  $function = __FUNCTION__ . '_' . $vars['node']->type;
  if (function_exists($function)) {
  $function($vars, $hook);
  }
  }
  // */

/**
 * Override or insert variables into the comment templates.
 *
 * @param $vars
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("comment" in this case.)
 */
/* -- Delete this line if you want to use this function
  function nquire_laf_preprocess_comment(&$vars, $hook) {
  $vars['sample_variable'] = t('Lorem ipsum.');
  }
  // */

/**
 * Override or insert variables into the block templates.
 *
 * @param $vars
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("block" in this case.)
 */
/* -- Delete this line if you want to use this function
  function nquire_laf_preprocess_block(&$vars, $hook) {
  $vars['sample_variable'] = t('Lorem ipsum.');
  }
  // */


$color_scheme = theme_get_setting('nquire_laf_color_scheme');
drupal_add_css(drupal_get_path('theme', 'nquire_laf') . '/css/color_scheme_' . ($color_scheme ? $color_scheme : 'original') . '.css', 'theme', 'all');

function _nquire_laf_color($key) {
	static $colors = array(
'a' => array('#b0b70f', '#eaeccb'),
 'b' => array('#0cadaa', '#d7ecea'),
 'c' => array('#00ace2', '#d5ebf8'),
 'd' => array('#756dab', '#e3e1ef'),
 'e' => array('#c4388a', '#f7d2e3'),
 'f' => array('#dc1340', '#f7d1c9'),
 'g' => array('#ea620d', '#fbd4b5'),
 'h' => array('#fcc724', '#feeec8'),
	);

	return $colors[$key];
}

function phptemplate_pi_inquiry_structure($node = NULL) {
	global $user;
	$inquiry_info = pi_info()->getInquiryInfo($node->nid);
	$inquiry_access = pi_info()->getAccessManager($inquiry_info->getInquiryNid(), $user->uid);
	$phases = $inquiry_info->getPhases();
	$phase_count = count($phases);

	if ($phase_count == 0) {
		return '';
	}

	$circle_radius = 60.0;
	$circle_stroke = 8.0;
	$circle_distance = 80.0;

	$aspect_ratio = sqrt(3. / 4.);
	$scale = .8;


	$net_radius = $phase_count * ($circle_radius + .5 * $circle_distance) / PI;
	$net_vertical_radius = $aspect_ratio * $net_radius;

	$net_center_x = $net_radius + $circle_radius + $circle_stroke;
	$net_center_y = $net_vertical_radius + $circle_radius + $circle_stroke;

	$net_width = 2 * $net_center_x;
	$net_height = 2 * $net_center_y;

	$svg_width = $scale * $net_width;
	$svg_height = $scale * $net_height;

	$inner_circle_radius = $circle_radius - $circle_stroke;
	$inner_circle_diameter = 2 * $inner_circle_radius;
	$outer_circle_radius = $circle_radius + .5 * $circle_stroke;

	$color_keys = _ag_phase_keys(count($phases));

	$svg = "<svg width='$svg_width' height='$svg_height' viewBox='0 0 $net_width $net_height' xmlns='http://www.w3.org/2000/svg'>";

	$i = 0;
	$a0 = PI / $phase_count;
	$points = array();
	foreach ($phases as $phase_nid => $phase_node) {
		$a = $a0 + 2 * ($i) * PI / $phase_count;
		$sin_a = sin($a);
		$cos_a = cos($a);

		$x = $net_center_x + $net_radius * $sin_a;
		$y = $net_center_y - $net_vertical_radius * $cos_a;
		$lx = $x - $outer_circle_radius * $sin_a;
		$ly = $y + $outer_circle_radius * $cos_a;

		$label = $inquiry_access->getAccessToItem($phase_node) === 'hidden' ? check_plain($phase_node->title) :
						l(check_plain($phase_node->title), 'phase/' . $phase_nid, array('attributes' => array('style' => 'color:black;text-decoration@none;')));

		$points[] = array(
				'x' => $x,
				'y' => $y,
				'lx' => $lx,
				'ly' => $ly,
				'label' => $label,
		);
		$i++;
	}

	for ($i = 0; $i < $phase_count - 1; $i++) {
		for ($j = $i + 2; $j < $phase_count; $j++) {
			if ($i !== 0 || $j !== $phase_count - 1) {
				$svg .= "<line stroke='#d2d3d4' id='line_{$i}_{$j}' x1='{$points[$i]['lx']}' y1='{$points[$i]['ly']}' x2='{$points[$j]['lx']}' y2='{$points[$j]['ly']}' stroke-linecap='round' stroke-linejoin='round' stroke-dasharray='6,12' stroke-width='4' fill='none'/>";
			}
		}
	}



	foreach ($points as $i => $point) {
		$tx = $point['x'] - $inner_circle_radius;
		$ty = $point['y'] - $inner_circle_radius;

		$cs = _nquire_laf_color($color_keys[$i % $phase_count]);
		$svg .= "<circle id='circle_phase_{$phase_nid}' r='$circle_radius' cy='{$point['y']}' cx='{$point['x']}' stroke-width='$circle_stroke' stroke='{$cs[0]}' fill='{$cs[1]}'/>";
		$svg .= "<foreignObject x='$tx' y='$ty' width='$inner_circle_diameter' height='$inner_circle_diameter'>"
						. "<div xmlns='http://www.w3.org/1999/xhtml' style='width:{$inner_circle_diameter}px;height:{$inner_circle_diameter}px;line-height:{$inner_circle_diameter}px;text-align: center;'>"
						. "<div class='nquire_diagram_text' style='font-family: sans-serif; white-space: pre-wrap;max-height: {$inner_circle_diameter}px;font-size:1.2em;font-weight: bold;border-radius:{$inner_circle_radius}px;display: inline-block; vertical-align: middle;line-height: normal;'>"
						. $point['label']
						. "</div>"
						. "</div>"
						. "</foreignObject>";
	}

	$arrow_length = 123.5;
	$arrow_angle = 15.5;
	$image_origin_x = 15;
	$image_origin_y = 45;
	$image_url = url(drupal_get_path('theme', 'nquire_laf') . '/images/nQuire_diagram_arrow.png');
	$image_width = 144;
	$image_height = 85;

	for ($i = 0; $i < $phase_count; $i++) {
		$p1 = $points[$i];
		$p2 = $points[($i + 1) % $phase_count];
		$a = atan2($p2['y'] - $p1['ly'], $p1['lx'] - $p2['x']);
		$p3x = $p2['x'] + $outer_circle_radius * cos($a);
		$p3y = $p2['y'] - $outer_circle_radius * sin($a);

		$dx = $p3x - $p1['lx'];
		$dy = $p3y - $p1['ly'];
		$d = sqrt($dx * $dx + $dy * $dy);
		$scale = $d / $arrow_length;

		$final_width = $image_width * $scale;
		$final_height = $image_height * $scale;

		$rotation = 180 - 180 * $a / PI + $arrow_angle;
		$image_x = $p1['lx'] - $scale * $image_origin_x;
		$image_y = $p1['ly'] - $scale * $image_origin_y;

		$svg .= "<image xlink:href='$image_url' height='$final_height' width='$final_width' y='$image_y' x='$image_x' transform='rotate($rotation {$p1['lx']} {$p1['ly']})'/>";
	}

	$svg .= '</svg>';

	$output = '<div><div style="max-width: 900px; max-height:512px;">' . $svg . '</div></div>';

	return $output;
}

function phptemplate_pi_activities_view_phase($data) {
	drupal_set_title($data['phase']['title']);

	if ($data['in_phase']) {
		$output = "<p>{$data['phase']['description']}</p><p><small>{$data['phase']['sharing']}</small></p>";
	}

	foreach ($data['activities'] as $activity_data) {
		$output .= '<div>' . theme('pi_activities_view_activity', $activity_data) . '</div>';
	}

	return $output;
}

function phptemplate_pi_activities_view_activity($activity_data) {

	$output = '<a name="' . $activity_data['node']->nid . '"></a>';

	if ($activity_data['content']['mode'] === 'activity_page') {
		$output .= $activity_data['content']['content'];
	} else {

		$output .= '<div class="phase_activity phase_activity_' . $activity_data['phase_key'] . '">';

		$output .= '<div class="phase_activity_title">' . $activity_data['title'] . '</div>';

		$output .= '<div class="phase_activity_content_wrapper">'
						. '<table class="phase_activity_table">'
						. '<tr><td class="phase_activity_label"><div>'
						. (strlen($activity_data['description']) > 0 ? t('Activity:') : '')
						. '</div></td><td class="phase_activity_content_cell phase_activity_description"><div>' . $activity_data['description'] . '</div>'
						. '</td></tr>';


		if ($activity_data['can_view'] && isset($activity_data['content']['mode'])) {
			switch ($activity_data['content']['mode']) {
				case 'twocolumns':
					foreach ($activity_data['content']['rows'] as $row) {
						if (is_array($row[1])) {
							$empty = $row[1]['empty'];
							$content = $row[1]['content'];
						} else {
							$empty = $row[1] === FALSE;
							$content = $empty ? '' : $row[1];
						}
						$content_class = $empty ? 'phase_activity_no_content' : 'phase_activity_content';

						$output .= '<tr><td class="phase_activity_label"><div>' . $row[0] . '</div></td><td class="phase_activity_content_cell"><div class="' . $content_class . '">' . $content . '</div></td></tr>';
					}
					$output .= '<tr><td class="phase_activity_label"></td><td class="phase_activity_link">' . $activity_data['links'] . '</td></tr>';
					break;
				case 'singleblock':
					$output .= '<tr><td colspan="2" class="phase_activity_content_cell"><div class="phase_activity_content">' . $activity_data['content']['content'] . '</div></td></tr>';
					$output .= '<tr><td colspan="2" class="phase_activity_link">' . $activity_data['links'] . '</td></tr>';
					break;
			}
		}

		$output .= '</table></div>';

		/* 					. '<div class="phase_activity_description phase_activity_metadata">' . $activity_data['description'] . '</div>'
		  . '<div class="phase_activity_access phase_activity_metadata">' . $activity_data['access_explanation'] . '</div>'
		  . '<div class="phase_activity_link">' .  . '</div>'; */



		$output .= '</div>';
	}

	return $output;
}

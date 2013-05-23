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


function phptemplate_pi_inquiry_structure($node = NULL) {
	global $user;
	$inquiry_info = pi_info()->getInquiryInfo($node->nid);
	$inquiry_access = pi_info()->getAccessManager($inquiry_info->getInquiryNid(), $user->uid);
	$phases = $inquiry_info->getPhases();
	$phase_count = count($phases);

	$width = 800;
	$height = 700;

	$cx = .5 * $width;
	$cy = .5 * $height;

	$rx = .7 * $cx;
	$ry = .7 * $cy;


	$radius = 70;
	$stroke = 8;
	$inner_radius = $radius - $stroke;
	$inner_diameter = 2 * $inner_radius;
	$outer_radius = $radius + .5 * $stroke;

	$colors = array(
			'a' => array('#b0b70f', '#eaeccb'),
			'b' => array('#0cadaa', '#d7ecea'),
			'c' => array('#00ace2', '#d5ebf8'),
			'd' => array('#756dab', '#e3e1ef'),
			'e' => array('#c4388a', '#f7d2e3'),
			'f' => array('#dc1340', '#f7d1c9'),
			'g' => array('#ea620d', '#fbd4b5'),
			'h' => array('#fcc724', '#feeec8'),
	);
	$color_keys = _ag_phase_colors(count($phases));

	$svg = "<svg width='100%' viewBox='0 0 $width $height' xmlns='http://www.w3.org/2000/svg'>";

	$i = 0;
	$a0 = PI / $phase_count;
	$points = array();
	foreach ($phases as $phase_nid => $phase_node) {
		$a = $a0 + 2 * ($i) * PI / $phase_count;
		$sin_a = sin($a);
		$cos_a = cos($a);

		$x = $cx + $rx * $sin_a;
		$y = $cy - $ry * $cos_a;
		$lx = $x - $outer_radius * $sin_a;
		$ly = $y + $outer_radius * $cos_a;

		$points[] = array(
				'x' => $x,
				'y' => $y,
				'lx' => $lx,
				'ly' => $ly,
				'label' => l(check_plain($phase_node->title), 'phase/' . $phase_nid, array('attributes' => array('style' => 'color:black'))),
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
		$tx = $point['x'] - $inner_radius;
		$ty = $point['y'] - $inner_radius;

		$cs = $colors[$color_keys[$i % $phase_count]];
		$svg .= "<circle id='circle_phase_{$phase_nid}' r='$radius' cy='{$point['y']}' cx='{$point['x']}' stroke-width='$stroke' stroke='{$cs[0]}' fill='{$cs[1]}'/>";
		$svg .= "<foreignObject x='$tx' y='$ty' width='$inner_diameter' height='$inner_diameter'>"
						. "<div xmlns='http://www.w3.org/1999/xhtml' style='width:{$inner_diameter}px;height:{$inner_diameter}px;line-height:{$inner_diameter}px;text-align: center;'>"
						. "<div style='white-space: pre-wrap;max-height: {$inner_diameter}px;font-size:1em;font-weight: bold;border-radius:{$inner_radius}px;display: inline-block; vertical-align: middle;line-height: normal;'>"
						. $point['label']
						. "</div>"
						. "</div>"
						. "</foreignObject>";
	}
	
	for ($i = 0; $i < $phase_count; $i++) {
		$p1 = $points[$i];
		$p2 = $points[($i + 1) % $phase_count];
		$a = atan2($p2['y'] - $p1['ly'], $p1['lx'] - $p2['x']);
		$p3x = $p2['x'] + $outer_radius * cos($a);
		$p3y = $p2['y'] - $outer_radius * sin($a);

		$svg .= "<line stroke='red' x1='{$p1['lx']}' y1='{$p1['ly']}' x2='{$p3x}' y2='{$p3y}' stroke-width='4' fill='none'/>";
	}

	$svg .= '</svg>';

	$output = '<div style="text-align: center"><div style="max-width: 900px; max-height:512px;">' . $svg . '</div></div>';

	return $output;
}

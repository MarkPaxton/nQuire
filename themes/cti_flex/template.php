<?php
// $Id: template.php,v 1.3 2010/10/18 17:37:36 aross Exp $

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
 */

// Add fluid or fixed width CSS file.
if (theme_get_setting('cti_flex_layout') == 'fixed') {
  drupal_add_css(drupal_get_path('theme', 'cti_flex') . '/css/layout-fixed.css', 'theme', 'all');
}
if (theme_get_setting('cti_flex_layout') == 'fluid') {
  drupal_add_css(drupal_get_path('theme', 'cti_flex') . '/css/layout-liquid.css', 'theme', 'all');
}

// Optionally add CSS files for the user-selected color design.

$design = theme_get_setting('cti_flex_design');
switch ($design) {
  case '0':
    drupal_add_css(drupal_get_path('theme', 'cti_flex') . '/colors/teal.css', 'theme', 'all');
    break;
  case '1':
    drupal_add_css(drupal_get_path('theme', 'cti_flex') . '/colors/blue.css', 'theme', 'all');
    break;
  case '2':
    drupal_add_css(drupal_get_path('theme', 'cti_flex') . '/colors/red.css', 'theme', 'all');
    break;
  case '3':
    drupal_add_css(drupal_get_path('theme', 'cti_flex') . '/colors/black.css', 'theme', 'all');
    break;
  case '4':
    break;
}

// Add local.css last so user can override any other settings
drupal_add_css(path_to_theme() . '/css/local.css', 'theme', 'all');

/**
 * Implementation of HOOK_theme().
 */
function cti_flex_theme(&$existing, $type, $theme, $path) {
  $hooks = zen_theme($existing, $type, $theme, $path);
  return $hooks;
}

/**
 * Override or insert variables into the page templates.
 *
 */

function cti_flex_preprocess_page(&$vars, $hook) {
  // Generate menu tree from source of primary links
  // Check for menu internationalization
  if (module_exists('i18nmenu')) {
    $vars['primary_links_tree'] = i18nmenu_translated_tree(variable_get('menu_primary_links_source', 'primary-links'));
  }
  else {
  $vars['primary_links_tree'] = menu_tree(variable_get('menu_primary_links_source', 'primary-links'));
  }

  // Include variable with custom font family
  $vars['custom_font']= '';
  $font = theme_get_setting('cti_flex_font_family');
  if ($font && $font != '0'){
    $vars['custom_font']= "<style type=\"text/css\">\n";
    if ($font == '1'){
      $vars['custom_font'].= "body {font-family: Arial, Helvetica, \"Bitstream Vera Sans\", sans-serif;}\n";
    }
    if ($font == '2'){
      $vars['custom_font'].= "body {font-family: \"Lucida Sans\", Verdana, Arial, sans-serif;}\n";
    }
    if ($font == '3'){
      $vars['custom_font'].= "body {font-family: Times, \"Times New Roman\", Georgia, \"Bitstream Vera Serif\", serif;}\n";
    }
    if ($font == '4'){
      $vars['custom_font'].= "body {font-family: Georgia, \"Times New Roman\", \"Bitstream Vera Serif\", serif;}\n";
    }
    if ($font == '5'){
      $vars['custom_font'].= "body {font-family: Verdana, Tahoma, Arial, Helvetica, \"Bitstream Vera Sans\", sans-serif;}\n";
    }
    if ($font == '6'){
      $vars['custom_font'].= "body {font-family: Tahoma, Verdana, Arial, Helvetica, \"Bitstream Vera Sans\", sans-serif;}\n";
    }
    $vars['custom_font'] .= "</style>";
  }
  $vars['custom_colors']= '';
  $color1 = theme_get_setting('cti_flex_color1');
  $color2 = theme_get_setting('cti_flex_color2');
  $color3 = theme_get_setting('cti_flex_color3');
  $color4_left = theme_get_setting('cti_flex_color4_left');
  $color4_lefthead = theme_get_setting('cti_flex_color4_lefthead');
  $color4_center = theme_get_setting('cti_flex_color4_center');
  $color4_centerhead = theme_get_setting('cti_flex_color4_centerhead');
  $color4_right = theme_get_setting('cti_flex_color4_right');
  $color4_righthead = theme_get_setting('cti_flex_color4_righthead');
  $color5 = theme_get_setting('cti_flex_color5');
  $color6 = theme_get_setting('cti_flex_color6');
  $color7 = theme_get_setting('cti_flex_color7');
  $color8 = theme_get_setting('cti_flex_color8');
  $color9_left = theme_get_setting('cti_flex_color9_left');
  $color9_center = theme_get_setting('cti_flex_color9_center');
  $color9_right = theme_get_setting('cti_flex_color9_right');
  
  // set CSS 
    $vars['custom_colors']= "<style type=\"text/css\">\n";
    if ($color1){
      $vars['custom_colors'].= "body {background: $color1 none;}\n";
    }
    if ($color2){
      $vars['custom_colors'].= "#header .section, #footer { background: $color2 none; }\n";
    }
    if ($color3){
      $vars['custom_colors'].= "#navigation, #suckerfishmenu ul.menu li ul a { background: $color3; border: $color3; }\n";
    }
    if ($color4_left){
      $vars['custom_colors'].= ".region-sidebar-first .block .content { background-color: $color4_left; border: 1px solid $color4_left; }\n";
    }
    if ($color4_lefthead){
      $vars['custom_colors'].= ".region-sidebar-first .block h2.title { background-color: $color4_lefthead; border: 1px solid $color4_lefthead; }\n";
    }
    if ($color4_center){
      $vars['custom_colors'].= "#content .block .content, #mission { background-color: $color4_center; border: 1px solid $color4_center; }\n";
    }
    if ($color4_centerhead){
      $vars['custom_colors'].= "#content .block h2.title { background-color: $color4_centerhead; border: 1px solid $color4_centerhead; }\n"; 
    }
    if ($color4_right){
      $vars['custom_colors'].= ".region-sidebar-second .block .content { background-color: $color4_right; border: 1px solid $color4_right; }\n";
    }
    if ($color4_righthead){
      $vars['custom_colors'].= ".region-sidebar-second .block h2.title { background-color: $color4_righthead; border: 1px solid $color4_righthead; }\n";
    }
    if ($color5){
      $vars['custom_colors'].= "#main a { color: $color5; }\n";
    }
    if ($color6){
      $vars['custom_colors'].= "#site-name a:link, #site-name a:visited, #site-slogan, #footer { color: $color6; }\n";
    }
    if ($color7){
      $vars['custom_colors'].= "#suckerfishmenu a { color: $color7; }\n";
    }
    if ($color8){
      $vars['custom_colors'].= "#secondary a { color: $color8; }\n";
    }
    if ($color9_left){
      $vars['custom_colors'].= ".region-sidebar-first .block h2.title, .region-sidebar-first .block h2.title a { color: $color9_left; }\n";
    }
    if ($color9_center){
      $vars['custom_colors'].= "#content .block h2.title, #content .block h2.title a { color: $color9_center; }\n";
    }
    if ($color9_right){
      $vars['custom_colors'].= ".region-sidebar-second .block h2.title, .region-sidebar-second .block h2.title a { color: $color9_right; }\n";
    }
    $vars['custom_colors'] .= "</style>";
}


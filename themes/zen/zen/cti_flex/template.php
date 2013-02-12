<?php
// $Id: template.php,v 1.1 2009/08/21 20:05:37 aross Exp $

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
 *   entirety and paste it here, changing the prefix from theme_ to STARTERKIT_.
 *   For example:
 *
 *     original: theme_breadcrumb()
 *     theme override: STARTERKIT_breadcrumb()
 *
 *   where STARTERKIT is the name of your sub-theme. For example, the
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


/*
 * Add any conditional stylesheets you will need for this sub-theme.
 *
 * To add stylesheets that ALWAYS need to be included, you should add them to
 * your .info file instead. Only use this section if you are including
 * stylesheets based on certain conditions.
 */

// Optionally add the fixed width CSS file.
if (theme_get_setting('cti_flex_fixed')) {
  drupal_add_css(path_to_theme() . '/fixed-layout.css', 'theme', 'all');
}

// Optionally add CSS files for the user-selected color design.

$design = theme_get_setting('cti_flex_design');
switch ($design) {
  case '0':
    drupal_add_css(path_to_theme() . '/colors/teal.css', 'theme', 'all'); 
    break;
  case '1':
    drupal_add_css(path_to_theme() . '/colors/blue.css', 'theme', 'all');
    break;
  case '2':
    drupal_add_css(path_to_theme() . '/colors/red.css', 'theme', 'all'); 
    break;
  case '3':
    break;
}

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
  $vars['primary_links_tree'] = menu_tree(variable_get('menu_primary_links_source', 'primary-links'));
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
  $color4 = theme_get_setting('cti_flex_color4');
  $color5 = theme_get_setting('cti_flex_color5');
  $color6 = theme_get_setting('cti_flex_color6');
  $color7 = theme_get_setting('cti_flex_color7');
  $color8 = theme_get_setting('cti_flex_color8');
  if ($color1 || $color2 || $color3 || $color4 || $color5){
    $vars['custom_colors']= "<style type=\"text/css\">\n";
    if ($color1){
      $vars['custom_colors'].= "body {background: $color1 none;}\n";
    }
    if ($color2){
      $vars['custom_colors'].= "#header-inner, #footer { background: $color2 none; }\n";
    }
    if ($color3){
      $vars['custom_colors'].= "#navbar, #sidebar-left .block h2.title, #sidebar-right .block h2.title { background: $color3; border: $color3; }\n";
    }
    if ($color4){
      $vars['custom_colors'].= "#sidebar-left .block .content, #sidebar-right .block .content { background-color: $color4; border: 1px solid $color4; }\n";
    }
    if ($color5){
      $vars['custom_colors'].= "#main a { color: $color5; }\n";
    }
    if ($color6){
      $vars['custom_colors'].= "#site-name a:link, #site-name a:visited, #site-slogan, #footer { color: $color6; }\n";
    }
    if ($color7){
      $vars['custom_colors'].= "#suckerfishmenu a, #sidebar-left .block h2.title, #sidebar-right .block h2.title, #sidebar-left .block h2.title a, #sidebar-right .block h2.title a{ color: $color7; }\n";
    }
    if ($color8){
      $vars['custom_colors'].= "#secondary a { color: $color8; }\n";
    }
    $vars['custom_colors'] .= "</style>";
  }
}






function phptemplate_filter_tips_more_info()
{
    return '';
}

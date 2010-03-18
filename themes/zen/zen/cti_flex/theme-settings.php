<?php
// $Id: theme-settings.php,v 1.1 2009/08/21 20:05:37 aross Exp $

// Include the definition of zen_settings() and zen_theme_get_default_settings().
include_once './' . drupal_get_path('theme', 'zen') . '/theme-settings.php';


/**
 * Implementation of THEMEHOOK_settings() function.
 *
 * @param $saved_settings
 *   An array of saved settings for this theme.
 * @return
 *   A form array.
 */
function cti_flex_settings($saved_settings) {

  // Get the default values from the .info file.
  $defaults = zen_theme_get_default_settings('cti_flex');

  // Merge the saved variables and their default values.
  $settings = array_merge($defaults, $saved_settings);

  /*
   * Create the form using Forms API: http://api.drupal.org/api/6
   */
  $form = array();
  $form['cti_flex_fixed'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Use fixed width for theme'),
    '#default_value' => $settings['cti_flex_fixed'],
    '#description'   => t('The theme will be centered and fixed at 960 pixels wide. If you do not select this, the layout will be fluid, full width.'),
  );

  $form['cti_flex_design'] = array(
    '#type'          => 'radios',
    '#title'         => t('Design'),
    '#default_value' => $settings['cti_flex_design'],
    '#options'       => array(
      '0' => t('Teal and Orange'),
      '1' => t('Blue and Green'),
      '2' => t('Red and Gray'),
      '4' => t('None (plain gray) - Select this option if using the color picker tool below'),
     ),    
    '#description'   => t('Select the specific site design you would like to use. Each design has its own style sheet in the "colors" directory of the theme.'),
  );

  $form['cti_flex_font_family'] = array(
    '#type'          => 'radios',
    '#title'         => t('Font Family'),
    '#default_value' => $settings['cti_flex_font_family'],
    '#options'       => array(
      '0' => t('None - set manually in stylesheet'),
      '1' => t('Arial, Helvetica, Bitstream Vera Sans, sans-serif'),
      '2' => t('Lucida Sans, Verdana, Arial, sans-serif'),
      '3' => t('Times, Times New Roman, Georgia, Bitstream Vera Serif, serif'),
      '4' => t('Georgia, Times New Roman, Bitstream Vera Serif, serif'),
      '5' => t('Verdana, Tahoma, Arial, Helvetica, Bitstream Vera Sans, sans-serif'),
      '6' => t('Tahoma, Verdana, Arial, Helvetica, Bitstream Vera Sans, sans-serif'),
     ),
    '#description'   => t('Select the font family to be used on the site.'),
  );

  $form['cti_flex_color1'] = array(
    '#type'          => (module_exists('colorpicker') ? 'colorpicker_' : '') . 'textfield',
    '#title'         => t('Select color for body background (outside of main content area)'),
    '#default_value' => $settings['cti_flex_color1'],
    '#description'   => t('Click on the eyedropper icon to use color picker.  Or enter an HTML color code.'),
    '#prefix'        => '<fieldset class="collapsible"><legend>Custom color settings</legend><br />These values will override any colors in your style sheets.  Leave blank to use defaults.<br /><b>Important:</b> You must have the <a href="http://drupal.org/project/colorpicker" target="_blank">Color Picker</a> module installed to use the color picker widget.<div style="margin: 20px 0 0 20px;"><b>BACKGROUND COLORS</b>',
  );

  $form['cti_flex_color2'] = array(
    '#type'          => (module_exists('colorpicker') ? 'colorpicker_' : '') . 'textfield',
    '#title'         => t('Select color for header and footer backgrounds'),
    '#default_value' => $settings['cti_flex_color2'],
    '#description'   => t('Click on the eyedropper icon to use color picker.  Or enter an HTML color code.'),
  );

 $form['cti_flex_color3'] = array(
    '#type'          => (module_exists('colorpicker') ? 'colorpicker_' : '') . 'textfield',
    '#title'         => t('Select color for main navigation bar and block header backgrounds'),
    '#default_value' => $settings['cti_flex_color3'],
    '#description'   => t('Click on the eyedropper icon to use color picker.  Or enter an HTML color code.'),
  );

  $form['cti_flex_color4'] = array(
    '#type'          => (module_exists('colorpicker') ? 'colorpicker_' : '') . 'textfield',
    '#title'         => t('Select color for block content background'),
    '#default_value' => $settings['cti_flex_color4'],
    '#description'   => t('Click on the eyedropper icon to use color picker.  Or enter an HTML color code.'),
  );

  $form['cti_flex_color5'] = array(
    '#type'          => (module_exists('colorpicker') ? 'colorpicker_' : '') . 'textfield',
    '#title'         => t('Select color for links'),
    '#default_value' => $settings['cti_flex_color5'],
    '#description'   => t('Click on the eyedropper icon to use color picker.  Or enter an HTML color code.'),
    '#prefix'        => t('<b>TEXT COLORS</b>')
  );

  $form['cti_flex_color6'] = array(
    '#type'          => (module_exists('colorpicker') ? 'colorpicker_' : '') . 'textfield',
    '#title'         => t('Select color for site name, site slogan and footer text'),
    '#default_value' => $settings['cti_flex_color6'],
    '#description'   => t('Click on the eyedropper icon to use color picker.  Or enter an HTML color code.'),
  );

  $form['cti_flex_color7'] = array(
    '#type'          => (module_exists('colorpicker') ? 'colorpicker_' : '') . 'textfield',
    '#title'         => t('Select color for primary menu links and block headers'),
    '#default_value' => $settings['cti_flex_color7'],
    '#description'   => t('Click on the eyedropper icon to use color picker.  Or enter an HTML color code.'),
  );

    $form['cti_flex_color8'] = array(
    '#type'          => (module_exists('colorpicker') ? 'colorpicker_' : '') . 'textfield',
    '#title'         => t('Select color for secondary menu links'),
    '#default_value' => $settings['cti_flex_color8'],
    '#description'   => t('Click on the eyedropper icon to use color picker.  Or enter an HTML color code.'),
    '#suffix'        => '</div></fieldset>',
  );

  // Add the base theme's settings.
  $form += zen_settings($saved_settings, $defaults);

  // Remove some of the base theme's settings. 
  unset($form['themedev']['zen_layout']); // We don't need to select the base stylesheet.

  // Return the form
  return $form;
}

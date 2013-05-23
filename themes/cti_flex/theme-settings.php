<?php
// $Id: theme-settings.php,v 1.2 2010/07/12 23:06:33 aross Exp $

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
  $form['cti_flex_layout'] = array(
    '#type'          => 'radios',
    '#title'         => t('Fixed or fluid width layout'),
    '#options'       => array(
      'fixed' => t('Fixed width layout (960 pixels wide)'),
      'fluid' => t('Fluid, full width layout'),
     ),
    '#default_value' => $settings['cti_flex_layout'],
  );

  $form['cti_flex_design'] = array(
    '#type'          => 'radios',
    '#title'         => t('Color Scheme'),
    '#default_value' => $settings['cti_flex_design'],
    '#options'       => array(
      '0' => t('Teal and Orange'),
      '1' => t('Blue and Green'),
      '2' => t('Red and Gray'),
      '3' => t('Black and Orange'),
      '4' => t('None (plain gray) - Select this option if using the color picker tool below to create your own custom color scheme'),
     ),
    '#description'   => t('Select the predesigned color scheme you would like to use. Each design has its own style sheet in the "colors" directory of the theme. 
Note: the predesigned schemes will not work with some of the custom color settings (below).'),
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

  $form['cti_flex_background'] = array(
    '#type' => 'fieldset',
    '#title' => t('Main background colors'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#prefix' => '<b>Custom Color Settings</b><br /><br />These values will override any colors in your style sheets.  Leave blank to use defaults.<br /><b>Important:</b> You must have the <a href="http://drupal.org/project/colorpicker" target="_blank">Color Picker</a> module installed to use the color picker widget.',
  );

  $form['cti_flex_background']['cti_flex_color1'] = array(
    '#type'          => (module_exists('colorpicker') ? 'colorpicker_' : '') . 'textfield',
    '#title'         => t('Select color for body background (outside of main content area)'),
    '#default_value' => $settings['cti_flex_color1'],
    '#description'   => t('Click on the eyedropper icon to use color picker.  Or enter an HTML color code.'),
 );

  $form['cti_flex_background']['cti_flex_color2'] = array(
    '#type'          => (module_exists('colorpicker') ? 'colorpicker_' : '') . 'textfield',
    '#title'         => t('Select color for header and footer backgrounds'),
    '#default_value' => $settings['cti_flex_color2'],
    '#description'   => t('Click on the eyedropper icon to use color picker.  Or enter an HTML color code.'),
  );

 $form['cti_flex_background']['cti_flex_color3'] = array(
    '#type'          => (module_exists('colorpicker') ? 'colorpicker_' : '') . 'textfield',
    '#title'         => t('Select color for main navigation bar'),
    '#default_value' => $settings['cti_flex_color3'],
    '#description'   => t('Click on the eyedropper icon to use color picker.  Or enter an HTML color code.'),
  );

  $form['cti_flex_blocks'] = array(
    '#type' => 'fieldset',
    '#title' => t('Block background colors'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['cti_flex_blocks']['cti_flex_color4_left'] = array(
    '#type'          => (module_exists('colorpicker') ? 'colorpicker_' : '') . 'textfield',
    '#title'         => t('Select color for left column block content background'),
    '#default_value' => $settings['cti_flex_color4_left'],
    '#description'   => t('Click on the eyedropper icon to use color picker.  Or enter an HTML color code.'),
    '#prefix'        => t('<b>BLOCKS</b>')
  );

  $form['cti_flex_blocks']['cti_flex_color4_lefthead'] = array(
    '#type'          => (module_exists('colorpicker') ? 'colorpicker_' : '') . 'textfield',
    '#title'         => t('Select color for left column block header background'),
    '#default_value' => $settings['cti_flex_color4_lefthead'],
    '#description'   => t('Click on the eyedropper icon to use color picker.  Or enter an HTML color code.'),
  );

  $form['cti_flex_blocks']['cti_flex_color4_center'] = array(
    '#type'          => (module_exists('colorpicker') ? 'colorpicker_' : '') . 'textfield',
    '#title'         => t('Select color for center region block content background'),
    '#default_value' => $settings['cti_flex_color4_center'],
    '#description'   => t('Click on the eyedropper icon to use color picker.  Or enter an HTML color code.'),
  );

  $form['cti_flex_blocks']['cti_flex_color4_centerhead'] = array(
    '#type'          => (module_exists('colorpicker') ? 'colorpicker_' : '') . 'textfield',
    '#title'         => t('Select color for center region block header background'),
    '#default_value' => $settings['cti_flex_color4_centerhead'],
    '#description'   => t('Click on the eyedropper icon to use color picker.  Or enter an HTML color code.'),
  );

  $form['cti_flex_blocks']['cti_flex_color4_right'] = array(
    '#type'          => (module_exists('colorpicker') ? 'colorpicker_' : '') . 'textfield',
    '#title'         => t('Select color for right column block content background'),
    '#default_value' => $settings['cti_flex_color4_right'],
    '#description'   => t('Click on the eyedropper icon to use color picker.  Or enter an HTML color code.'),
  );

  $form['cti_flex_blocks']['cti_flex_color4_righthead'] = array(
    '#type'          => (module_exists('colorpicker') ? 'colorpicker_' : '') . 'textfield',
    '#title'         => t('Select color for right column block header background'),
    '#default_value' => $settings['cti_flex_color4_righthead'],
    '#description'   => t('Click on the eyedropper icon to use color picker.  Or enter an HTML color code.'),
  );

  $form['cti_flex_text'] = array(
    '#type' => 'fieldset',
    '#title' => t('Text colors'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['cti_flex_text']['cti_flex_color5'] = array(
    '#type'          => (module_exists('colorpicker') ? 'colorpicker_' : '') . 'textfield',
    '#title'         => t('Select color for links'),
    '#default_value' => $settings['cti_flex_color5'],
    '#description'   => t('Click on the eyedropper icon to use color picker.  Or enter an HTML color code.'),
    '#prefix'        => t('<b>TEXT COLORS</b>')
  );

  $form['cti_flex_text']['cti_flex_color6'] = array(
    '#type'          => (module_exists('colorpicker') ? 'colorpicker_' : '') . 'textfield',
    '#title'         => t('Select color for site name, site slogan and footer text'),
    '#default_value' => $settings['cti_flex_color6'],
    '#description'   => t('Click on the eyedropper icon to use color picker.  Or enter an HTML color code.'),
  );

  $form['cti_flex_text']['cti_flex_color7'] = array(
    '#type'          => (module_exists('colorpicker') ? 'colorpicker_' : '') . 'textfield',
    '#title'         => t('Select color for primary menu links'),
    '#default_value' => $settings['cti_flex_color7'],
    '#description'   => t('Click on the eyedropper icon to use color picker.  Or enter an HTML color code.'),
  );

    $form['cti_flex_text']['cti_flex_color8'] = array(
    '#type'          => (module_exists('colorpicker') ? 'colorpicker_' : '') . 'textfield',
    '#title'         => t('Select color for secondary menu links'),
    '#default_value' => $settings['cti_flex_color8'],
    '#description'   => t('Click on the eyedropper icon to use color picker.  Or enter an HTML color code.'),
  );

  $form['cti_flex_text']['cti_flex_color9_left'] = array(
    '#type'          => (module_exists('colorpicker') ? 'colorpicker_' : '') . 'textfield',
    '#title'         => t('Select color for left column block header text'),
    '#default_value' => $settings['cti_flex_color9_left'],
    '#description'   => t('Click on the eyedropper icon to use color picker.  Or enter an HTML color code.'),
  );

  $form['cti_flex_text']['cti_flex_color9_center'] = array(
    '#type'          => (module_exists('colorpicker') ? 'colorpicker_' : '') . 'textfield',
    '#title'         => t('Select color for center region block header text'),
    '#default_value' => $settings['cti_flex_color9_center'],
    '#description'   => t('Click on the eyedropper icon to use color picker.  Or enter an HTML color code.'),
  );

  $form['cti_flex_text']['cti_flex_color9_right'] = array(
    '#type'          => (module_exists('colorpicker') ? 'colorpicker_' : '') . 'textfield',
    '#title'         => t('Select color for right column block header text'),
    '#default_value' => $settings['cti_flex_color9_right'],
    '#description'   => t('Click on the eyedropper icon to use color picker.  Or enter an HTML color code.'),
  );

  // Add the base theme's settings.
  $form += zen_settings($saved_settings, $defaults);

  // Remove some of the base theme's settings.
  unset($form['themedev']['zen_layout']); // We don't need to select the base stylesheet.

  // Return the form
  return $form;
}

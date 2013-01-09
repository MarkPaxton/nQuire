<?php

/**
 * @file
 * Settings of the simplemenu.
 */

/**
 * SimpleMenu settings page.
 */
function simplemenu_admin_settings() {
  $simplemenu_path = drupal_get_path('module', 'simplemenu');

  if (variable_get('file_downloads', FILE_DOWNLOADS_PUBLIC) != FILE_DOWNLOADS_PUBLIC) {
    $form['private_warning'] = array(
      '#value' => '<p style="color: red; font-weight: bold;">'.
        t('WARNING: Your !file_system is set to Private. Although the menu will work, many of the options offered here will not. The JavaScript caching of the menu will be ignored. The CSS can also be changed dynamically from this form. In Private mode, unless you have Chaos Tools (ctools) installed, the default simplemenu/simplemenu.css file is used instead.',
          array('!file_system' => l('file system', 'admin/settings/file-system')))
        . '</p>',
    );
  }

  // menu selection
  $form['default_menu'] = array(
    '#type' => 'fieldset',
    '#title' => t('Menu settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  if (module_exists('menu')) {
    $form['default_menu']['simplemenu_menu'] = array(
      '#type' => 'select',
      '#title' => t('Menu'),
      '#options' => menu_parent_options(menu_get_menus(), array('mlid' => 0)), // return complete tree
      '#default_value' => variable_get('simplemenu_menu', 'navigation:0'),
      '#description' => t('Select the menu to display.'),
      '#weight' => -1,
    );
  }

  $themes = file_scan_directory($simplemenu_path . '/themes', '.*',
    array('.', '..', 'CVS', '.svn'), 0, FALSE, 'basename');
  $theme_selection = array('custom' => 'custom');
  foreach ($themes as $name => $ignore) {
    $theme_selection[$name] = $name;
  }
  $form['default_menu']['simplemenu_theme'] = array(
    '#type' => 'select',
    '#title' => t('Theme'),
    '#options' => $theme_selection,
    '#default_value' => variable_get('simplemenu_theme', 'original'),
    '#description' => t('Select which theme to use. If you specify custom, you need to define CSS in your theme.'),
  );

  $form['default_menu']['simplemenu_hide_when_empty'] = array(
    '#type' => 'checkbox',
    '#title' => t('Hide when empty'),
    '#default_value' => variable_get('simplemenu_hide_when_empty', FALSE),
    '#description' => t('Hide the whole menubar when empty. This is quite handy for when you have users who do not have permission to see any one menu items of the selected menus. When this flag is not set, you see a message letting you know that Simplemenu could not display a menu.'),
  );


  // standard settings
  $form['settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Simplemenu settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $fix_options = array(
    'scroll' => t('Scroll with page'),
    'top' => t('Fix at the Top (Forces Body, Prepend)'),
    //'bottom' => t('Fix at the Bottom (Forces Body, Append)'), -- this requires another CSS... hmmm...
  );
  $form['settings']['simplemenu_fix'] = array(
    '#type' => 'radios',
    '#title' => t('Scroll or fix menu'),
    '#options' => $fix_options,
    '#default_value' => variable_get('simplemenu_fix', 'scroll'),
    '#description' => t('Select the mode to use. The default is to let the menu scroll with the page.')
      . '<br /><span style="color: red;">' . t('WARNING') . ':</span> '
      . t('The At the Top/Bottom options prevent you from ever seeing the bottom of your drop-down menus. In other words, if you have many modules installed, it is not unlikely that your <em>Site configuration</em> menu will not fit the screen and the last few entries won\'t be accessible via Simplemenu.'),
  );

  $form['settings']['simplemenu_hide_delay'] = array(
    '#type' => 'textfield',
    '#title' => t('Hide delay'),
    '#size' => 4,
    '#default_value' => variable_get('simplemenu_hide_delay', 800),
    '#description' => t('How long (in milliseconds) should a menu still appear after losing focus.'),
  );

  $form['settings']['simplemenu_effect'] = array(
    '#type' => 'radios',
    '#title' => t('Show effect'),
    '#options' => array(
      'opacity' => t('Fade'),
      'height' => t('Slide'),
      'none' => t('None')
    ),
    '#default_value' => variable_get('simplemenu_effect', 'opacity'),
    '#description' => t('The effect used when displaying a menu.'),
  );

  $form['settings']['simplemenu_effect_speed'] = array(
    '#type' => 'radios',
    '#title' => t('Show speed'),
    '#options' => array('slow' => t('Slow'), 'medium' => t('Medium'), 'fast' => t('Fast'), '1' => t('Instantaneous')),
    '#default_value' => variable_get('simplemenu_effect_speed', 'fast'),
    '#description' => t('The speed of the effect, not used when "none" is set to show effect.'),
  );


  // advanced options
  $form['advanced'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['advanced']['simplemenu_uid1'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show to User ID 1'),
    '#description' => t('Check this option to enable simplemenu for user 1 (superuser/administration). This is useful if you want to use a different menu (such as admin_menu) for the superuser/admin and simplemenu for others.'),
    '#default_value' => variable_get('simplemenu_uid1', 1),
  );

  $superfish_js = file_scan_directory($simplemenu_path, '^superfish-[0-9.]*\.js$',
    array('.', '..', 'CVS', '.svn'), 0, FALSE, 'basename');
  $superfish = array('custom' => 'custom or theme');
  foreach ($superfish_js as $name => $ignore) {
    $superfish[$name] = $name;
  }
  $form['advanced']['simplemenu_superfish_version'] = array(
    '#type' => 'select',
    '#title' => t('SuperFish Version'),
    '#options' => $superfish,
    '#description' => t('Select which version of SuperFish you prefer using. The choice "custom or theme" means that Simplemenu does not include one of its own version of Superfish. It is expected that another module or your theme does so already.'),
    '#default_value' => variable_get('simplemenu_superfish_version', 'superfish-1.4.1.js'),
  );

  $scope = array(
    'header' => 'Header',
    'footer' => 'Footer',
  );
  $form['advanced']['simplemenu_menu_scope'] = array(
    '#type' => 'select',
    '#title' => t('Scope of simplemenu variable'),
    '#options' => $scope,
    '#description' => t('By default, the <code>simplemenu</code> variable is put in the footer (backward compatible.) It is possible to put it in the header instead.'),
    '#default_value' => variable_get('simplemenu_menu_scope', 'footer'),
  );

  $form['advanced']['simplemenu_cache_menu'] = array(
    '#type' => 'checkbox',
    '#title' => t('Cache the simplemenu variable'),
    '#description' => t('The Simplemenu now has the capability to cache the simplemenu variable in a .js file. This accelerate the transfer by using the Browser cache.'),
    '#default_value' => variable_get('simplemenu_cache_menu', TRUE),
  );

  $form['advanced']['simplemenu_menubar_zindex'] = array(
    '#type' => 'textfield',
    '#title' => t('Menubar CSS z-index value'),
    '#description' => t('By default, the menubar CSS z-index is set to 9999. Some themes or other modules may require you to change this value. Use -1 to completely disable the z-index in the menubar. If this value is not set to -1, then the following z-index will not have any effect and can as well be set to -1.'),
    '#default_value' => variable_get('simplemenu_menubar_zindex', 9999),
  );

  $form['advanced']['simplemenu_dropdown_zindex'] = array(
    '#type' => 'textfield',
    '#title' => t('Dropdown CSS z-index value'),
    '#description' => t('By default, the dropdown CSS z-index is set to 9999. However, some themes and modules use an even larger z-index. For instance, the AddThis overlay is put at z-index 100,000 (although from my tests, it seems that they use a much higher z-index...). So if you want the Simplemenu to appear over the AddThis pop-up, you want to use a z-index which is even larger (i.e. 2,000,000 [do not enter the commas!].) On the other hand, 9999 may be too large for your site. You can use a smaller number if that works better for you. Use -1 to not remove the z-index from your dropdown.'),
    '#default_value' => variable_get('simplemenu_dropdown_zindex', 9999),
  );

  $form['advanced']['simplemenu_element'] = array(
    '#type' => 'textfield',
    '#title' => t('CSS selector to attach menu to'),
    '#default_value' => variable_get('simplemenu_element', 'body'),
    '#description' => t('A valid CSS selector to attach the menu to. <em>Example: body, #primary, div.my-class</em>'),
    '#required' => TRUE,
  );

  $form['advanced']['simplemenu_element_method'] = array(
    '#type' => 'radios',
    '#title' => t('Attach method'),
    '#options' => array(
      'prepend' => t('Prepend'),
      'append' => t('Append'),
      'replace' => t('Replace'),
    ),
    '#default_value' => variable_get('simplemenu_element_method', 'prepend'),
    '#description' => t('Choose how the menu should be attached to the above selector.<br /><span style="color: red;">WARNING:</span> The Replace option should only be used with a specialized theme that offers a tag that is to be replaced by the simple menu. Make sure you don\'t use that option with your body!'),
    '#required' => TRUE,
  );

  // when someone has many themes, this list grows big!
  $themes = list_themes();
  $use_list = count($themes) > 15;
  $form['advanced']['simplemenu_exclusions'] = array(
    '#type' => $use_list ? 'select' : 'checkboxes',
    '#title' => t('Theme exclusions'),
    '#options' => drupal_map_assoc(array_keys($themes)),
    '#multiple' => $use_list,
    '#default_value' => variable_get('simplemenu_exclusions', array()),
    '#description' => t('Select which themes to <strong>not</strong> display the menu. Use this when you have a theme that displays its own admin navigation.'),
  );

  $form['advanced']['simplemenu_detect_popup'] = array(
    '#type' => 'checkbox',
    '#title' => t('Detect pop-up windows'),
    '#default_value' => variable_get('simplemenu_detect_popup', 1),
    '#description' => t("Choose whether SimpleMenu should attempt to detect if it is inside of a pop-up window.  If enabled, SimpleMenu will not display if it is inside of a pop-up window."),
  );

  $form['advanced']['simplemenu_call_theme'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow Simplemenu to call the theme() function'),
    '#description' => t('When a module calls theme() from their hook_init() function, it can break any other module that attempts to set the theme to something else than the default. By default, Simplemenu avoids the theme() function altogether meaning that its menu items may not look right in your theme, but the menu works. This flag is always ignored on the block list page.')
      . '<br /><br />'
      . t('IMPORTANT NOTE: SimpleMenu Inactive Parent Menu uses the theme() function and thus requires this flag to be set.'),
    '#default_value' => variable_get('simplemenu_call_theme', FALSE),
  );

  $form['advanced']['simplemenu_visibility_operator'] = array(
    '#type' => 'radios',
    '#title' => t('Show block on specific pages'),
    '#default_value' => variable_get('simplemenu_visibility_operator', 0),
    '#options' => array(
      0 => t('Show on every page except the listed pages.'),
      1 => t('Show on only the listed pages.'),
    ),
  );

  $form['advanced']['simplemenu_visibility_pages'] = array(
    '#type' => 'textarea',
    '#title' => t('Pages'),
    '#default_value' => variable_get('simplemenu_visibility_pages', ''),
    '#description' => t("Enter one page per line as Drupal paths. The '*' character is a wildcard. Example paths are %blog for the blog page and %blog-wildcard for every personal blog. %front is the front page.",
      array('%blog' => 'blog', '%blog-wildcard' => 'blog/*', '%front' => '<front>')),
    '#wysiwyg' => FALSE,
  );

  $form['#validate'][] = 'simplemenu_admin_settings_validate';
  $form['#submit'][] = 'simplemenu_admin_settings_submit';

  return system_settings_form($form);
}

/**
 * Verify that we have settings that are sensical.
 */
function simplemenu_admin_settings_validate($form, &$form_state) {
  $values = &$form_state['values'];
  if ($values['simplemenu_fix'] != 'scroll' && $values['simplemenu_menubar_zindex'] < 1) {
    form_set_error('simplemenu_menubar_zindex', t('In order to use a Fix mode, you want to increase the menubar z-index value to 1 or more.'));
  }
}

/**
 * Handle some special cases.
 */
function simplemenu_admin_settings_submit($form, $form_state) {
  // make sure we regenerate the CSS file
  variable_del('simplemenu_css_filename');
}

// vim: ts=2 sw=2 et syntax=php

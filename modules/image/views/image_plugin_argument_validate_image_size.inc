<?php
// $Id: image_plugin_argument_validate_image_size.inc,v 1.1 2009/05/01 22:29:47 sun Exp $

/**
 * @file
 * Views validation plugin for image size argument.
 */

/**
 * Validation handler for image size.
 *
 * Validates an image size argument and allows to restrict which image sizes
 * are valid.
 */
class image_plugin_argument_validate_image_size extends views_plugin_argument_validate {
  var $image_sizes = array();

  function init(&$view, &$argument, $id = NULL) {
    parent::init($view, $argument, $id);
    $this->image_sizes = image_get_sizes();
  }

  function validate_form(&$form, &$form_state) {
    $image_size_options = array();
    foreach ($this->image_sizes as $key => $size) {
      $image_size_options[$key] = $size['label'];
    }

    $form['image_size'] = array(
      '#type' => 'select',
      '#title' => t('Image sizes'),
      '#options' => $image_size_options,
      '#default_value' => $this->get_argument(),
      '#multiple' => TRUE,
      '#required' => TRUE,
      '#process' => array('views_process_dependency'),
      '#dependency' => array('edit-options-validate-type' => array($this->id)),
      '#description' => t('Which image sizes are allowed to be passed through this argument.'),
    );
  }

  function get_argument() {
    return isset($this->argument->options['image_size']) ? $this->argument->options['image_size'] : array_keys($this->image_sizes);
  }

  function validate_argument($argument) {
    return $this->argument->options['image_size'][$argument];
  }
}


<?php
// $Id: quiz.views_default.inc,v 1.1.2.4 2009/05/21 19:02:12 sivaji Exp $

/*
 * @file
 * Handles Default views.
 * // Declare all the .view files in the views subdir that end in .view
 */

function quiz_views_default_views() {
  $files = file_scan_directory(QUIZ_VIEWS_DIR . '/defaults', '.view');

  $views = array();
  foreach ($files as $path => $file) {
    require $path;
    $views[$file->name] = $view;
  }
  return $views;
}

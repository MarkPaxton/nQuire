<?php
// $Id: quiz_question.theme.inc,v 1.2.2.11 2010/11/05 16:14:48 falcon Exp $

/*
 * @file
 * Themes for the quiz question module.
 */

/**
 * Theme the question creation form
 *
 * @param $form
 *  Question creation form
 */
function theme_quiz_question_creation_form($form) {
  module_load_include('inc', 'quiz', 'quiz.admin');
  quiz_set_auto_title();
}

/**
 * Theme the question navigation form(Back, Next, Skip buttons...)
 *
 * @param $form
 *  Form holding the navigation buttons
 */
function theme_quiz_question_navigation_form($form) {
  return drupal_render($form);
}
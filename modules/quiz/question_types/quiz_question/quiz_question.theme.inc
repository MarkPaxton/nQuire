<?php
// $Id: quiz_question.theme.inc,v 1.1.2.2 2009/05/21 19:02:13 sivaji Exp $

/*
 * @file
 * Themes for the quiz question module.
 */

/**
 * Implementation of theme_$type_report().
 * Theme the feedback report for a quiz. This report is generated (typically) at the end of the quiz.
 */
function theme_quiz_question_report($question, $show_points, $show_feedback) {

  if (!isset($question->answers[0])) {
    drupal_set_message('No result could be calculated.', 'status');
    return;
  }

  $answer = $question->answers[0];
  $types = _quiz_get_question_types();
  $constructor = $types[$question->type]['response provider'];
  $result = new $constructor($answer['result_id'], $question);

  return $result->formatReport();
}

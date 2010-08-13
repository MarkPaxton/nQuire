<?php
// $Id: long_answer.theme.inc,v 1.1.2.2 2009/12/09 10:44:19 sivaji Exp $

/**
 * @file
 * Theme functions for long_answer.
 */
 
/**
 * Implementation of theme_$type_report().
 * Theme the feedback report for a quiz. This report is generated (typically) at the end of the quiz.
 */
function theme_long_answer_report($question, $show_points, $show_feedback) {
  if (is_array($question)) {
    // Cast this if it is an assoc array.
    $question = (object)$question;
  }
  
  $slug = '<div class="quiz_summary_question"><span class="quiz_question_bullet">'. t('Q:') . '</span> '. 
    check_markup($question->body) .
    '</div>';
    
  $result = '<div class="quiz_answer_feedback">';
  if ($question && !empty($question->answers)) {
    $answer = (object) current($question->answers);
    
    if ($answer->is_evaluated == 1) {
      // Show score:
      if ($show_points) {
        $args = array('@yours' => $answer->score, '@total' => $question->maximum_score);
        $result .= t('Score: @yours of @total possible points', $args);
      }
      
      // Show feedback, if any.
      if ($show_feedback && !empty($answer->feedback)) {
        $result .= '</div><div class="quiz_answer_feedback">' . $answer->feedback;
      }
      
    }
    else {
      $result .= t('This answer has not yet been scored.') . 
        '<br/>' . 
        t('Until the answer is scored, the total score will not be correct.');
    }
    
    if (user_access('score long answer')) {
      $path = sprintf('admin/quiz/score-long-answer/%s/%s', $question->vid, $answer->result_id);
      $result .= '<p>' . l(t('Score this answer'), $path) . '</p>';
    }
  }
  else {
    $result .= t('This question was not answered.');
  }
  $result .= '</div>';
  
  return $slug . $result;
}

/**
 * Implementation of theme_$type_feedback().
 * This is basically useless for long answer questions.
 */
function theme_long_answer_feedback($quiz, $report) {
  $output = '<div class="quiz_summary_text"><strong>' . t('Q:') . '</strong>'. 
    check_markup($report->body, $report->format) .
    '<br /><em>'.
    t('The answer to this question will be scored by hand.')
    . '</em></div>';
  return $output;
}

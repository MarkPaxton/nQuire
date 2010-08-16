<?php
// $Id: short_answer.theme.inc,v 1.1.2.3 2009/12/09 10:44:19 sivaji Exp $

/**
 * @file
 * Theme functions for short_answer.
 */
 
/**
 * Implementation of theme_$type_report().
 * Theme the feedback report for a quiz. This report is generated (typically) at the end of the quiz.
 */
function theme_short_answer_report($question, $show_points, $show_feedback) {
  if (is_array($question)) {
    // Cast this if it is an assoc array.
    $question = (object)$question;
  }
  
  $slug = '<div class="quiz_summary_question"><span class="quiz_question_bullet">'. t('Q:') . '</span> '. 
    check_markup($question->body, $question->format) .
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
        $result .= '</div><div class="quiz_answer_feedback">' . check_markup($answer->feedback, $question->format);
      }
      
    }
    else {
      $result .= t('This answer has not yet been scored.') . 
        '<br/>' . 
        t('Until the answer is scored, the total score will not be correct.');
      
      // Add scoring link for admins.
      if (user_access('score short answer')) {
        $path = sprintf('admin/quiz/score-short-answer/%s/%s', $question->vid, $answer->result_id);
        $result .= '<p>' . l(t('Score this answer'), $path) . '</p>';
      }
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
function theme_short_answer_feedback($quiz, $report) {
  $output = '<div class="quiz_summary_text"><strong>' . t('Q:') . '</strong>'. 
    check_markup($report->body, $report->format) .
    '<br /><em>';
  if ($report->is_evaluated) {
    $output .= 'Score: ' . $report->score;
  }
  else {
    t('The answer to this question will be scored by hand.');
  }
    
  $output .= '</em></div>';
  return $output;
}





<?php
// $Id: quiz.pages.inc,v 1.2.2.21 2010/01/03 17:37:56 sivaji Exp $

/**
 * User pages.
 * @file
 */

/*
 * Quiz Results User.
 */
function quiz_user_results($result_id) {
  global $user;

  $sql = 'SELECT qnp.nid, qnrs.uid
    FROM {quiz_node_properties} qnp
    INNER JOIN {quiz_node_results} qnrs ON qnrs.nid = qnp.nid
    WHERE qnrs.result_id = %d';
  $result = db_fetch_object(db_query( $sql, $result_id));
  if ($result->nid) {

    // User can view own results (quiz_menu sets access to 'own results').
    // User with role 'user results' can view other user's results.
    if ($result->uid != $user->uid && !user_access('view user results')) {
      drupal_access_denied();
      return;
    }

    $quiz = node_load($result->nid);
    $questions = _quiz_get_answers($result_id);
    $score = quiz_calculate_score($quiz, $result_id);
    $summary = _quiz_get_summary_text($quiz, $score);
    return theme('quiz_user_summary', $quiz, $questions, $score, $summary);
  }
  else {
    drupal_not_found();
  }
}

/**
 * Displays all the quizzes the user has taken part in.
 *
 * @return
 *  HTML output for page.
 */
function quiz_get_user_results($user_id) {
  global $user;

  if ($user_id == $user->uid || user_access('view user results')) {
    $results = array();
    $sql = "SELECT n.nid, n.title, u.name, qnrs.result_id, qnrs.time_start, qnrs.time_end
      FROM {node} n
      INNER JOIN {quiz_node_properties} qnp ON n.nid = qnp.nid
      INNER JOIN {quiz_node_results} qnrs ON qnrs.nid = qnp.nid
      INNER JOIN {users} u ON u.uid = qnrs.uid
      WHERE n.type = 'quiz'
        AND u.uid = %d
      ORDER BY qnrs.result_id ASC";

    $dbresult = db_query($sql, $user_id);
    // Create an array out of the results.
    while ($line = db_fetch_array($dbresult)) {
     $results[$line['result_id']] = $line;
    }
    return theme('quiz_get_user_results', $results);
  }
  else {
    return 'You have no permission to view the results of this user.';
  }
}

// THEME FUNCTIONS

/**
 * Theme a message about the quiz's availability for quiz takers.
 *
 * @ingroup themeable
 */
function theme_quiz_availability($node) {
  $status = _quiz_availability($node);
  $output = '<div class="quiz_availability"><p>';
  switch ($status) {
    case 'future':
      $output .= t('This quiz will not be available until %time.', array('%time' => format_date($node->quiz_open)));
      break;
    case 'open':
      $output .= t('This quiz closes %time.', array('%time' => format_date($node->quiz_close)));
      break;
    case 'closed':
      $output .= t('This quiz is no longer available.');
      break;
  }
  $output .= '</p></div>'."\n";
  return $output;
}


/**
 * Theme the node view for quizzes.
 *
 * @ingroup themeable
 */
function theme_quiz_view($node, $teaser = FALSE, $page = FALSE) {
  $output = '';
  // Output quiz options.
  $output .= '<h3>'. t('@quiz Options', array('@quiz' => QUIZ_NAME)) .'</h3>';
  $header = array(
    t('# of Random Questions'),
    t('Shuffle?'),
    t('Feedback'),
    t('Number of takes'),
  );
  $shuffle = $node->shuffle == 1 ? t('Yes') : t('No');
  $feedback_options = _quiz_get_feedback_options();
  $feedback = $feedback_options[$node->feedback_time];
  $takes = $node->takes == 0 ? t('Unlimited') : check_plain($node->takes);
  $rows = array();
  $rows[] = array(
    check_plain($node->number_of_random_questions),
    $shuffle,
    $feedback,
    $takes,
  );
  $output .= theme('table', $header, $rows);
  // Format Quiz Dates.
  $output .= '<h3>'. t('@quiz start/end', array('@quiz' => QUIZ_NAME)) .'</h3>';
  if (!$node->quiz_always) {
    // If we are previewing, make sure the dates are timestamps and not form arrays.
    if (is_array($node->quiz_open)) {
      quiz_translate_form_date($node, 'quiz_open');
    }
    if (is_array($node->quiz_close)) {
      quiz_translate_form_date($node, 'quiz_close');
    }

    // Format the availability info.
    $output .= '<p>'. format_date($node->quiz_open) .' &mdash; '. format_date($node->quiz_close) .'</p>';
    $output .= '<p><strong>'. t('Days @quiz live for: ', array('@quiz' => QUIZ_NAME))  .'</strong> '. floor(($node->quiz_close - $node->quiz_open) / 60 / 60 / 24) .'</p>';
    $remaining = floor(($node->quiz_close - time()) / 60 / 60 / 24);
    $remaining = ($remaining < 0) ? t('Expired') : $remaining;
    $output .= '<p><strong>'. t('Days remaining:') . '</strong> '. $remaining .'</p>';
    $elapsed = floor((time() - $node->quiz_open) / 60 / 60 / 24);
    $elapsed = ($elapsed < 0)?(-$elapsed) . t(' days to go') : $elapsed;
    $output .= '<p><strong>' . t('Days since start:') . '</strong> '. $elapsed .'</p>';
  }
  else {
    $output .= '<p>'. t('This Quiz is always available.') .'</p>'."\n";
  }

  // Format taxonomy selection (if applicable).
  if (function_exists('taxonomy_node_get_terms')) {
    $output .= '<h3>'. t('Taxonomy selection') .'</h3>';
    $terms = array();
    foreach (taxonomy_node_get_terms($node) as $term) {
      $terms[] = check_plain($term->name);
    }
    if (!empty($terms)) {
      $terms = implode(', ', $terms);
      $output .= "<p>$terms</p>";
    }
    else {
      $output .= '<p>'. t('No selected terms found') .'</p>';
    }
  }

  // Format pass / fail and summary options.
  if ($node->pass_rate || $node->summary_default || $node->summary_pass) {
    if ($node->pass_rate) {
      $output .= '<h3>'. t('Pass / fail and summary options') .'</h3>'."\n";
      $output .= '<p><strong>'. t('Percentage needed to pass:') .'</strong> '. check_plain($node->pass_rate) .'</p>'."\n";
      $output .= '<div><strong>'. t('Summary text if the user passed:') .'</strong> ';
      $output .= ($node->summary_pass) ? check_markup($node->summary_pass, $node->format) : t('No text defined.');
      $output .= '</div>'."\n";
    }
    $output .= '<div><strong>'. t('Default summary text:') .'</strong> ';
    $output .= !empty($node->summary_default) ? check_markup($node->summary_default, $node->format) : t('No text defined.');
    $output .= '</div>'."\n";
  }

  // Format result options if available.
  if (!empty($node->resultoptions)) {
    $scored_quiz = ($node->pass_rate > 0);

    $output .= '<h3>'. t('@quiz Results', array('@quiz' => QUIZ_NAME)) .'</h3>';

    $header = array(t('Name') => 'option_name', t('Summary') => 'option_summary');
    if ($scored_quiz) {
      $header = array_merge($header, array(t('Start') => 'option_start', t('End') => 'option_end'));
    }
    $values = array_values($header);

    foreach ($node->resultoptions as $option) {
      $row = array();
      foreach ($values as $field) {
        $row[] = $option[$field];
      }
      $option_rows[] = $row;
    }
    $output .= theme('table', array_keys($header), $option_rows);

  }

  // Format quiz questions.
  if (is_numeric(arg(1))) {
    $output .= '<h3>'. t('@quiz Questions', array('@quiz' => QUIZ_NAME)) .'</h3>';
    $questions = _quiz_get_questions($node->vid);
    $output .= theme('quiz_question_table', $questions, $node->nid);
  }
  return $output;
}

/**
 * Theme the user results page.
 *
 * @param $results
 *  An array of quiz information.
 * @return
 *  Themed html.
 *
 * @ingroup themeable
 */
function theme_quiz_get_user_results($results) {
  $output = '';
  $rows = array();
  while (list($key, $result) = each($results)) {
    $rows[] = array(
      l('view', 'user/quiz/'. $result['result_id'] .'/userresults'),
      check_plain($result['title']),
      check_plain($result['name']),
      $result['result_id'],
      format_date($result['time_start'], 'small'),
      ($result['time_end'] > 0) ? format_date($result['time_end'], 'small') : t('In Progress'),
    );
  }

  $header = array(
    t('Action'),
    t('@quiz Title', array('@quiz' => QUIZ_NAME)),
    t('Username'),
    t('Result<br />ID'),
    t('Time Started'),
    t('Finished?'));

  if (!empty($rows)) {
    $output .= theme('table', $header, $rows);
  }
  else {
    $output .= t('No @quiz results found.', array('@quiz' => QUIZ_NAME));
  }
  return $output;
}

/**
 * Theme the filtered question list.
 *
 * @ingroup themeable
 */
function theme_quiz_filtered_questions($form) {
  $quiz_id = is_numeric(arg(1)) ? arg(1) : NULL;
  $header = array(t('Random'), t('Always'), t('Never'), t('Question'), t('Type'), t('Edit'));
  $rows = array();
  $output = '';

  while (list($nid, $values) = each($form['question_status'])) {
    if (is_numeric($nid)) {
      $rows[] = array(
        drupal_render($form['question_status'][$nid][QUIZ_FEEDBACK_END]),
        drupal_render($form['question_status'][$nid][QUIZ_FEEDBACK_QUESTION]),
        drupal_render($form['question_status'][$nid][QUIZ_FEEDBACK_NEVER]),
        drupal_render($form['question'][$nid]),
        drupal_render($form['type'][$nid]),
        l(t('Edit'), 'node/'. $nid .'/edit/'. $quiz_id),
      );
    }
  }
  if (!empty($rows)) {
    $output .= theme('table', $header, $rows);
  }
  else {
    $output .= t('No questions found.');
  }
  return $output;
}

/**
 * Theme a table containing array of questions and options.
 *
 * @param $questions
 *  Array of question nodes.
 * @return
 *  HTML for a table.
 *
 * @ingroup themeable
 */
function theme_quiz_question_table($questions, $quiz_id = NULL) {
  $output = '';
  $rows = array();
  $status_descriptions = array(t('Random'), t('Always'), t('Never'));
  while (list($key, $question) = each($questions)) {
    $rows[] = array(
      $status_descriptions[$question->question_status],
      $question->question,
      $question->type,
      l(t('Edit'), 'node/'. $question->nid .'/edit/'. $quiz_id));
  }
  $header = array(t('Status'), t('Question'), t('Type'), t('Edit'));

  if (!empty($rows)) {
    $output .= theme('table', $header, $rows);
  }
  else {
    $output .= t('No questions found.');
  }
  return $output;
}

/**
 * Pass the correct mark to the theme so that theme authors can use an image.
 *
 * @ingroup themeable
 */
function theme_quiz_score_correct() {
  return theme('image', drupal_get_path('module', 'quiz') .'/images/correct.gif', t('correct'));
}

/**
 * Pass the incorrect mark to the theme so that theme authors can use an image.
 *
 * @ingroup themeable
 */
function theme_quiz_score_incorrect() {
  return theme('image', drupal_get_path('module', 'quiz') .'/images/incorrect.gif', t('incorrect'));
}

/**
 * Theme a progress indicator for use during a quiz.
 *
 * @param $question_number
 *  The position of the current question in the sessions' array.
 * @param $num_of_question
 *  The number of questions for this quiz as returned by quiz_get_number_of_questions().
 * @return
 *  Themed html.
 *
 * @ingroup themeable
 */
function theme_quiz_progress($question_number, $num_of_question) {
  // Determine the percentage finished (not used, but left here for other implementations).
  //$progress = ($question_number*100)/$num_of_question;

  // Get the current question # by adding one.
  $current_question = $question_number + 1;

  $output  = '';
  $output .= '<div id="quiz_progress">';
  $output .= t('Page %x of %y', array('%x' => $current_question, '%y' => $num_of_question));
  $output .= '</div><br />'."\n";
  $output .= '<div class="countdown"></div>';
  return $output;
}

/**
 * Theme a question page.
 *
 * @param $quiz
 *  The quiz node object.
 * @param $question_node
 *  The question node.
 * @return
 *  Themed html.
 *
 * @ingroup themeable
 *
 * @deprecated This should not be used. Rendering is now done through the standard node rendering system.
 */
function theme_quiz_take_question($quiz, $question_node) {
  //Calculation for quiz progress bar.
  $number_of_questions = quiz_get_number_of_questions($quiz->vid, $quiz->nid);
  $question_number = $number_of_questions - count($_SESSION['quiz_'. $quiz->nid]['quiz_questions']);
  $question_node->question_number = $question_number;
  // Set the title here in case themers want to do something different.
  drupal_set_title(check_plain($quiz->title));

  // Return the elements of the page.
  $output = '';
  $output .= theme('quiz_progress', $question_number, $number_of_questions);
  $output .= module_invoke($question_node->type, 'render_question', $question_node);
  return $output;
}

/**
 * Theme the summary page after the quiz has been completed.
 *
 * @param $quiz
 *  The quiz node object.
 * @param $questions
 *  The questions array as defined by _quiz_get_answers.
 * @param $score
 *  Array of score information as returned by quiz_calculate_score().
 * @param $summary
 *  Filtered text of the summary.
 * @return
 *  Themed html.
 *
 * @ingroup themeable
 */
function theme_quiz_take_summary($quiz, $questions, $score, $summary) {
  // Set the title here so themers can adjust.
  drupal_set_title(check_plain($quiz->title));

  // Display overall result.
  $output = '';

  // Only display scoring information if this is not a personality test:
  //if ($score['percentage_score']) {
  if (!empty($score['possible_score'])) {
    if (!$score['is_evaluated']) {
      $msg = t(
        'Parts of this @quiz have not been evaluated yet. The score below is not final.',
        array('@quiz' => QUIZ_NAME)
      );
      drupal_set_message($msg, 'error');
    }
    $output .= '<div id="quiz_score_possible">'. t('You got %num_correct of %question_count possible points.', array('%num_correct' => $score['numeric_score'], '%question_count' => $score['possible_score'])) .'</div>'."\n";
    $output .= '<div id="quiz_score_percent">'. t('Your score: %score%', array('%score' => $score['percentage_score'])) .'</div><br />'."\n";
  }
  $output .= '<div id="quiz_summary">'. check_markup($summary, $quiz->format) .'</div><br />'."\n";
  // Get the feedback for all questions.
  $output .= theme('quiz_feedback', $questions, ($quiz->pass_rate > 0), TRUE);

  return $output;
}

/**
 * Theme the summary page for user results.
 *
 * @param $quiz
 *  The quiz node object.
 * @param $questions
 *  The questions array as defined by _quiz_get_answers.
 * @param $score
 *  Array of score information as returned by quiz_calculate_score().
 * @param $summary
 *  Filtered text of the summary.
 * @return
 *  Themed html.
 *
 * @ingroup themeable
 */
function theme_quiz_user_summary($quiz, $questions, $score, $summary) {
  // Set the title here so themers can adjust.
  drupal_set_title(check_plain($quiz->title));

  if (!$score['is_evaluated']) {
    $msg = t('Parts of this @quiz have not been evaluated yet. The score below is not final.', array('@quiz' => QUIZ_NAME));
    drupal_set_message($msg, 'status');
  }

  // Display overall result.
  $output = '';
  $output .= '<div id="quiz_score_possible">'. t('You got %num_correct of %question_count possible points.', array('%num_correct' => $score['numeric_score'], '%question_count' => $score['possible_score'])) .'</div>'."\n";
  $output .= '<div id="quiz_score_percent">'. t('Your score was: @score%', array('@score' => $score['percentage_score'])) .'</div><br />'."\n";
  $output .= '<div id="quiz_summary">'. check_markup($summary,$quiz->format) .'</div><br />'."\n";
  // Get the feedback for all questions.
  $output .= theme('quiz_feedback', $questions, TRUE, TRUE);
  return $output;
}

/**
 * Theme the question feedback.
 *
 * @param $questions
 *  Array of quiz questions objects as returned by _quiz_get_answers.
 * @param showpoints
 *  Binary flag for whether to show the actual answers or not.
 * @param $showfeedback
 *  Binary flag for whether to show question feedback.
 * @return
 *  Themed html.
 *
 * @ingroup themeable
 */
function theme_quiz_feedback($questions, $showpoints = TRUE, $showfeedback = FALSE) {
  $header = array(format_plural(count($questions), 'Question Result', 'Question Results'));
  $rows = array();

  // Go through each of the questions.
  foreach ($questions as $question) {
    $cols = array();
    // Ask each question to render a themed report of how the user did.
    $module = quiz_module_for_type($question->type);

    $report = theme($module .'_report', $question, $showpoints, $showfeedback);

    // Only include reports with actual data:
    if (!empty($report) && strlen($report) > 0) {

      $cols[] = array('data' => $report, 'class' => 'quiz_summary_qrow');

      // Get the score result for each question only if it's a scored quiz.
      if ($showpoints) {

        $theme = ($question->correct) ? 'quiz_score_correct' : 'quiz_score_incorrect';
        $cols[] = array('data' => theme($theme), 'class' => 'quiz_summary_qcell');
      }
      // Pack all of this into a table row.
      $rows[] = array('data' => $cols, 'class' => 'quiz_summary_qrow');
    }
  }

  return theme('table', $header, $rows);
}

/**
 * Theme feedback for one question.
 *
 * @param $quiz
 *  Quiz node (may not be needed).
 * @param $question_node
 *  The question node giving feedback for.
 * @param $answer
 *  User's response to previous question.
 * @return
 *  Themed html.
 *
 * @ingroup themeable
 */
function theme_quiz_single_question_feedback($quiz, $report) {
  $output = '<div class="quiz-summary-question">';
  $output .= theme($report->type .'_feedback', $quiz, $report);
  $output .= '</div><br class="clear" />';
  return $output;
}

/**
 * Allow the option to theme the questions form.
 *
 * @ingroup themeable
 */
function theme_quiz_questions($form) {
  $output = '';
  $output .= drupal_render($form);
  return $output;
}

/**
 * Theme the "no feedback" option.
 *
 * @return
 *  Themed html feedback.
 *
 * @ingroup themeable
 */
function theme_quiz_no_feedback() {
  return t('Thanks for taking the quiz!');
}

function theme_quiz_single_question_node($node) {
  return $node->body;
}

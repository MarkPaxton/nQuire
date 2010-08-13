<?php

// $Id: short_answer.classes.inc,v 1.1.2.5 2009/12/09 10:44:19 sivaji Exp $

/**
 * The main classes for the short answer question type.
 *
 * These inherit or implement code found in quiz_question.classes.inc.
 *
 * If you are developing your own question type, the easiest place to start is with
 * quiz_question.truefalse.inc. This and long answer are good for understanding
 * question types that involve textual answers.
 *
 * @file
 */

/**
 * Implementation of QuizQuestion.
 *
 * This could have extended long answer, except that that would have entailed
 * adding long answer as a dependency.
 */
class ShortAnswerQuestion implements QuizQuestion {

  // Answer matching algorithms.
  const ANSWER_MATCH = 0;
  const ANSWER_INSENSITIVE_MATCH = 1;
  const ANSWER_REGEX = 2;
  const ANSWER_MANUAL = 3;

  // Field display types.
  const TEXT_ENTRY_TEXTFIELD = 0;
  const TEXT_ENTRY_TEXTAREA = 1;

  /**
   * The current node for this question.
   */
  protected $node = NULL;

  public function __construct($node) {
    $this->node = $node;
  }

  public function save($is_new = FALSE) {
    if (!isset($this->node->feedback)) {
      $this->node->feedback = '';
    }

    if ($is_new || $this->node->revision == 1) {
      $sql = 'INSERT INTO {quiz_short_answer_node_properties}
        (nid, vid, maximum_score, correct_answer, correct_answer_evaluation)
        VALUES (%d, %d, %d, \'%s\', %d)';
      db_query($sql, $this->node->nid, $this->node->vid, $this->node->maximum_score, $this->node->correct_answer, $this->node->correct_answer_evaluation);
    }
    else {
      $sql = 'UPDATE {quiz_short_answer_node_properties}
        SET maximum_score = %d, correct_answer = \'%s\', correct_answer_evaluation = %d
        WHERE nid = %d AND vid = %d';
      db_query($sql, $this->node->maximum_score, $this->node->correct_answer, $this->node->correct_answer_evaluation, $this->node->nid, $this->node->vid);
    }
  }

  public function validate($node, &$form) {
    $maximum_score = $node->maximum_score;
    if (!ctype_digit($maximum_score) || intval($maximum_score) < 0) {
      form_set_error('maximum_score', t('Score must be a positive integer (0 or higher).'));
    }

    if ($node->correct_answer_evaluation != self::ANSWER_MANUAL && empty($node->correct_answer)) {
      form_set_error('correct_answer_evaluation', t('An answer must be specified for any evaluation type other than manual scoring.'));
    }
  }

  public function delete($only_this_version = FALSE) {
    if ($only_this_version) {
      db_query('DELETE FROM {quiz_short_answer_node_properties} WHERE nid = %d AND vid = %d', $this->node->nid, $this->node->vid);
    }
    else {
      db_query('DELETE FROM {quiz_short_answer_node_properties} WHERE nid = %d', $this->node->nid);
    }
  }

  public function load() {
    $sql = 'SELECT maximum_score, correct_answer, correct_answer_evaluation
      FROM {quiz_short_answer_node_properties}
      WHERE nid = %d AND vid = %d';
    return db_fetch_object(db_query($sql, $this->node->nid, $this->node->vid));
  }

  public function view() {
    return $this->getQuestionForm($this->node);
  }

  // This is called whenever a question is rendered, either
  // to an administrator or to a quiz taker.
  public function getQuestionForm($node, $context = NULL) {
    $form['question'] = array(
       '#type' => 'markup',
       '#value' => check_markup($node->body, $node->format),
     );
     $form['tries'] = array(
       '#type' => 'textfield',
       '#title' => t('Answer'),
       '#description' => t('Enter your answer here'),
       '#default_value' => isset($context['tries']) ? $context['tries'] : '',
       '#size' => 60,
       '#maxlength' => 256,
       '#required' => FALSE,
     );

     return $form;
  }
  public function getAdminForm($edit = NULL) {
    /*
    $form['short_answer_allow_regex'] = array(
      '#type' => 'checkbox',
      '#title' => t('Allow regular expression matching'),
      '#description' => t('If this is checked, quiz creators can use regular expressions for matching criteria.'),
      '#default_value' => variable_get('short_answer_allow_regex', TRUE),
    );
    return $form;
    */
    $form['empty'] = array(
      '#type' => 'markup',
      '#value' => t('There are currently no available configuration options.'),
    );

  }
  public function getCreationForm($edit) {
     $form['answer'] = array(
       '#type' => 'fieldset',
       '#title' => t('Answer'),
       '#description' => t('Provide the answer and the method by which the answer will be evaluated.'),
       '#collapsible' => TRUE,
       '#collapsed' => FALSE,
     );

    $form['answer']['correct_answer_evaluation'] = array(
       '#type' => 'radios',
       '#title' => t('Pick one'),
       '#description' => t('Choose a matching type.'),
       '#options' => array(
         self::ANSWER_MATCH => t('Exact match (answer must match case)'),
         self::ANSWER_INSENSITIVE_MATCH => t('Case insensitive match (answer need not match case)'),
         self::ANSWER_REGEX => t('Match against a regular expression (answer must match the supplied regular expression)'),
         self::ANSWER_MANUAL => t('Manually score matches (no automatic grading)'),
       ),
       '#default_value' => isset($this->node->correct_answer_evaluation) ? $this->node->correct_answer_evaluation : self::ANSWER_MATCH,
       '#required' => FALSE,
     );

     $form['answer']['regex_box'] = array(
       '#type' => 'fieldset',
       '#title' => t('About regular expressions'),
       '#collapsible' => TRUE,
       '#collapsed' => TRUE,
     );

     $form['answer']['regex_box']['regex_help'] = array(
       '#type' => 'markup',
       '#value' => '<p>' .
         t('Regular expressions are an advanced syntax for pattern matching. They allow you to create a concise set of rules that must be met before a value can be considered a match.') .
         '</p><p>' .
         t('For more on regular expression syntax, visit !url.', array('!url' => l('the PHP regular expressions documentation', 'http://www.php.net/manual/en/book.pcre.php'))) .
         '</p>',
     );


     $form['answer']['correct_answer'] = array(
       '#type' => 'textfield',
       '#title' => t('Answer'),
       '#description' => t('Specify the answer. If this question is manually scored, no answer need be supplied.'),
       '#default_value' => isset($this->node->correct_answer) ? $this->node->correct_answer : '',
       '#size' => 60,
       '#maxlength' => 256,
       '#required' => FALSE,
     );
     $form['answer']['maximum_score'] = array(
       '#type' => 'textfield',
       '#title' => t('Maximum score'),
       '#description' => t('If this is automatically graded, this will be treated as an "all or nothing" score. Manual grading may assign any whole number value between 0 and this number.'),
       '#default_value' => isset($this->node->maximum_score) ? $this->node->maximum_score : 1,
       '#size' => 3,
       '#maxlength' => 3,
       '#required' => FALSE,
     );

     return $form;

  }
  public function getMaximumScore() {
    return $this->node->maximum_score;
  }

  /**
   * Evaluate the correctness of an answer based on the correct answer and evaluation method.
   */
  public function evaluateAnswer($user_answer) {
    $score = 0;
    switch ($this->node->correct_answer_evaluation) {
      case self::ANSWER_MATCH:
        if ($user_answer == $this->node->correct_answer) {
          $score = $this->node->maximum_score;
        }
        break;
      case self::ANSWER_INSENSITIVE_MATCH:
        if (drupal_strtolower($user_answer) == drupal_strtolower($this->node->correct_answer)) {
          $score = $this->node->maximum_score;
        }
        break;
      case self::ANSWER_REGEX:
        //drupal_set_message('Regex is: ' . $this->node->correct_answer, 'status');
        if (preg_match($this->node->correct_answer, $user_answer) > 0) {
          $score = $this->node->maximum_score;
        }
        break;
    }

    return $score;
  }

}

/**
 * The short answer question response class.
 */
class ShortAnswerResponse extends AbstractQuizQuestionResponse {

  /**
   * Get all quiz scores that have not yet been evaluated.
   *
   * @param $count
   *  Number of items to return (default: 50).
   * @param $offset
   *  Where in the results we should start (default: 0).
   *
   * @return
   *  Array of objects describing unanswered questions. Each object will have result_id, question_nid, and question_vid.
   */
  public static function fetchAllUnscoredAnswers($count = 50, $offset = 0) {
    $sql = 'SELECT a.result_id, a.question_nid, a.question_vid, r.title, n.time_end, n.time_start, n.uid
      FROM {quiz_short_answer_user_answers} AS a
      INNER JOIN {node_revisions} AS r ON a.question_vid = r.vid
      INNER JOIN {quiz_node_results} AS n ON a.result_id = n.result_id
      WHERE is_evaluated = 0';
    $results = db_query_range($sql, $offset, $count);
    $unscored = array();

    if ($results) {
      while ($row = db_fetch_object($results)) {
        $unscored[] = $row;
      }
    }

    return $unscored;
  }
  /**
   * Given a quiz, return a list of all of the unscored answers.
   *
   * @param $nid
   *  Node ID for the quiz to check.
   * @param $vid
   *  Version ID for the quiz to check.
   * @param $count
   *  Number of items to return (default: 50).
   * @param $offset
   *  Where in the results we should start (default: 0).
   *
   * @return
   *  Indexed array of result IDs that need to be scored.
   */
  public static function fetchUnscoredAnswersByQuestion($nid, $vid, $count = 50, $offset = 0) {
    $results = db_query_range('SELECT result_id FROM {quiz_short_answer_user_answers} WHERE is_evaluated = 0 AND question_nid = %d AND question_vid = %d', $nid, $vid, $offset, $count);
    $unscored = array();
    foreach (db_fetch_object($results) as $row) {
      $unscored[] = $row->result_id;
    }
    return $unscored;
  }

  /**
   * ID of the answer.
   */
  protected $answer_id = 0;

  public function __construct($rid, $question, $answer = NULL) {
    $this->rid = $rid;
    $this->question = $question;

    if (!isset($answer)) {
      $sql = "SELECT answer_id, answer, is_evaluated, score, question_vid, question_nid, result_id
        FROM {quiz_short_answer_user_answers}
        WHERE question_nid = %d AND question_vid = %d AND result_id = %d";
      $r = db_fetch_object(db_query($sql, $question->nid, $question->vid, $rid));

      if (!empty($r)) {
        $this->answer = $r->answer;
        $this->score = $r->score;
        $this->evaluated = $r->is_evaluated;
        $this->answer_id = $r->answer_id;
      }
    }
    else {
      $this->answer = $answer;
    }
  }

  public function save() {

    // We need to set is_evaluated depending on whether the type requires evaluation.
    // Unfortunately, when score() is called, we don't yet have the record to modify (since
    // score() is called before save()), so we have to set the param here.
    $this->is_evaluated = (int)($this->question->correct_answer_evaluation != ShortAnswerQuestion::ANSWER_MANUAL);

    $sql = "INSERT INTO {quiz_short_answer_user_answers}
      (answer, question_nid, question_vid, result_id, score, is_evaluated)
      VALUES ('%s', %d, %d, %d, %d, %d)";
    db_query($sql, $this->answer, $this->question->nid, $this->question->vid, $this->rid, $this->score, $this->is_evaluated);
    $this->answer_id = db_last_insert_id('quiz_long_answer_user_answers', 'answer_id');
  }
  public function delete() {
    $sql = 'DELETE FROM {quiz_short_answer_user_answers} WHERE question_nid = %d AND question_vid = %d AND result_id = %d';
    db_query($sql, $this->question->nid, $this->question->vid, $this->rid);
  }
  public function score() {
    // Manual scoring means we go with what is in the DB.
    if ($this->question->correct_answer_evaluation == ShortAnswerQuestion::ANSWER_MANUAL) {
      $sql = "SELECT score FROM {quiz_short_answer_user_answers} WHERE result_id = %d AND question_vid = %d";
      $score = (int)db_result(db_query($sql, $this->rid, $this->question->vid));
    }
    // Otherwise, we run the scoring automatically.
    else {
      $shortAnswer = new ShortAnswerQuestion($this->question);
      $score = $shortAnswer->evaluateAnswer($this->getResponse());
    }

    $this->score = $score;
    return $score;
  }

  public function isCorrect() {
    $possible = _quiz_question_get_instance($this->question)->getMaximumScore();
    $actual = $this->score;
    // To prevent Division by zero warning
    $possible = ($possible == 0) ? 1 : $possible;
    return (($actual / $possible) > 0.5);
  }

  public function getResponse() {
    return $this->answer;
  }

  public function formatReport($showpoints = TRUE, $showfeedback = TRUE) {

    $slug = '<div class="quiz_summary_question"><span class="quiz_question_bullet">'. t('Q:') . '</span> '.
      check_markup($this->question->body) .
      '</div>';

    $result = '<div class="quiz_answer_feedback">';
    if ($this->question && !empty($this->question->answers)) {
      $answer = (object) current($this->question->answers);

      if ($answer->is_evaluated == 1) {
        // Show score:
        if ($showpoints) {
          $args = array('@yours' => $answer->score, '@total' => $this->question->maximum_score);
          $result .= t('Score: @yours of @total possible points', $args);
        }

        // Show feedback, if any.
        if ($showfeedback && !empty($answer->feedback)) {
          $result .= '</div><div class="quiz_answer_feedback">' . $answer->feedback;
        }

      }
      else {
        $result .= t('This answer has not yet been scored.') .
          '<br/>' .
          t('Until the answer is scored, the total score will not be correct.');
      }

      if (user_access('score short answer')) {
        $path = sprintf('admin/quiz/score-short-answer/%s/%s', $this->question->vid, $this->rid);
        $result .= '<p>' . l(t('Score this answer'), $path) . '</p>';
      }
    }
    else {
      $result .= t('This question was not answered.');
    }
    $result .= '</div>';

    return $slug . $result;
  }
}

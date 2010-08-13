<?php
// $Id: long_answer.classes.inc,v 1.1.2.5 2009/12/09 10:44:19 sivaji Exp $

/**
 * Long answer classes.
 *
 * @file
 */


/**
 * Implementation of QuizQuestion.
 */
class LongAnswerQuestion implements QuizQuestion {

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
      $sql = 'INSERT INTO {quiz_long_answer_node_properties} (nid, vid, maximum_score) VALUES (%d, %d, %d)';
      db_query($sql, $this->node->nid, $this->node->vid, $this->node->maximum_score);
    }
    else {
      $sql = 'UPDATE {quiz_long_answer_node_properties} SET maximum_score = %d WHERE nid = %d AND vid = %d';
      db_query($sql, $this->node->maximum_score, $this->node->nid, $this->node->vid);
    }
  }

  public function validate($node, &$form) {
    // Check to make sure that the maximum score is greater-than or equal-to 0.
    $maximum_score = $node->maximum_score;
    if (!ctype_digit($maximum_score) || intval($maximum_score) < 0) {
      form_set_error('maximum_score', t('Score must be a positive integer (0 or higher).'));
    }
  }

  public function delete($only_this_version = FALSE) {
    if ($only_this_version) {
      db_query('DELETE FROM {quiz_long_answer_node_properties} WHERE nid = %d AND vid = %d', $this->node->nid, $this->node->vid);
    }
    else {
      db_query('DELETE FROM {quiz_long_answer_node_properties} WHERE nid = %d', $this->node->nid);
    }
  }

  public function load() {
    $sql = 'SELECT maximum_score FROM {quiz_long_answer_node_properties} WHERE nid = %d AND vid = %d';
    return db_fetch_object(db_query($sql, $this->node->nid, $this->node->vid));
  }

  public function view() {
    return $this->getQuestionForm($this->node);
  }

  // This is called whenever a question is rendered, either
  // to an administrator or to a quiz taker.
  public function getQuestionForm($node, $context = NULL) {
    //print_r($node);exit;
    $form['question'] = array(
       '#type' => 'markup',
       '#value' => $node->body
     );
     $form['tries'] = array(
       '#type' => 'textarea',
       '#title' => t('Answer'),
       '#description' => t('Enter your answer here. If you need more space, click on the grey bar at the bottom of this area and drag it down.'),
       '#rows' => 15,
       '#cols' => 60,
       '#required' => FALSE,
     );
     return $form;
  }
  public function getAdminForm($edit = NULL) {
    $form['settings'] = array(
      '#type' => 'markup',
      '#value' => t('There are no settings for this question type.'),
    );
    return $form;
  }
  public function getCreationForm($edit) {
    $form['maximum_score'] = array(
      '#type' => 'textfield',
      '#title' => t('Maximum Possible Score'),
      '#description' => t('Long answer questions are scored manually. This field indicates to the person scoring what the maximum number of points per essay is. Multichoice questions have a score of 1.'),
      '#default_value' => isset($this->node->maximum_score) ? $this->node->maximum_score : variable_get('long_answer_default_maximum_score', 1),
      '#size' => 3,
      '#maxlength' => 3,
      '#required' => TRUE,
    );
    return $form;
  }
  public function getMaximumScore() {
    return $this->node->maximum_score;
  }

}

class LongAnswerResponse extends AbstractQuizQuestionResponse {

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
      FROM {quiz_long_answer_user_answers} AS a
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
    $results = db_query_range('SELECT result_id FROM {quiz_long_answer_user_answers} WHERE is_evaluated = 0 AND question_nid = %d AND question_vid = %d', $nid, $vid, $offset, $count);
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
        FROM {quiz_long_answer_user_answers}
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
    $sql = "INSERT INTO {quiz_long_answer_user_answers} (answer, question_nid, question_vid, result_id) VALUES ('%s', %d, %d, %d)";
    db_query($sql, $this->answer, $this->question->nid, $this->question->vid, $this->rid);
    $this->answer_id = db_last_insert_id('quiz_long_answer_user_answers', 'answer_id');
  }

  public function delete() {
    $sql = 'DELETE FROM {quiz_long_answer_user_answers} WHERE question_nid = %d AND question_vid = %d AND result_id = %d';
    db_query($sql, $this->question->nid, $this->question->vid, $this->rid);
  }

  public function score() {
    $sql = "SELECT score FROM {quiz_long_answer_user_answers} WHERE result_id = %d AND question_vid = %d";
    $score = (int)db_result(db_query($sql, $this->rid, $this->question->vid));
    $this->score = $score;
    return $score;
  }

  public function isCorrect() {
    $possible = _quiz_question_get_instance($this->question)->getMaximumScore();
    $actual = $this->score;
    // To prevent Division by zero warning
    $possible = ($possible == 0) ? 1 : $possible;
    return (($actual / $possible) * 100 > 50);
  }

  public function getResponse() {
    return $this->answer;
  }

  public function formatReport($showpoints = TRUE, $showfeedback = TRUE) {

    $slug = '<div class="quiz_summary_question"><span class="quiz_question_bullet">'. t('Q:') . '</span> '.
      check_markup($this->question->body, $this->question->filter) .
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
          $result .= '</div><div class="quiz_answer_feedback">' . check_markup($answer->feedback, $this->question->format);
        }

      }
      else {
        $result .= t('This answer has not yet been scored.') .
          '<br/>' .
          t('Until the answer is scored, the total score will not be correct.');
      }

      if (user_access('score long answer')) {
        $path = sprintf('admin/quiz/score-long-answer/%s/%s', $this->question->vid, $this->rid);
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


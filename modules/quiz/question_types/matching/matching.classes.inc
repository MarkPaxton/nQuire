<?php
// $Id: matching.classes.inc,v 1.1.2.20 2010/01/03 17:52:30 sivaji Exp $
/**
 * quiz_directions.classes
 *
 * This module uses the question interface to define something which is actually not a question.
 *
 * A Quiz Directions node is a placeholder for adding directions to a quiz. It can be inserted any number
 * of times into a quiz. Example uses may include:
 *
 * - Initial quiz-wide directions
 * - Section directions, e.g. "The next five questions are multiple choice, please..."
 * - Final confirmation, e.g. "You have answered all questions. Click submit to submit this quiz."
 *
 * @file
 */

/**
 * Implementation of Matching.
 */
class MatchingQuestion implements QuizQuestion {
  /**
   * The current node for this question.
   */
  protected $node = NULL;

  public function __construct($node) {
    $this->node = $node;
  }

  public function save($is_new = FALSE) {

    for ($i = 1; $i <= variable_get('quiz_matching_form_size', 5); $i++) {
      //$foo->{'prop_' . $i};
      $question = $this->node->{'question_' . $i};
      $answer = $this->node->{'answer_' . $i};
      $feedback = $this->node->{'feedback_' . $i};

      if (empty($question) || empty($answer)) {
        continue;
      }
      $feedback = (isset($feedback))? $feedback : '';
      if ($is_new || $this->node->revision == 1) {
        $sql = "INSERT INTO {quiz_matching_node} (nid, vid, question, answer, feedback) VALUES (%d, %d, '%s', '%s', '%s')";
        db_query($sql, $this->node->nid, $this->node->vid, $question, $answer, $feedback);

      }
      else {
        $match_id = $this->node->{'match_id_' . $i};
        $sql = "UPDATE {quiz_matching_node} SET question = '%s', answer = '%s', feedback = '%s' WHERE match_id = %d";
        db_query($sql, $question, $answer, $feedback, $match_id);

      }
    }
  }


  public function validate($node, &$form) {
    // This space intentionally left blank. :)
  }


  public function delete($only_this_version = FALSE) {
    // Only delete a nid/vid.
    if ($only_this_version) {
      $sql = 'DELETE FROM {quiz_matching_node} WHERE nid = %d AND vid = %d';
      db_query($sql, $this->node->nid, $this->node->vid);
    }
    // Delete all versions of the quiz question.
    else {
      $sql = 'DELETE FROM {quiz_matching_node} WHERE nid = %d';
      db_query($sql, $this->node->nid, $this->node->vid);
    }
  }

  public function load() {
    $rows = $result = array();
    $sql = "SELECT match_id, question, answer, feedback FROM {quiz_matching_node} WHERE nid = %d AND vid = %d";
    $results = db_query($sql, $this->node->nid, $this->node->vid);
    while ($result = db_fetch_object($results)) {
      $rows[] = $result;
    }
    return $rows;
  }



  public function view() {
    return $this->getQuestionForm($this->node);
  }

  // This is called whenever a question is rendered, either
  // to an administrator or to a quiz taker.
  public function getQuestionForm($node, $context = NULL) {
    $form = array();
    $form['question'] = array(
      '#type' => 'markup',
      '#value' => check_markup($this->node->body, $this->node->format, FALSE),
    );
    list($questions, $select_option) = $this->getQuestion($node);
    foreach ($questions as $question) {
      $form['subquestion']['tries[' . $question['match_id'] . ']'] = array(
        '#type' => 'select',
        '#title' => $question['question'],
        '#options' => $this->customShuffle($select_option),
      );
    }
    return array($form['question'], $this->customShuffle($form['subquestion']));
  }

  public function customShuffle($array = array()) {
    $new_array = array();
    while (count($array)) {
      $element = array_rand($array);
      $new_array[$element] = $array[$element];
      unset($array[$element]);
    }
    return $new_array;
  }

  public function getQuestion($node) {
    $questions = $select_option = array();
    $sql = "SELECT match_id, question, answer, feedback FROM {quiz_matching_node} WHERE nid = %d AND vid = %d";
    $results = db_query($sql, $node->nid, $node->vid);
    while ($result = db_fetch_object($results)) {
      $questions[] = array(
        'match_id' => $result->match_id,
        'question' => $result->question,
        'answer' => $result->answer,
        'feedback' => $result->feedback,
      );
      $select_option[$result->match_id] = $result->answer;
    }
    return array($questions, $select_option);
  }

  public function getAdminForm($edit = NULL) {
    $form['matching'] = array(
      '#type' => 'fieldset',
      '#title' => t('Matching Settings'),
      '#description' => t('Matching Questions Settings and Configuration'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );
    $form['matching']['quiz_matching_form_size'] = array(
      '#type' => 'textfield',
      '#title' => t('Match Question Size'),
      '#description' => t('Number of question allowed to wrap under a match type question.'),
      '#default_value' => 5
    );
    return $form;
  }

  public function getCreationForm($edit) {

    $form['match'] = array(
      '#type' => 'fieldset',
      '#title' => t('Match Question'),
    );

    for ($i = 1; $i <= variable_get('quiz_matching_form_size', 5); ++$i) {

      $match_id = $this->node->{$i - 1}->match_id;
      $question = $this->node->{$i - 1}->question;
      $answer = $this->node->{$i - 1}->answer;
      $feedback = $this->node->{$i - 1}->feedback;

      $form['match']['question_' . $i] = array(
        '#type' => 'fieldset',
        '#title' => t('Question '. $i),
      );
      $form['match']['question_' . $i]['match_id_' . $i] = array(
        '#type' => 'hidden',
        '#default_value' => isset($match_id) ? $match_id : ''
      );
      $form['match']['question_' . $i]['question_' . $i] = array(
        '#type' => 'textarea',
        '#title' => t('Question'),
        '#rows' => 2,
        '#default_value' => isset($question) ? $question : ''
      );
      $form['match']['question_' . $i]['answer_' . $i] = array(
        '#type' => 'textarea',
        '#title' => t('Match'),
        '#rows' => 2,
        '#default_value' => $answer//isset($answer) ? $answer : ''
      );

      $form['match']['question_' . $i]['feedback_' . $i] = array(
        '#type' => 'textarea',
        '#title' => t('Feedback / Reason'),
        '#rows' => 2,
        '#default_value' => isset($feedback) ? $feedback : ''
      );
    }
    return $form;
  }

  public function getMaximumScore() {
    return 1;
  }

  public function getCorrectAnswer() {
    $correct_answers = array();
    //match_id  nid   vid   question  answer  feedback # {quiz_matching_node} fields

    $results = db_query("SELECT match_id, question, answer FROM {quiz_matching_node} WHERE nid = %d AND vid = %d", $this->node->nid, $this->node->vid);

    while ($result = db_fetch_object($results)) {
      $correct_answers[$result->match_id] = array(
        'match_id' => $result->match_id,
        'question' => $result->question,
        'answer' => $result->answer
      );
    }
    return $correct_answers;
  }
}

/**
 * Class that describes a "Directions question response". For
 * the most part, no real scoring takes place for a direction
 * node. However, there are a few behind-the-scenes tricks that
 * are done here to make the quiz-taking process a little easier.
 */

class MatchingResponse extends AbstractQuizQuestionResponse {

  public function __construct($rid, $question, $answer = NULL) {
    $this->rid = $rid;
    $this->question = $question;
    $this->answer = $answer;
    $this->is_correct = $this->score();
  }

  public function save() {
    $user_answers = $this->answer;
    foreach ($user_answers as $key => $value) {
      $score = ($key == $value)? 1 : 0;
      $sql = "INSERT INTO {quiz_matching_user_answers} (match_id, result_id, answer, score) VALUES (%d, %d, %d, %d)";
        // This is expensive need to be changed
      db_query($sql, $key, $this->rid, (int)$value, $score);
    }
  }

  public function delete() {
    //$sql = 'DELETE FROM {quiz_matching_user_answers} WHERE question_nid = %d AND question_vid = %d AND result_id = %d';
    //db_query($sql, $this->question->nid, $this->question->vid, $this->rid);
  }

  public function score() {
    $wrong_answer = 0;
    $user_answers = isset($this->answer) ? $this->answer : $this->getUserAnswers();
    foreach ($user_answers as $key => $value) {
      if ($key != $value) {
        $wrong_answer++;
      }
    }
    $this->score = (!empty($user_answers) && !$wrong_answer) ? 1 : 0;
    return $this->score;
  }


  public function getResponse() {
    return $this->answer;
  }


  public function getUserAnswers() {
    $user_answers = array();
    // answer_id  match_id  result_id   score   answer #{quiz_matching_user_answers} fields
    $results = db_query("SELECT match_id, answer FROM {quiz_matching_user_answers} WHERE result_id = %d", $this->rid);
    while ($result = db_fetch_object($results)) {
      $user_answers[$result->match_id] = $result->answer;
    }
    return $user_answers;
  }


  public function formatReport($showpoints = TRUE, $showfeedback = TRUE) {
    $rows = $innerheader = array();
    // Build the question answers header (add blank space for IE).
    $innerheader[] = t('Match');
    if ($showpoints) {
      $innerheader[] = t('Correct Answer');
    }
    $innerheader[] = t('User Answer');
    if ($showfeedback) {
      $innerheader[] = '&nbsp;';
    }

    //if (empty($this->question->answers)) {
    //  return t('Missing question.');
    //}
    $MatchingQuestion = new MatchingQuestion($this->question);
    $correct_answers = $MatchingQuestion->getCorrectAnswer();

    //$correct_answers = $this->question
    $user_answers = $this->getUserAnswers();

    //print_r($correct_answers);
    //print_r($user_answers);
    //exit;
    foreach ($correct_answers as $correct_answer) {
      $id = $user_answers[$correct_answer['match_id']];
      $theme = ($correct_answer['answer'] == $correct_answers[$id]['answer']) ? 'quiz_score_correct' : 'quiz_score_incorrect';
      $rows[] = array(
        'match' => check_markup($correct_answer['question'], $this->question->format),
        'correct_answer' => ($showpoints) ? check_plain($correct_answer['answer']) : '',
        'user_answer' => ($showfeedback) ? check_plain($correct_answers[$id]['answer']) : '',
        'image' => ($showfeedback) ? array('data' => theme($theme), 'class' => 'quiz_summary_qcell') : '',
      );
    }

    //$answer = $this->question->answers[0];
    //$correct_answer = $answer['is_correct'] ? $answer['answer'] : !$answer['answer'];
    //$user_answer = $answer['answer'];

    //if ($showpoints) {
    //  $rows[0][] = ($correct_answer ? t('True') : t('False'));
   // }
    //$rows[0][] = ($user_answer ? t('True') : t('False'));
    //if($showfeedback && !empty($this->question->feedback)) {
    //  $rows[0][] = $this->question->feedback;
    //}

    // Add the cell with the question and the answers.
    $q_output = '<div class="quiz_summary_question"><span class="quiz_question_bullet">'. t('Q:') . '</span> '. check_markup($this->question->body) .'</div>';
    $q_output .= theme('table', $innerheader, $rows) .'<br />';
    return $q_output;
  }
}

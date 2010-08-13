<?php

// $Id: quiz_question.core.inc,v 1.1.2.4 2009/08/26 08:22:47 falcon Exp $

/**
 * Classes used in the Quiz Question module.
 *
 * The core of the Quiz Question module is a set of interfaces and abstract
 * classes that can be used to quickly and efficiently create new question
 * types.
 *
 * Why OO?
 * Drupal has a long history of avoiding many of the traditional OO structures
 * and metaphors. However, with PHP 5, there are many good reasons to use OO
 * principles more broadly.
 *
 * The case for Quiz question types is that question types all share common
 * structure and logic. Using the standard hook-only Drupal metaphor, we are
 * forced to copy and past large amounts of repetitive code from question
 * type to question type. By using OO principles and construction, we can
 * easily encapsulate much of that logic, while still making it easy to
 * extend the existing content.
 *
 * Where do I start?
 * To create a new question type, check out the True/False type that is included
 * in this module.
 *
 * @file
 */

/**
 * A Quiz Question.
 * There should be an implementation of this interface for every question type support
 * by Quiz.
 */
interface QuizQuestion {

  /**
   * Construct a new quiz question.
   */
  public function __construct($node);

  /**
   * Responsible for handling insert/update of question-specific data.
   * This is typically called from within the Node API, so there is no need
   * to save the node. This function is only responsible for saving data
   * specific to the implement ation.
   *
   * The $is_new flag is set to TRUE whenever the node is being initially
   * created.
   *
   * A save function is required to handle the following three situations:
   * - A new node is created ($is_new is TRUE)
   * - A new node *revision* is created ($is_new is NOT set, because the
   *   node itself is not new).
   * - An existing node revision is modified.
   *
   * @param $is_new
   *  TRUE when the node is initially created.
   */
  public function save($is_new = FALSE);
  /**
   * Provides validation for question before it is created.
   *
   * When a new question is created and initially submited, this is
   * called to validate that the settings are acceptible.
   *
   * @param $node
   *  The node storing the question.
   * @param $form
   *  The processed form.
   */
  public function validate($node, &$form);
  /**
   * Deletes a question from the database.
   *
   * @param $only_this_version
   *  If the $only_this_version flag is TRUE, then only the particular
   *  nid/vid combo should be deleted. Otherwise, all questions with the
   *  current nid can be deleted.
   */
  public function delete($only_this_version = FALSE);
  /**
   * Retrieve information relevant for viewing the node.
   * This data is generally added to the node's extra field.
   */
  public function view();
  /**
   * Retrieve information about the question and add it to the node.
   */
  public function load();
  /**
   * Get the form that will be displayed to the test-taking user.
   *
   * @param $node
   *  The question node.
   * @param $context
   *  The form context.
   * @return
   *  Must return a FAPI array.
   */
  public function getQuestionForm($node, $context = NULL);
  /**
   * Get the form that contains admin settings for this question type.
   *
   * @return
   *  Must return a FAPI array.
   */
  public function getAdminForm($edit = NULL);
  /**
   * Get the form used to create a new question.
   * @return
   *  Must return a FAPI array.
   */
  public function getCreationForm($edit);
  /**
   * Get the maximum possible score for this question.
   */
  public function getMaximumScore();

}

/**
 * Each question type must store its own response data and be able to calculate a score for
 * that data.
 */
interface QuizQuestionResponse {

  /**
   * Create a new user response.
   *
   * @param $rid
   *  The result ID for the user's result set. There is one result ID per time
   *  the user takes a quiz.
   * @param $question
   *  The question node.
   * @param $answer
   *  The answer (dependent on question type).
   */
  public function __construct($rid, $question, $answer = NULL);

  /**
   * Save the current response.
   */
  public function save();
  /**
   * Delete the response.
   */
  public function delete();
  /**
   * Calculate the score for the response.
   * This MUST set the $score instance variable.
   */
  public function score();
  /**
   * Determine whether the score is a passing score.
   */
  public function isCorrect();
  /**
   * Indicate whether the response has been evaluated (scored) yet.
   * Questions that require human scoring (e.g. essays) may need to manually
   * toggle this.
   */
  public function isEvaluated();
  /**
   * Get the user's response.
   */
  public function getResponse();
  /**
   * Get data suitable for reporting a user's score on the question.
   * This expects an object with the following attributes:
   *
   *  answer_id; // The answer ID
   *  answer; // The full text of the answer
   *  is_evaluated; // 0 if the question has not been evaluated, 1 if it has
   *  score; // The score the evaluator gave the user; this should be 0 if is_evaluated is 0.
   *  question_vid
   *  question_nid
   *  result_id
   */
  public function getReport();

  /**
   * Return an HTML marked-up report for displaying the results of this question.
   *
   * @return
   *  An HTML string.
   */
  public function formatReport($showPoints = TRUE, $showFeedback = TRUE);

  /**
   * Get the integer score.
   */
  public function getScore();

  /**
   * Repesent the response as a stdClass object.
   *
   * Convert data to an object that has the following properties:
   * - $score
   * - $rid
   * - $nid
   * - $vid
   * - $is_correct
   */
  public function toBareObject();
}

/**
 * A base implementation of QuizQuestionResponse.
 *
 */
abstract class AbstractQuizQuestionResponse implements QuizQuestionResponse{
  // These are only public until we can
  // adjust all of Quiz. DO NOT EXPECT THESE
  // TO BE ACCESSIBLE!
  public $score = 0;
  public $rid = 0;
  public $is_correct = FALSE;

  protected $evaluated = TRUE;
  protected $question = NULL;
  protected $answer = NULL;

  /*
  public function __construct($rid, $question, $answer) {
    $this->rid = $rid;
    $this->question = $question;
    $this->answer = $answer;
  }
  */

  public function isEvaluated() {
    return $this->evaluated;
  }

  /**
   * Check to see if the answer is marked as correct.
   *
   * This default version returns TRUE iff the score is equal to the maximum possible score.
   */
  function isCorrect() {
    $possible = _quiz_question_get_instance($this->question)->getMaximumScore();
    $actual = $this->score;

    return ($possible == $actual);
  }

  function getScore() {
    return $this->score;
  }

  function toBareObject() {
    $obj = new stdClass();
    $obj->score = $this->score; // This can be 0 for unscored.
    $obj->nid = $this->question->nid;
    $obj->vid = $this->question->vid;
    $obj->rid = $this->rid;
    $obj->is_correct = $this->isCorrect();
    $obj->is_evaluated = $this->isEvaluated();
    $obj->is_skipped = FALSE;
    return $obj;
  }

  public function getReport() {
    // Basically, we encode internal information in a
    // legacy array format for Quiz.
    $report = array(
      'answer_id' => 0, // <-- Stupid vestige of multichoice.
      'answer' => $this->answer,
      'is_evaluated' => $this->isEvaluated(),
      'is_correct' => $this->isCorrect(),
      'score' => $this->score,
      'question_vid' => $this->question->vid,
      'question_nid' => $this->question->nid,
      'result_id' => $this->rid,
    );

    return $report;
  }
}


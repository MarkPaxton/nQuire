<?php
// $Id: quiz.admin.inc,v 1.2.2.35 2010/04/05 10:44:00 sivaji Exp $

/**
 * Administrator interface for Quiz module.
 *
 * @file
 */


// QUIZ ADMIN

/**
 * Quiz Admin.
 */
function quiz_admin($nid) {
  $breadcrumb = drupal_get_breadcrumb();
  //lets add the Quiz Results to the breadcrumb array
  $breadcrumb[] = l(t('Quiz Results'), 'admin/quiz/results');
  drupal_set_breadcrumb($breadcrumb);

  $results = _quiz_get_results($nid);
  return theme('quiz_admin', $results);
}

/**
 * Displays the quizzes by title with a link to the appropriate results
 * for that specific quiz.
 *
 * @return
 *  Formatted data.
 */
function quiz_admin_quizzes() {
  $results = _quiz_get_quizzes();
  return theme('quiz_admin_quizzes', $results);
}

 // QUIZ RESULTS ADMIN

 /**
  * Quiz Results Admin.
  *
  * @param $qid
  *  The quiz result ID for a particular result.
  */
function quiz_admin_results($qid) {
  $breadcrumb = drupal_get_breadcrumb();
  // Lets add the Quiz Results to the breadcrumb array.
  $breadcrumb[] = l(t('Quiz Results'), 'admin/quiz/results');

  // We join against node because we might have multiple versions
  // of a quiz, and joining against quiz_node_properties would
  // return multiple rows with the same quiz result. Since we are
  // only hitting the {node} index, this should be fast.
  $result = db_fetch_object(db_query(
    "SELECT qnrs.nid
      FROM  {quiz_node_results} qnrs
      INNER JOIN {node} n ON qnrs.nid = n.nid
      WHERE qnrs.result_id = %d",
    $qid
  ));
  if ($result->nid) {
    $quiz = node_load($result->nid);
    $questions = _quiz_get_answers($qid);
    $score = quiz_calculate_score($quiz, $qid);
    $summary = _quiz_get_summary_text($quiz, $score);

    // Lets add the quiz title to the breadcrumb array.
    $breadcrumb[] = l($quiz->title, 'admin/quiz/' . $result->nid . '/view');
    drupal_set_breadcrumb($breadcrumb);
    return theme('quiz_admin_summary', $quiz, $questions, $score, $summary);
  }
  else {
    // Set the breadcrumb without the title since there isn't one and show error page.
    drupal_set_breadcrumb($breadcrumb);
    drupal_not_found();
  }
}

/**
 * Creates a form for quiz questions.
 *
 * Handles the manage questions tab.
 *
 * @param $qid
 *   ID of quiz to create
 * @return
 *   String containing the form.
 */
function quiz_questions($node) {
  // Set page title.
  drupal_set_title(check_plain($node->title));

  // Add JS
  $path = drupal_get_path('module', 'quiz') . '/quiz.admin.js';
  drupal_add_js($path, 'module');

  return drupal_get_form('quiz_questions_form', $node);
}

// EDIT QUIZ

/**
 * Handles "manage questions" tab.
 *
 * Displays form which allows questions to be assigned to the given quiz.
 *
 * @param $context
 *  The form context
 * @param $quiz
 *  The quiz node.
 * @return
 *  HTML output to create page.
 */
function quiz_questions_form($context, $quiz) {
  // This is a target for AHAH callbacks. Do not remove.
  $form['ahah_target'] = array(
    '#type' => 'markup',
    '#value' => '<div id="questions-always-target"></div>',
  );

  // Display links to create other questions.
  $form['additional_questions'] = array(
    '#type' => 'fieldset',
    '#title' => t('Create additional questions'),
    '#theme' => 'additional_questions',
  );
  $types = _quiz_get_question_types();
  foreach ($types as $type => $info) {
    $url_type = str_replace('_', '-', $type);
    $form['additional_questions'][$type] = array(
      '#type' => 'markup',
      // FIXME: This looks broken:
      '#value' => '<div class="add-questions">' . l(t($info['name']), 'node/add/'. $url_type .'/'. $quiz->nid, array('title' => t('Go to @name administration', array('@name' => $info['name'])))) .'</div>',
    );
  }

  // Display questions 'always' on this quiz.
  $form['filtered_question_list_always'] = array(
    '#type' => 'fieldset',
    '#title' => t('Questions always on this quiz'),
 //   '#theme' => 'quiz_filtered_questions',
    '#theme' => 'question_selection_table',
    '#collapsible' => TRUE,
    'question_status' => array('#tree' => TRUE),
  );

  $form['filtered_question_list_always']['always_box'] = array(
    '#type' => 'fieldset',
    '#title' => '<strong>' . t('Find and add a question') . '</strong>',
    '#description' => t('Begin typing a question title or keyword. Suggestions will be presented based on your typing. ') .
       '<strong>' . t('You must choose one of the suggested questions.') . '</strong> ' .
       t('To add a new question, use the "Create additional questions" section above.'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );

  $form['filtered_question_list_always']['always_box']['always_autocomplete'] = array(
    '#type' => 'textfield',
    //'#title' => t('Find a question'),
    // '#description' => t('Begin typing a question title or keyword. Suggestions will be presented based on your typing. ') .
    //       '<strong>' . t('You must choose one of the suggested questions.') . '</strong> ' .
    //       t('To add a new question, use the "Create additional questions" section above.'),
    '#default_value' => '',
    '#size' => 60,
    '#maxlength' => 256,
    '#required' => FALSE,
    '#autocomplete_path' => 'admin/quiz/listquestions',
  );
  $form['filtered_question_list_always']['always_box']['add_to_list'] = array(
    '#type' => 'submit',
    '#value' => t('Add to quiz'),
    '#submit' => 'quiz_questions_form_submit',
    '#ahah' => array(
      'path' => 'admin/quiz/newquestion',
      'wrapper' => 'questions-always-target',
      'progress' => array('type' => 'bar', 'message' => t('Adding question...')),
    ),
  );
  $form['filtered_question_list_always']['remove_from_quiz'] = array(
    '#type' => 'hidden',
    '#default_value' => '',
  );

  // Display questions 'random' on this quiz.
  $form['filtered_question_list_random'] = array(
    '#type' => 'fieldset',
    '#title' => t('Pool of questions randomly selected for this quiz'),
    '#theme' => 'question_selection_table',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE, // Setting this to TRUE breaks tableDrag. What a drag.
    'question_status' => array('#tree' => TRUE),
  );

  $form['filtered_question_list_random']['number_of_random_questions'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of questions to randomize'),
    '#size' => 3,
    '#default_value' => $quiz->number_of_random_questions,
    '#description' => t('The number of randomly selected questions that should be assigned to this quiz.'),
    '#weight' => 1,
  );

  $form['filtered_question_list_random']['random_box'] = array(
    '#type' => 'fieldset',
    '#title' => '<strong>' . t('Find and add a question') . '</strong>',
    '#description' => t('Begin typing a question title or keyword. Suggestions will be presented based on your typing. ') .
       '<strong>' . t('You must choose one of the suggested questions.') . '</strong> ' .
       t('To add a new question, use the "Create additional questions" section above.'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#weight' => -1,
  );

  $form['filtered_question_list_random']['random_box']['random_autocomplete'] = array(
    '#type' => 'textfield',
    '#default_value' => '',
    '#size' => 60,
    '#maxlength' => 256,
    '#required' => FALSE,
    '#weight' => -2,
    '#autocomplete_path' => 'admin/quiz/listquestions',
  );
  $form['filtered_question_list_random']['random_box']['add_to_random'] = array(
    '#type' => 'submit',
    '#value' => t('Add to quiz'),
    '#submit' => 'quiz_questions_form_submit',
    '#weight' => -1,
    '#ahah' => array(
      'path' => 'admin/quiz/newquestion',
      'wrapper' => 'questions-always-target',
      'progress' => array('type' => 'bar', 'message' => t('Adding question...')),
    ),
  );


  $terms =  _quiz_taxonomy_select($quiz->tid);
  if (!empty($terms) && function_exists('taxonomy_get_vocabularies')) {
    $form['filtered_question_list_random']['random_term_id'] = array(
      '#type' => 'select',
      '#title' => t('Terms'),
      '#size' => 1,
      '#options' => _quiz_taxonomy_select($quiz->tid),
      '#default_value' => $quiz->tid,
      '#description' => t('Randomly select from questions with this term, or choose from the random question pool below'),
      '#weight' => 2,
    );
  }



  // Build up a list of questions, sorted into those that are random and those that are
  // always on the quiz.
  $questions = _quiz_get_questions($quiz->vid);
  $rows = array();
  $form['filtered_question_list_random']['weights'] = array('#tree' => TRUE);
  $form['filtered_question_list_always']['weights'] = array('#tree' => TRUE);
  foreach ($questions as $question) {
    $id_mod = ($question->question_status == QUESTION_RANDOM ? 'random' : 'always');
    $fieldset = 'filtered_question_list_' . $id_mod;
    $id = $id_mod . '-' . $question->nid;

    $form[$fieldset]['weights'][$id] = array(
      //'#type' => 'weight',
      //'#delta' => 60,
      '#type' => 'textfield',
      '#size' => 3,
      '#maxlength' => 4,
      '#default_value' => isset($question->weight) ? $question->weight : 0,
    );
    $form[$fieldset]['titles'][$id] = array(
      // '#value' => $question->question, // Question is too long for drag and drop.
      '#value' => $question->title,
    );
    $form[$fieldset]['types'][$id] = array(
      '#value' => $question->type,
    );
    $form[$fieldset]['view_links'][$id] = array(
      '#value' => l('View', 'node/' . $question->nid),
    );
    $form[$fieldset]['remove_links'][$id] = array(
      // FIXME: This does not degrade for non-JS browsers.
      //'#value' => l('Remove', 'node/' . $question->nid .'/questions/remove', array('attributes' => array('class' => 'rem-link'))),
      '#value' => '<a href="#" class="rem-link">' . t('Remove') . '</a>',
    );
  }

  // Show the number of 'always' questions in the 'always' table header.
  $always_count = isset($form['filtered_question_list_always']['titles']) ? count($form['filtered_question_list_always']['titles']) : 0;
  $form['filtered_question_list_always']['#title'] .= ' ('. $always_count .')';

  $form['new_revision'] = array(
    '#type' => 'checkbox',
    '#default_value' => in_array('revision', variable_get('node_options_quiz', array())),
    '#title' => t('New revision'),
    '#description' => t('Allow question status changes to create a new revision of the quiz?'),
  );

  $form['timestamp'] = array('#type' => 'hidden', '#value' => time());

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit questions'),
  );

  return $form;
}

/**
 * Validate that the supplied questions are real.
 */
function quiz_questions_form_validate($form, $form_state) {
  $question_types = array_keys(_quiz_get_question_types());
  $removed = explode(',', $form_state['values']['remove_from_quiz']);
  $placeholders = db_placeholders($question_types, 'varchar');
  $sql = 'SELECT COUNT(nid) FROM {node} WHERE type IN (' . $placeholders . ') AND nid = %d';
  $already_checked = array();
  if (empty($form_state['values']['weights'])) {
    return;
  }
  foreach ($form_state['values']['weights'] as $id => $weight) {
    if (in_array($id, $removed)) {
      // Ignore items that are going to be removed anyway.
      continue;
    }
    list($state, $nid) = explode('-', $id, 2);
    $params = $question_types; // Copy array.
    $params[] = $nid;

    if (db_result(db_query($sql, $params)) == 0) {
      form_set_error('none', 'One of the supplied questions was invalid. It has been removed from the quiz.');
      unset($form_state['values']['weights'][$id]);
    }
    elseif (in_array($nid, $already_checked)) {
      form_set_error('none', 'A duplicate question has been removed. You can only ask a question once per quiz.');
      unset($form_state['values']['weights'][$id]);
    }
    else {
      $already_checked[] = $nid;
    }
  }
}


 /**
  * Submit function for quiz_questions.
  *
  * Updates from the "manage questions" tab.
  *
  * @param $form_id
  *  A string containing the form id.
  * @param $values
  *  Array containing the form values.
  */
function quiz_questions_form_submit($form, &$form_state) {
  // This is ugly and should be fixed.
  $quiz = node_load(arg(1));

  $new_revision = $form_state['values']['new_revision'];
  $num_random = $form_state['values']['number_of_random_questions'];
  $term_id = isset($form_state['values']['random_term_id']) ? $form_state['values']['random_term_id'] : 0;

  $removed = explode(',', $form_state['values']['remove_from_quiz']);
  // Get quiz questions that will always be on the quiz:
  $questions = array();
  foreach ($form_state['values']['weights'] as $id => $weight) {
    if (in_array($id, $removed)) {
      // Skip items that should be removed.
      continue;
    }
    list($state, $nid) = explode('-', $id, 2);
    $nid = (int)$nid;
    $question['nid'] = $nid;
    $question['state'] = ($state == 'always' ? QUESTION_ALWAYS : QUESTION_RANDOM);
    $question['weight'] = $weight;

    // Add item as an object in the questions array.
    $questions[] = (object)$question;
  }

  // Save questions.
  quiz_set_questions($quiz, $questions, (bool)$new_revision);

  // If using random questions and no term ID is specified, make sure we have enough.
  if (empty($term_id)) {
   $assigned_random = 0;

   foreach ($questions as $question) {
     if ($question->state == QUESTION_RANDOM) {
       ++$assigned_random;
     }
   }

   // Adjust number of random questions downward to match number of selected questions..
   if ($num_random > $assigned_random) {
     $num_random = $assigned_random;
     drupal_set_message(t('The number of random questions for this @quiz have been lowered to %anum to match the number of questions you assigned.', array('@quiz' => QUIZ_NAME, '%anum' => $assigned_random), 'warning'));
   }
  }
  else {
    // Warn user if not enough questions available with this term_id.
    $available_random = count(_quiz_get_random_taxonomy_question_ids($term_id, $num_random));
    if ($num_random > $available_random) {
      $num_random = $available_random;
      drupal_set_message(t('There are currently not enough questions assigned to this term (@random). Please lower the number of random quetions or assign more questions to this taxonomy term before taking this @quiz.', array('@random' => $available_random, '@quiz' => QUIZ_NAME)), 'error');
    }
  }

  // Update the quiz properties.
  $sql = "UPDATE {quiz_node_properties} SET number_of_random_questions = %d, tid = %d WHERE vid = %d AND nid = %d";
  if (db_query($sql, $num_random, $term_id, $quiz->vid, $quiz->nid) == FALSE) {
    drupal_set_message(t('There was an error updating the @quiz.', array('@quiz' => QUIZ_NAME)), 'error');
  }
  else {
    drupal_set_message(t('Questions updated successfully.'));
  }
}

/**
 * Autocompletion function for quiz questions.
 */
function quiz_admin_list_questions_ac($vid, $string = '') {
  $type_map = _quiz_get_question_types();
  $values = array_keys($type_map);
  $placeholder = db_placeholders($values, 'varchar');
  $query = strtolower($string);

  $sql = "SELECT n.nid, n.title, n.type
    FROM {node} n
    INNER JOIN {node_revisions} r ON n.vid = r.vid
    WHERE n.type IN ($placeholder)
      AND n.status = 1
      AND (LOWER(n.title) LIKE '%%%s%%' OR LOWER(r.body) LIKE '%%%s%%')";

  array_push($values, $query, $query);
  $result = db_query_range(db_rewrite_sql($sql), $values, 0, 20);

  $matches = array();
  while ($node = db_fetch_object($result)) {
    $type = $type_map[$node->type]['name'];
    $key = sprintf('%s [type:%s, id:%d]', check_plain($node->title), check_plain($type), $node->nid);
    $matches[$key] = $key;
  }

  drupal_json($matches);
}

/**
 * AHAH form completion for adding new questions.
 */
function quiz_admin_add_question_ahah() {
  $cached_form_state = array();
  $form_id = $_POST['form_build_id'];

  // Make sure the form exists
  if (!($cached_form = form_get_cache($form_id, $cached_form_state))) {
    form_set_error('form_token', t('Validation error, please try again. If this error persists, please contact the site administrator.'));
    $output = theme('status_messages');
    print drupal_to_js(array('status' => TRUE, 'data' => $output));
    exit();
  }

  // Just remember that $form_state is not checked, here. It's raw POST.
  $form_state = array('values' => $_POST);

  if (!empty($form_state['values']['always_autocomplete'])) {
    $new_question = $form_state['values']['always_autocomplete'];
    $frequency = 'always';
  }
  elseif (!empty($form_state['values']['random_autocomplete'])) {
    drupal_set_message('Adding random question', 'status');
    $new_question = $form_state['values']['random_autocomplete'];
    $frequency = 'random';
  }
  else {
    form_set_error('form_token', t('No question was found.'));
    $output = theme('status_messages');
    print drupal_to_js(array('status' => TRUE, 'data' => $output));
    exit();
  }

  $matches = array();
  $reg = '/id:([0-9]+)\]$/';
  preg_match($reg, $new_question, $matches);

  // Modify the form to add a new entry to weights.
  if (!empty($matches[1]) && empty($cached_form['filtered_question_list_' . $frequency]['weights'][$frequency . '-' . $matches[1]])) {
    $cached_form['filtered_question_list_' . $frequency]['weights'][$frequency . '-' . $matches[1]] = array(
      '#type' => 'textfield',
      '#size' => 3,
      '#maxlength' => 4,
      '#default_value' => 0,
    );
  }

  // Re-save the cache.
  form_set_cache($form_id, $cached_form, $cached_form_state);

  // Because rendering a new table row is not possible from the server side, we
  // need to launch a one-time JavaScript to add the item to the table.
  $output = theme('status_messages') . '<script>Quiz.addQuestion("' . $frequency . '");</script>';

  print drupal_to_js(array('status' => TRUE, 'data' => $output));
  exit;
}

// Quiz Admin Settings

/**
 * Implementation of hook_settings().
 *
 * This builds the main settings form for the quiz module.
 */
function quiz_admin_settings() {
  $form = array();

  $form['quiz_global_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Global Configuration'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t('Control aspects of the Quiz module\'s display'),
  );

  $form['quiz_global_settings']['quiz_default_close'] = array(
    '#type' => 'textfield',
    '#title' => t('Default number of days before a @quiz is closed', array('@quiz' => QUIZ_NAME)),
    '#default_value' => variable_get('quiz_default_close', 30),
    '#description' => t('Supply a number of days to calculate the default close date for new quizzes.'),
  );

  $form['quiz_global_settings']['quiz_default_pass_rate'] = array(
    '#type' => 'textfield',
    '#title' => t('Default percentage needed to pass a @quiz', array('@quiz' => QUIZ_NAME)),
    '#default_value' => variable_get('quiz_default_pass_rate', 75),
    '#description' => t('Supply a number between 1 and 100 to set as the default percentage correct needed to pass a quiz. Set to 0 if you want to ignore pass/fail summary information by default.'),
  );

  $form['quiz_global_settings']['quiz_use_passfail'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow quiz creators to set a pass/fail option when creating a @quiz.', array('@quiz' => strtolower(QUIZ_NAME))),
    '#default_value' => variable_get('quiz_use_passfail', 1),
    '#description' => t('Check this to display the pass/fail options in the @quiz form. If you want to prohibit other quiz creators from changing the default pass/fail percentage set below, uncheck this option.', array('@quiz' => QUIZ_NAME)),
  );

  $form['quiz_global_settings']['quiz_email_results'] = array(
    '#type' => 'checkbox',
    '#title' => t('E-mail users their quiz results at the end of a quiz.', array('@quiz' => strtolower(QUIZ_NAME))),
    '#default_value' => variable_get('quiz_email_results', 1),
    '#description' => t('Check this to send users quiz results over email at the end of a quiz.')
  );



  $form['quiz_global_settings']['quiz_max_result_options'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum Result Options'),
    '#description' => t('Set the maximum number of result options (categorizations for scoring a quiz).'),
    '#default_value' => variable_get('quiz_max_result_options', 5),
    '#size' => 2,
    '#maxlength' => 2,
    '#required' => FALSE,
  );

  // Added for support of actions and allowing the user to filter the actions dropdown by a value.
  $form['quiz_global_settings']['quiz_action_type'] = array(
    '#type' => 'textfield',
    '#title' => t('Default actions type'),
    '#size' => 25,
    '#default_value' => variable_get('quiz_action_type', 'all'),
    '#description' => t('Filter the actions dropdown by a specific type.'),
  );

  $form['quiz_global_settings']['quiz_remove_partial_quiz_record'] = array(
    '#type' => 'select',
    '#title' => t('Remove Incomplete Quiz Records (older than)'),
    '#options' => quiz_remove_partial_quiz_record_value(),
    '#default_value' => variable_get('quiz_remove_partial_quiz_record', '0'),
    '#description' => t('Number of days that you like to keep the incomplete quiz records'),
  );

  $target = array(
    'attributes' => array(
      'target' => '_blank'
    ),
  );

  $links = array(
    '!views' => l(t('Views'), 'http://drupal.org/project/views', $target),
    '!cck' => l(t('CCK'), 'http://drupal.org/project/cck', $target),
    '!jquery_countdown' => l(t('JQuery Countdown'), 'http://drupal.org/project/jquery_countdown', $target),
    '!userpoints' => l(t('UserPoints'), 'http://drupal.org/project/userpoints', $target),
    '@quiz' => QUIZ_NAME,
  );

  $form['quiz_addons'] = array(
    '#type' => 'fieldset',
    '#title' => t('Addons Configuration'),
    '#description' => t('Quiz module can integrate with other d.o modules like !views, !cck, !userpoints, !jquery_countdown. Here you can configure the way quiz module integrates to other modules. Disabled checkbox indicates modules are not enabled/installed', $links),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['quiz_addons']['quiz_has_userpoints'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable UserPoints Module Integration'),
    '#default_value' => variable_get('quiz_has_userpoints', 0),
    '#description' => t('!userpoints an *optional* module for quiz, provides way for users to gain or lose points for performing certain actions on your site like attending @quiz.  you will need to install the !userpoints module to use this feature.', $links),
    '#disabled' => !module_exists('userpoints'),
  );

  $form['quiz_addons']['quiz_has_timer'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display Timer for Timed Quiz'),
    '#default_value' => variable_get('quiz_has_timer', 0),
    '#description' => t('!jquery_countdown an *optional* module for quiz used for the timer to be displayed on the user\'s page, you will need to install the !jquery_countdown module. Without this timer, the user will not know how long he or she has to complete the @quiz', $links),
    '#disabled' => !function_exists('jquery_countdown_add'),
  );

  $form['quiz_look_feel'] = array(
    '#type' => 'fieldset',
    '#title' => t('Look and Feel Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t('Control aspects of the Quiz module\'s display'),
  );

  $form['quiz_look_feel']['quiz_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Display name'),
    '#default_value' => QUIZ_NAME,
    '#description' => t('Change the name of the quiz type. Do you call it <em>test</em> or <em>assessment</em> instead? Change the display name of the module to something else. Currently, it is called @quiz. By default, it is called <em>Quiz</em>.',
      array('@quiz' => QUIZ_NAME)),
    '#required' => TRUE,
  );

  $form['quiz_look_feel']['quiz_show_allowed_times'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show allowed times'),
    '#description' => t('When a user begins a new quiz, show the user the number of times they may take the test, and how many times they have already taken the test.'),
    '#default_value' => variable_get('quiz_show_allowed_times', TRUE),

  );

  $form['quiz_look_feel']['quiz_email_results_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Configure Email subject'),
    '#description' => t('This format will be used to send Email to notify results at the end of quiz.'),
    '#default_value' => variable_get('quiz_email_results_subject', quiz_email_results_format('subject')),
  );

  $form['quiz_look_feel']['quiz_email_results_body'] = array(
    '#type' => 'textarea',
    '#title' => t('Configure Email format'),
    '#description' => t('This format will be used to send Email to notify results at the end of quiz. !title !sitename !username !date  !desc !correct !total !percentage !url are placeholder.'),
    '#default_value' => variable_get('quiz_email_results_body', quiz_email_results_format('body')),
  );

  return system_settings_form($form);
}

/**
  * Validation of the Form Settings form.
  *
  * Checks the values for the form administration form for quiz settings.
  */
function quiz_settings_form_validate($form, &$form_state) {
  if (!is_numeric($form_state['values']['number_of_random_questions']) || $form_state['values']['number_of_random_questions'] <0 ) {
    form_set_error('number_of_random_questions', t('The number of random questions must be at least 0.'));
  }
  if (!is_numeric($form_state['values']['quiz_default_close']) || $form_state['values']['quiz_default_close'] <= 0) {
    form_set_error('quiz_default_close', t('The default number of days before a quiz is closed must be a number greater than 0.'));
  }
  if (!is_numeric($form_state['values']['quiz_default_pass_rate'])) {
    form_set_error('quiz_default_pass_rate', t('The pass rate value must be a number between 0% and 100%.'));
  }
  if ($form_state['values']['quiz_default_pass_rate'] > 100) {
    form_set_error('quiz_default_pass_rate', t('The pass rate value must not be more than 100%.'));
  }
  if ($form_state['values']['quiz_default_pass_rate'] < 0) {
    form_set_error('quiz_default_pass_rate', t('The pass rate value must not be less than 0%.'));
  }
}

 // DELETE QUIZ RESULTS

 /**
  * Delete Result.
  */
function quiz_admin_result_delete() {
  return drupal_get_form('quiz_admin_result_delete_form');
}

/**
 * Creates a form used for deleting a set of quiz results.
 */
function quiz_admin_result_delete_form() {
  $form['del_rid'] = array('#type' => 'hidden', '#value' => arg(2));
  return confirm_form($form,
    t('Are you sure you want to delete this @quiz result?', array('@quiz' => QUIZ_NAME)),
    'admin/quiz/results',
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

function quiz_admin_result_delete_form_submit($form, &$form_state) {
  $nid = db_result(db_query("SELECT nid FROM {quiz_node_results} WHERE result_id = %d",$form_state['values']['del_rid']));
  db_query("DELETE FROM {quiz_node_results} WHERE result_id = %d", $form_state['values']['del_rid']);
  db_query("DELETE FROM {quiz_node_results_answers} WHERE result_id = %d", $form_state['values']['del_rid']);
  drupal_set_message(t('Result has been Deleted.'));
  $form_state['redirect'] = 'admin/quiz/'. intval($nid) .'/view';
  $form_state['nid'] = $nid;
}



// THEME FUNCTIONS
// Remember to updated quiz_theme() in quiz.module

/**
 * Theme the admin quizzes table.
 *
 * @param $results
 *  As returned by _quiz_get_quizzes().
 *
 * @ingroup themeable
 */
function theme_quiz_admin_quizzes($results) {
  $output = '';
  $rows = array();

  while (list($key, $result) = each($results)) {
    $rows[] = array(
      l($result['title'], 'admin/quiz/'. $result['nid'] .'/view'),
      check_plain($result['name']),
      format_date($result['created'], 'small'),
    );
  }

  $header = array(
    t('@quiz title', array('@quiz' => QUIZ_NAME)),
    t('Created by'),
    t('Created on'));

  if (!empty($rows)) {
    $output .= theme('table', $header, $rows);
  }
  else {
    $output .= t('No @quizzes found.', array('@quiz' => QUIZ_NAME));
  }
  return $output;
}


/**
 * Theme the admin results table.
 *
 * @param $results
 *  As returned by _quiz_get_results().
 *
 * @ingroup themeable
 */
function theme_quiz_admin($results) {
  $output = '';
  $current = current($results);
  drupal_set_title(t('@current Results', array('@current' => check_plain($current['title']))));
  while (list($key, $result) = each($results)) {
    $rows[] = array(
      l(theme('image', drupal_get_path('module', 'quiz') . '/images/view.png', t('View')), 'admin/quiz/reports/' . $result['result_id'] . '/results', array('html' => TRUE)) . ' | ' . l(theme('image', drupal_get_path('module', 'quiz') . '/images/delete.png', t('View')), 'admin/quiz/' . $result['result_id'] . '/delete', array('html' => TRUE)),
      //check_plain($result['title']),
      check_plain($result['name']),
      $result['result_id'],
      format_date($result['time_start'], 'small'),
      ($result['time_end'] > 0) ? format_date($result['time_end'], 'small') : t('In Progress'),
      //($result['time_end'] > 0) ? date('i:s', $result['time_end']) - date('i:s', $result['time_start']) : 0,
      ($result['time_end']) ? $result['score'] : t('In Progress'),
    );
  }

  $header = array(
    t('Action'),
    //t('@quiz Title', array('@quiz' => QUIZ_NAME)),
    t('Username'),
    t('Result<br />ID'),
    t('Time Started'),
    t('Finished?'),
    //t('Time Taken <br /> (in sec)'),
    t('Score'),
  );

  if (!empty($rows)) {
    $output .= theme('table', $header, $rows);
  }
  else {
    $output .= t('No @quiz results found.', array('@quiz' => QUIZ_NAME));
  }
  return $output;
}

/**
 * Theme the summary page for admins.
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
function theme_quiz_admin_summary($quiz, $questions, $score, $summary) {
  // Set the title here so themers can adjust.
  drupal_set_title(check_plain($quiz->title));

  if (!$score['is_evaluated']) {
    drupal_set_message(t('This quiz has not been scored yet.'), 'error');
  }

  // Display overall result.
  $output = '';
  $params = array(
    '%num_correct' => $score['numeric_score'],
    '%question_count' => $score['possible_score'],
  );
  $output .= '<div id="quiz_score_possible">'. t('This person got %num_correct of %question_count possible points.', $params) .'</div>'."\n";
  $output .= '<div id="quiz_score_percent">'. t('Total score: @score%', array('@score' => $score['percentage_score'])) .'</div><br />'."\n";
  $output .= '<div id="quiz_summary">'. check_markup($summary, $quiz->format) .'</div><br />'."\n";
  // Get the feedback for all questions.
  $output .= theme('quiz_feedback', $questions, TRUE, TRUE);
  return $output;
}

/**
 * Theme a question selection table, adding drag and drop support.
 */
function theme_question_selection_table($form) {
  // This is a temporary hack.
  static $table_counter = 0;
  ++$table_counter;

  $question_types = _quiz_get_question_types();

  //drupal_add_tabledrag('questions-order-' . $table_counter, 'order', 'sibling', 'question-order-weight', 'question-order-weight-' . $table_counter, NULL, TRUE);
  drupal_add_tabledrag('questions-order-' . $table_counter, 'order', 'sibling', 'question-order-weight-' . $table_counter, NULL, NULL, TRUE);

  $headers = array(t('Question'), t('Type'), t('Actions'), t('Weight'));
  $rows = array();
  if (!empty($form['titles'])) {
    foreach (element_children($form['titles']) as $nid) {
      $form['weights'][$nid]['#attributes']['class'] = 'question-order-weight question-order-weight-' . $table_counter;
      $type = $form['types'][$nid]['#value'];

      $rows[] = array(
        'class' => 'draggable',
        'data' => array(
          drupal_render($form['titles'][$nid]),
          $question_types[$type]['name'],
          $form['view_links'][$nid]['#value'] . ' | ' . $form['remove_links'][$nid]['#value'],
          drupal_render($form['weights'][$nid]),
        ),
      );
      unset($form['types'][$nid], $form['view_links'][$nid], $form['remove_links'][$nid]);
    }

  }
  $table = theme('table', $headers, $rows, array('id' => 'questions-order-' . $table_counter));
  return $table . drupal_render($form);
}

function quiz_remove_partial_quiz_record_value() {
  /*$list = array();
  $list[0] = t('Never');
  for ($i=1; $i<31; $i++) {
    $list[$i * 86400] = $i . ' ' . t('Day(s)');
  }
  for ($i=35; $i<125; $i+=5) {
    $list[$i * 86400] = $i . ' ' . t('Day(s)');
  }*/
  //return $list;
  return array(
    '0' => t('Never'),
    '86400' => t('1 Day'),
    '172800' => t('2 Days'),
    '259200' => t('3 Days'),
    '345600' => t('4 Days'),
    '432000' => t('5 Days'),
    '518400' => t('6 Days'),
    '604800' => t('7 Days'),
    '691200' => t('8 Days'),
    '777600' => t('9 Days'),
    '864000' => t('10 Days'),
    '950400' => t('11 Days'),
    '1036800' => t('12 Days'),
    '1123200' => t('13 Days'),
    '1209600' => t('14 Days'),
    '1296000' => t('15 Days'),
    '1382400' => t('16 Days'),
    '1468800' => t('17 Days'),
    '1555200' => t('18 Days'),
    '1641600' => t('19 Days'),
    '1728000' => t('20 Days'),
    '1814400' => t('21 Days'),
    '1900800' => t('22 Days'),
    '1987200' => t('23 Days'),
    '2073600' => t('24 Days'),
    '2160000' => t('25 Days'),
    '2246400' => t('26 Days'),
    '2332800' => t('27 Days'),
    '2419200' => t('28 Days'),
    '2505600' => t('29 Days'),
    '2592000' => t('30 Days'),
    '3024000' => t('35 Days'),
    '3456000' => t('40 Days'),
    '3888000' => t('45 Days'),
    '4320000' => t('50 Days'),
    '4752000' => t('55 Days'),
    '5184000' => t('60 Days'),
    '5616000' => t('65 Days'),
    '6048000' => t('70 Days'),
    '6480000' => t('75 Days'),
    '6912000' => t('80 Days'),
    '7344000' => t('85 Days'),
    '7776000' => t('90 Days'),
    '8208000' => t('95 Days'),
    '8640000' => t('100 Days'),
    '9072000' => t('105 Days'),
    '9504000' => t('110 Days'),
    '9936000' => t('115 Days'),
    '10368000' => t('120 Days'),
  );
}

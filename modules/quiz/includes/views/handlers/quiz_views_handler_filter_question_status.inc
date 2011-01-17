<?php
// $Id: quiz_views_handler_filter_question_status.inc,v 1.2 2009/05/28 16:52:07 mbutcher Exp $

/*
 * @file
 * Handles filter question status.
 */
class quiz_views_handler_filter_question_status extends views_handler_filter_in_operator {
  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Question Status in Quiz');
      $this->value_options = array(
        QUESTION_RANDOM => t('Random'), // 'Random-ly' better?
        QUESTION_ALWAYS => t('Always'),
        QUESTION_NEVER => t('Never'),
      );
    }
  }
}

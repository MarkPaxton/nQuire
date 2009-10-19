<?php
// $Id: quiz_views_handler_field_question_status.inc,v 1.1.2.3 2009/05/21 19:02:13 sivaji Exp $

/**
 * @file
 * Views field handler that translates question status integer constants (as
 * defined in quiz.module) into their human-readable string counterparts.
 */
class quiz_views_handler_field_question_status extends views_handler_field {
  var $text_map = array();

  function pre_render($values) {
    $this->text_map = array(
      QUESTION_RANDOM => t('Random'), // 'Random-ly' better?
      QUESTION_ALWAYS => t('Always'),
      QUESTION_NEVER => t('Never'),
    );
  }

  function render($values) {
    return $this->text_map[$values->{$this->field_alias}];
  }
}

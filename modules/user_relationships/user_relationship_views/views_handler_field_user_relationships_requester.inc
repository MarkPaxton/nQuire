<?php

/**
 * @file
 * User Relationships Views integration.
 * Link to user profile of relationship requester
 */
class views_handler_field_user_relationships_requester extends views_handler_field_user_link {
  function construct() {
    parent::construct();
    $this->additional_fields['uid'] = 'requester_id';
  }
  
  function render($values) {
    $text = !empty($this->options['text']) ? $this->options['text'] : t('view');
    $uid = $values->{$this->aliases['requester_id']};
    return l($text, "user/$uid");
  }
  
}

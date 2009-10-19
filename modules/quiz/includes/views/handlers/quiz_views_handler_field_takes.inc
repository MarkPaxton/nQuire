<?php
// $Id: quiz_views_handler_field_takes.inc,v 1.1.2.3 2009/05/21 19:02:13 sivaji Exp $

/*
 * @file
 * Hanldes field takes.
 */
class quiz_views_handler_field_takes extends views_handler_field {
  function render($values) {
    $value = $values->{$this->field_alias};
    if ((int) $value === 0) {
      return 'Unlimited';
    }
    return $value;
  }
}

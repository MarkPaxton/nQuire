<?php
// $Id: comment.rules_forms.inc,v 1.1 2009/06/24 09:17:48 ou_pm Exp $


/**
 * @file
 * Rules configuration forms for the comment module.
 *
 * @addtogroup rules
 * @{
 */

/**
 * Action "Load a comment by id" variable label callback.
 */
function rules_action_load_comment_variable_label($settings) {
  return t('Comment with id @id', array('@id' => $settings['cid']));
}

/**
 * @}
 */
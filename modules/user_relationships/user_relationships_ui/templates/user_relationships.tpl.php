<?php

//$relationships array is loaded in template_preprocess_user_relationships()
if ($relationships) {
  foreach ($relationships as $relationship) {
    $edit_access = ($user->uid == $account->uid && user_access('maintain own relationships')) || user_access('administer users');

    $this_user_str  = $account->uid == $relationship->requestee_id ? 'requester' : 'requestee';
    $this_user      = $relationship->{$this_user_str};

    if(!empty($relationship->extra_for_display)) {
      $extra = $relationship->extra_for_display;
    } else {
      $extra = '';
    }
    $row = array(
      theme('username', $this_user),
      check_plain(ur_tt("user_relationships:rtid:$relationship->rtid:name", $relationship->name)) . ($relationship->is_oneway ? ($this_user_str == 'requestee' ? t(' (You to Them)') : t(' (Them to You)')) : NULL),
      $extra,
      $edit_access ? theme('user_relationships_remove_link', $account->uid, $relationship->rid) : '&nbsp;',
    );
    if (variable_get('user_relationships_show_user_pictures', 0)) {
      array_unshift($row, theme('user_picture', $this_user)); 
    }
    $rows[] = $row;
  }

  print
    theme('table', array(), $rows, array('class' => 'user-relationships-listing-table')) .
    theme('pager', NULL, $relationships_per_page);
}
else {
  print t('No relationships found');
}
?>

<?php
// $Id: quiz_stats_revision_selector.tpl.php,v 1.1.2.4 2010/11/05 16:14:47 falcon Exp $
print '<p>' . $content['explanation'] . '</p>' . "\n";
print '<p>';
$counter = 1;
foreach ($content['links'] as $key => $value) {
  print ' | '. l(t('revision !num', array('!num' => $counter++)), $value);
}
print ' |</p>';
?>
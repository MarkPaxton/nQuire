<?php
/**
 * @file
 * prints only $content . used for ajax loaded content.
 */
 ?>
<title><?php echo $head_title ?></title>
<?php if ($show_messages && $messages): print $messages; endif; ?>
<?php echo $styles ?>
<?php echo $scripts ?>
<?php echo $content ?>
<script>
  // validation error case.
  var formAction = $("form").attr('action');
  formAction = formAction.replace("?ajax=1", "");
  $("form").attr('action', formAction);
</script>

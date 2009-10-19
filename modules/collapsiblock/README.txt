Drupal collapsiblock.module README.txt
==============================================================================

Makes blocks collapsible.


Requirements
------------------------------------------------------------------------------
This module is written for Drupal 5.0 and requires the jstools.module to be
enabled.


Theme Support
------------------------------------------------------------------------------

Collapsiblock needs to know the page element in which block titles (subjects)
are enclosed, something that varies by theme. In many cases this will be a
<h2> or <h3> element.

To add support for a custom theme, include a file named for the theme in
collapsiblock's theme directory, e.g., bluemarine.inc for the bluemarine
theme. The contents of this file identify the jQuery selector for the title
element in blocks.

To determine this, look in the theme for the place where blocks are generated.
For PHPTemplate-based themes, look for a block.tpl.php file.

Identify the line in the template or theme file where block titles are
generated, and look for the enclosing elements. For example, in garland's
block.tpl.php, the relevant line is:

  <h2><?php print $block->subject ?></h2>

In this case, the jQuery selector will be 'h2'.

So the theme .inc file will be called garland.inc and include the following function:

<?php

function collapsiblock_theme_data() {
  return array(
    'titleSelector' => 'h2',
  );
}
Before you start (Important) :
-------------------------------

Open the page.tpl.php file for your theme, and search for 
$content.The $content should be surrounded by a div with an class 
(or id).If no div add the div yourself.

Example : <div class="region-content"><?php print $content; ?></div>

In this case, just enter ".region-content" in 'admin/settings/ajax-links-api'

How to use Ajax links API :
----------------------------

METHOD 1 : in your tpl => l_ajax($title,$path,$target)

* $title: Title.
* $path : Drupal path.
* $target (optional): ID or CLASS of DIV to be replaced. Default value is 
".region-content", you can change default value in admin page 
'admin/settings/ajax-links-api'

Example : l_ajax("add page","node/add/page",".region-content").


METHOD 2 : Add class="ajax_link" to any link. In this case target div will be 
default CSS selector defined . You can override target by specifying rel="".

Example : 
<a class="ajax_link" href="node/add/page" rel=".region-content">Add page</a>

DEMO :
-------
After enabling module goto
<YOUR SITE>/ajax-links-api/test

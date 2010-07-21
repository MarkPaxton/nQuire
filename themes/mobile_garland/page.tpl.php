<?php print '<' . '?xml version="1.0" encoding="utf-8"?' . '>' ?><!DOCTYPE html PUBLIC "-//WAPFORUM//DTD XHTML Mobile 1.0//EN" "http://www.wapforum.org/DTD/xhtml-mobile10.dtd" >
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language ?>" lang="<?php print $language->language ?>" dir="<?php print $language->dir ?>">
<head>
	<?php print $head; ?>
	<title><?php print $head_title ?></title>
	<?php print $styles; ?>
	<?php print $scripts; ?>
</head>
<body class="<?php print $bodyclasses; ?>">
	<div id="container">
		<div id="header-region"><?php print $header; ?></div>
		<div id="header" class="mobileregion">
			<?php
			if ($logo) {
				print '<a id="sitelogo" title="' . $site_slogan . '" href="' . check_url($front_page) . '"><img src="'. check_url($logo) .'" /></a>';
			}
			if ($site_name) {
				print '<h1 id="sitename"><a title="' . $site_slogan . '" href="' . check_url($front_page) . '"><span>' . $site_name . '</span></a></h1>';
			}
			?>
		</div>
		<div id="primary_links">
			<?php
			if (isset($primary_links)) {
				print theme('toplinks', $primary_links, array('class' => 'links primary-links'));
			}
			if (isset($secondary_links)) {
				print theme('toplinks', $secondary_links, array('class' => 'links secondary-links'));
			}
			?>
		</div>
		<?php print $breadcrumb; ?>
		<?php if ($left): ?>
		<div id="sidebar-left" class="mobileregion">
			<?php if ($search_box): ?><div class="block block-theme"><?php print $search_box ?></div><?php endif; ?>
			<?php print $left ?>
		</div>
		<?php endif; ?>
		<div id="maincontent" class="mobileregion">
			<?php if ($mission) print '<div id="mission">' . $mission . '</div>'; ?>
			<?php if ($title) print '<div id="main"><h2>' . $title . '</h2></div>'; ?>
			<?php if ($tabs) print '<ul class="tabs primary">' . $tabs . '</ul>'; ?>
			<?php if ($tabs2) print '<ul class="tabs secondary">' . $tabs2 . '</ul>'; ?>
			<?php if ($show_messages && $messages) print $messages; ?>
			<?php if ($help) print $help; ?>
			<div id="content">
				<?php print $content ?>
			</div>
		</div>
		<?php if ($right): ?>
		<div id="sidebar-right" class="mobileregion">
			<?php if (!$left && $search_box): ?><div class="block block-theme"><?php print $search_box ?></div><?php endif; ?>
			<?php print $right ?>
		</div>
		<?php endif; ?>
		<div id="footer" class="mobileregion">
			<?php if($feed_icons) print $feed_icons; ?>
			<?php print $footer_message . $footer; ?>
		</div>
	</div>
	<?php print $closure; ?>
</body>
</html>
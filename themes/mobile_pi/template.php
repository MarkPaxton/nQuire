<?php
// $Id: template.php,v 1.4 2009/05/26 12:58:49 teemule Exp $

/**
 * mobile_garland is a Garland inspired mobile optimized Drupal theme.
 * The theme should be used with the Mobile Plugin -module.
 */

/**
 * Preprocesses template variables.
 * @param vars variables for the template
 */
function phptemplate_preprocess_page(&$vars) {
	$vars['tabs2'] = menu_secondary_local_tasks();
	if (module_exists('color')) {
		_color_page_alter($vars);
	}
	if (module_exists('mobileplugin')) {
		$vars['styles'] = _mobileplugin_filter_css($vars['css']);
		$vars['scripts'] = _mobileplugin_filter_js();
		$vars['bodyclasses'] = 'mobile' . _mobileplugin_add_class();
		$scaled_logo = _mobileplugin_image_scaled($vars['logo'], 30, 30);
		if (!is_array($scaled_logo)) {
			$vars['logo'] = $scaled_logo;
		}
	}
}

/**
 * Override local tasks theming to split the secondary tasks.
 * @return rendered local tasks
 */
function phptemplate_menu_local_tasks() {
  return menu_primary_local_tasks();
}

/**
 * Wraps comments to a stylable element.
 * @param content the comment markup
 * @param node the node commented
 * @return full comment markup
 */
function phptemplate_comment_wrapper($content, $node) {
	if (!$content || $node->type == 'forum') {
		return '<div id="comments">' . $content . '</div>';
	}
	return '<div id="comments"><h2 class="comments">' . t('Comments') . '</h2>' . $content . '</div>';
}

/**
 * Themes comment submitted info.
 * @param comment the comment
 * @return submitted info
 */
function phptemplate_comment_submitted(&$comment) {
  return t('!datetime — !username',
    array(
      '!username' => theme('username', $comment),
      '!datetime' => format_date($comment->timestamp)
    ));
}

/**
 * Themes node submitted info.
 * @param node the node
 * @return submitted info
 */
function phptemplate_node_submitted(&$node) {
  return t('!datetime — !username',
    array(
      '!username' => theme('username', $node),
      '!datetime' => format_date($node->created),
    ));
}

/**
 * Themes feed icon.
 * @param url a feed url
 * @param title a feed title
 * @return feed markup
 */
function phptemplate_feed_icon($url, $title) {
	if ($image = theme('image', 'misc/feed.png', t('Syndicate content'), $title)) {
		return '<div class="feed-div"><a href="'. check_url($url) .'" class="feed-icon">'. $image . ' ' . $title . '</a></div>';
	}
}

/**
 * Register original theme functions.
 * @return theme function array
 */
function mobile_garland_theme() {
	return array(
		'toplinks' => array(
			'arguments' => array($links => array(), $attributes => array())
		)
	);
}

/**
 * Themes the top links.
 * @param links links data
 * @param attributes attributes to add for the element
 * @return top links markup
 */
function mobile_garland_toplinks($links, $attributes = array('class' => 'links')) {
	$num_links = count($links);
	if ($num_links == 0) {
		return '';
	}
	$output = '<div'. drupal_attributes($attributes) .'>';
	$i = 1;
	foreach ($links as $key => $link) {

		// Add active or passive class to links.
		$active = (strpos($key, 'active') ? ' active' : ' passive');
		if (isset($link['attributes']) && isset($link['attributes']['class'])) {
			$link['attributes']['class'] .= ' ' . $key;
		}
		else {
			$link['attributes']['class'] = $key;
		}

		// Add first and last classes to links.
		$extra_class = '';
		if ($i == 1) {
			$extra_class .= 'first ';
		}
		if ($i == $num_links) {
			$extra_class .= 'last ';
		}
		$output .= '<span '. drupal_attributes(array('class' => $extra_class . $key . $active)) .'>';

		// Create a link or plain title.
		$html = isset($link['html']) && $link['html'];
		$link['query'] = isset($link['query']) ? $link['query'] : NULL;
		$link['fragment'] = isset($link['fragment']) ? $link['fragment'] : NULL;
		if (isset($link['href'])) {
			$output .= l($link['title'], $link['href'], $link['attributes'], $link['query'], $link['fragment'], FALSE, $html);
		} else if ($link['title']) {
			if (!$html) {
				$link['title'] = check_plain($link['title']);
			}
			$output .= '<span'. drupal_attributes($link['attributes']) .'>'. $link['title'] .'</span>';
		}
		$output .= "</span>\n";
		$i++;
	}
	$output .= '</div>';
	return $output;
}

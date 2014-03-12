<?php

/** @file This file contains all the functions to display the nQuire home view; this is the
 * page with all the inquiries listed.
 */

/**
 * pi_home_view - Home page for PI that lists all the inquiries
 */
function pi_home_view($type) {
  global $user;
  $active_uid = $user->uid;

  $keyword = isset($_REQUEST['k']) ? $_REQUEST['k'] : '';
  $sort = isset($_REQUEST['s']) ? $_REQUEST['s'] : 'creation';
  $desc = isset($_REQUEST['o']) && $_REQUEST['o'] == 'd';
  $page = isset($_REQUEST['p']) ? intval($_REQUEST['p']) : 1;

  $active_list = $active_uid && $type === 'active';

  drupal_set_title($active_list ? t('My inquiries') : t('Available inquiries'));
  $output = '';

  if ($active_list) {
    $intro = t('<p>Here you can see the inquiries you have joined.</p><p>Click <a href="!url">here</a> to search for available inquiries.</p>', array('!url' => url('availableinquiries')));
  } else {
    $intro = t('<p>Search for available inquiries to join.</p>');
    if ($active_uid) {
      $intro .= t('<p>Click <a href="!url">here</a> to browse your active inquiries.</p>', array('!url' => url('inquiries')));
    }
  }

  $output .= $intro;



  $inquiries_for_user = pi_inquiry_groupings_get_inquiries_for_user($user->uid, $active_list);
  if (count($inquiries_for_user) === 0) {
    $output .= $active_list ? t('You are not participating in any inquiry yet.') : t('There are not available inquiries at this moment.');
  } else {
    $output .= '<form method="GET" action="' . url($active_list ? 'inquiries' : 'availableinquiries') . '">'
            . '<input type="text" name="k" value="' . $keyword . '" />'
            . '<button type="submit">Search</button>'
            . '</form>';

    $inquiries_data = array_filter(array_map(function($inquiry_nid) use($keyword) {
                      $node = node_load($inquiry_nid);
                      return $keyword && stripos($node->title, $keyword) === FALSE ?
                              FALSE :
                              array(
                          'nid' => $inquiry_nid,
                          'node' => $node,
                          'uid' => $node->uid,
                          'title' => check_plain($node->title),
                          'description' => nl2br(check_plain($node->body)),
                          'usercount' => count(nquire_group_get_member_uids($node->actor_gid, TRUE)),
                          'created' => $node->created,
                          'rating' => pi_promote_inquiry_rating($inquiry_nid),
                      );
                    }, $inquiries_for_user));

    if (count($inquiries_data) === 0) {
      $output .= '<p>' . t('No inquiry matches your search.') . '</p>';
    } else {
      switch ($sort) {
        case 'title':
          $sf = function($a, $b) {
                    return strcasecmp($a['title'], $b['title']);
                  };
          break;
        case 'users':
          $sf = function($a, $b) {
                    return $a['usercount'] - $b['usercount'];
                  };
          break;
        case 'rating':
          $sf = function($a, $b) {
                    return $a['rating'] === FALSE ?
                            ($b['rating'] === FALSE ? 0 : -1) :
                            ($b['rating'] === FALSE ? 1 : ($a['rating']['r'] - $b['rating']['r']) * 10);
                  };
          break;
        default:
          $sf = function($a, $b) {
                    return $a['created'] - $b['created'];
                  };
          break;
      }

      usort($inquiries_data, function($a, $b) use($sf, $desc) {
                $v = $sf($a, $b);
                return $desc ? -$v : $v;
              });

      if (count($inquiries_data) > 0) {

        $output .= '<p>'
                . t('Sort by: ');
        $order_options = array(
            'creation' => t('Date of creation'),
            'title' => t('Title'),
            'users' => t('Number of users'),
            'rating' => t('Rating'),
        );

        $order_links = array();

        foreach ($order_options as $order_option => $label) {
          $sort_active = $order_option == $sort;
          if ($sort_active) {
            $label .= $desc ? ' ▲' : ' ▼';
          }
          $order_links[] = _pi_home_create_link($label, $active_list, $keyword, $order_option, $sort_active && !$desc, 0);
        }

        $output .= implode(' | ', $order_links);

        $max_per_page = 5;
        $page_links_distance = 4;
        $pages = ceil(count($inquiries_data) / $max_per_page);
        if ($pages > 1) {
          $last_page = $pages;
          $show_page = max(1, min($last_page, $page));

          $from_page = max(1, $show_page - $page_links_distance);
          if ($from_page <= 3) {
            $from_page = 1;
          } else if ($from_page > 2) {
            $show_first_page = TRUE;
          }
          $to_page = min($last_page, $show_page + $page_links_distance);
          if ($to_page >= $last_page - 2) {
            $to_page = $last_page;
          } else {
            $show_last_page = TRUE;
          }

          $show_previous = $show_page > 1;
          $show_next = $show_page < $last_page;

          $paginator = '';
          
          $paginator_item = function($n, $label) use($show_page, $active_list, $keyword, $sort, $desc) {
            return $n !== FALSE && $n != $show_page ? _pi_home_create_link($label, $active_list, $keyword, $sort, $desc, $n, 'nquire-paginator-item') : 
              "<span class='nquire-paginator-active'>" . check_plain($label) . "</span>";
          };

          $paginator .= $paginator_item($show_previous ? $show_page - 1 : FALSE, '<');
          
          if ($show_first_page) {
            $paginator .= $paginator_item(1, '1') . '...';
          }
          for ($i = $from_page; $i <= $to_page; $i++) {
            $paginator .= $paginator_item($i, $i);
          }
          if ($show_last_page) {
            $paginator .= '...' . $paginator_item($last_page, $last_page);
          }
          $paginator .= $paginator_item($show_next ? $show_page + 1 : FALSE, '>');

          $output .= '<p>' . $paginator . '</p>';

          $from_inquiry = $max_per_page * ($show_page - 1);
          $to_inquiry = min($from_inquiry + $max_per_page, count($inquiries_data));
        } else {
          $from_inquiry = 0;
          $to_inquiry = count($inquiries_data);
        }

        for ($i = $from_inquiry; $i < $to_inquiry; $i++) {
          $inquiry_data = $inquiries_data[$i];
          $content = '';

          if ($inquiry_data['node']->image) {
            $image = file_create_url($inquiry_data['node']->image);
            $content .= '<img src="' . $image . '" style="float:right;border: 1px solid gray;background: white;margin: 0 0 10px 10px;" height="75px" />';
          }

          $content .= '<p>' . l($inquiry_data['title'], "inquiry/{$inquiry_data['nid']}", array('attributes' => array('style' => 'font-size: 1.5em;font-weight: bold;'))) . '</p>'
                  . '<p>' . $inquiry_data['description'] . '</p>'
                  . '<small>' . t('Created by !user. ', array('!user' => l(nquire_commons_user_name_or_me($inquiry_data['uid']), 'user/' . $inquiry_data['uid'], array('html' => TRUE)))) . '<br/>'
                  . t('Number of participants: !n', array('!n' => $inquiry_data['usercount'])) . '</small>';

          if ($user->uid) {
            $user_status = nquire_group_get_user_status_in_group($inquiry_data['node']->actor_gid, $user->uid);
            if ($user_status !== 'member') {
              $content .= theme('pi_inquiry_manager_participants_group_status_and_options', $inquiry_data['node'], $user->uid);
            }
          }


          $output .= '<div style="padding:10px;border: 1px solid gray;margin-bottom: 10px;background: #eee;">' . $content . '</div>';
        }
      }
    }
  }

  return $output;
}

function _pi_home_create_link($title, $active_list, $keyword, $sort, $desc, $page, $classname='') {
  $args = array('query' => array(), 'attributes' => array('class' => $classname));
  if ($keyword) {
    $args['query']['k'] = $keyword;
  }
  if ($sort) {
    $args['query']['s'] = $sort;
  }
  if ($desc) {
    $args['query']['o'] = 'd';
  }
  if ($page) {
    $args['query']['p'] = $page;
  }

  return l($title, $active_list ? 'inquiries' : 'availableinquiries', $args);
}

function _pi_home_inquiry_presentation_in_list($inquiry_data, $uid) {
  $output = l($inquiry_data['title'], "inquiry/{$inquiry_data['nid']}", array('attributes' => array('style' => 'font-size: 1.5em;font-weight: bold;')))
          . '<div style="padding: .5em 0 .5em 1em;">' . $inquiry_data['description'] . '</div>'
          . '<div style="padding-left: 1em;">'
          . '<small>' . t('Created by !user', array('!user' => l(nquire_commons_user_name_or_me($inquiry_data['uid']), 'user/' . $inquiry_data['uid'], array('html' => TRUE)))) . '<br/>'
          . t('Number of participants: !n', array('!n' => $inquiry_data['usercount'])) . '</small>'
          . '</div>';

  if ($uid) {
    return array($output, theme('pi_inquiry_manager_participants_group_status_and_options', $inquiry_data['node'], $uid));
  } else {
    return array($output, FALSE);
  }
}

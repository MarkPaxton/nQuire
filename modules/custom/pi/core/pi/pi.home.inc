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

          if ($show_previous) {
            $paginator .= _pi_home_create_link('<', $active_list, $keyword, $sort, $desc, $show_page - 1) . '&nbsp;&nbsp;';
          } else {
            $paginator .= '<span style="color:gray">&lt;</span>&nbsp;&nbsp;';
          }
          if ($show_first_page) {
            $paginator .= _pi_home_create_link('1', $active_list, $keyword, $sort, $desc, 1) . '&nbsp;...';
          }
          for ($i = $from_page; $i <= $to_page; $i++) {
            if ($i == $show_page) {
              $paginator .= '&nbsp;' . $i . '&nbsp;';
            } else {
              $paginator .= '&nbsp;' . _pi_home_create_link($i, $active_list, $keyword, $sort, $desc, $i) . '&nbsp;';
            }
          }
          if ($show_last_page) {
            $paginator .= '&nbsp;...&nbsp;' . _pi_home_create_link($last_page, $active_list, $keyword, $sort, $desc, $last_page);
          }
          if ($show_next) {
            $paginator .= '&nbsp;&nbsp;' . _pi_home_create_link('>', $active_list, $keyword, $sort, $desc, $show_page + 1);
          } else {
            $paginator .= '&nbsp;&nbsp;<span style="color:gray">&gt;</span>';
          }


          $output .= '<p>' . $paginator . '</p>';

          $from_inquiry = $max_per_page * ($show_page - 1);
          $to_inquiry = min($from_inquiry + $max_per_page, count($inquiries_data));
        } else {
          $from_inquiry = 0;
          $to_inquiry = count($inquiries_data);
        }

        $header = array(t('Inquiry'), $active_list ? t('Status') : t('Join'));

        $rows = array();
        for ($i = $from_inquiry; $i < $to_inquiry; $i++) {
          $rows[] = _pi_home_inquiry_presentation_in_list($inquiries_data[$i], $active_uid);
        }

        $output .= nquire_commons_create_not_expanding_table_block(theme('table', $header, $rows));
      }
    }
  }

  return $output;
}

function _pi_home_create_link($title, $active_list, $keyword, $sort, $desc, $page) {
  $args = array('query' => array());
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
    return array($output);
  }
}

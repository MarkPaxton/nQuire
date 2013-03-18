<?php

function pi_inquiry_manager_participants($nid) {
	global $user;

	$inquiry = node_load($nid);
	$output = theme('pi_inquiry_manager_participants_view_inquiry_group', $inquiry);

	$output .= theme('pi_inquiry_manager_participants_list', $inquiry->actor_gid, $user->uid);

	return $output;
}

function theme_pi_inquiry_manager_participants_view_inquiry_group($inquiry_node) {

	$output = '';

	$group = NULL;
	$inquiry_selection = pi_inquiry_groupings_get_selection_type($inquiry_node);
	switch ($inquiry_selection) {
		case 'justme':
			$output .= t('This inquiry is just for myself.');
			break;
		case 'group':
			$cohort = node_load(pi_inquiry_groupings_get_cohort_group($inquiry_node));
			og_load_group($cohort);

			$output .= t('This inquiry is for members of group: <b>@group</b>.', array('@group' => $cohort->og_description));
			$group = pi_inquiry_groupings_get_inquiry_group($inquiry_node);
			break;
		case 'anyone':
			$output .= t('This inquiry is for anyone.');
			$group = pi_inquiry_groupings_get_inquiry_group($inquiry_node);
			break;
	}

	if ($group) {
		$output .= '<br/>';

		$group_selection = pi_inquiry_groupings_get_join_mode($group);
		switch ($group_selection) {
			case 0:
				$output .= $inquiry_selection === 'group' ? t('Access to the inquiry is open for group members.') : t('Access to the inquiry is open.');
				break;
			case 1:
				$output .= t('Access needs to be approved by the inquiry owner.');
				break;
			case 2:
				$output .= t('Access is through invitation only.');
				break;
		}
	}

	if (pi_inquiry_manager_participants_group_edit_access($inquiry_node->nid)) {
		$output .= '<br/>' . l(t('Edit'), "inquiry/{$inquiry_node->nid}/participants/access");
	}
	return $output;
}

function pi_inquiry_manager_participants_edit_inquiry_group($inquiry_nid) {
	$inquiry_node = node_load($inquiry_nid);
	return drupal_get_form('pi_inquiry_manager_participants_edit_inquiry_group_form', $inquiry_node);
}

function pi_inquiry_manager_participants_edit_inquiry_group_form(&$form_state, $inquiry_node) {
	$form = array();

	ahah_helper_register($form, $form_state);

	nquire_commons_load_ahah_fix();

	$current_participants_selection = $inquiry_node->participants_selection;
	$participants_selection = nquire_commons_form_node_default_value($inquiry_node, 'participants_selection', $form_state, 'target', 'justme');

	$has_changed = $current_participants_selection !== $participants_selection;

	$form['target'] = array(
			'#type' => 'select',
			'#title' => t('Who can join this inquiry'),
			'#description' => t('Select the group of users that may become participants of this inquiry.'),
			'#options' => array('justme' => t('Just me'), 'group' => t('Members of a group'), 'anyone' => t('Anyone')),
			'#default_value' => $participants_selection,
			'#ahah' => array(
					'path' => ahah_helper_path(array('target_dependent_section')),
					'wrapper' => 'target_dependent_section',
					'method' => 'replace',
					'effect' => 'fade',
			),
			'#required' => TRUE,
	);

	nquire_commons_create_form_wrapped_section($form, 'target_dependent_section');


	$participants_cohort = nquire_commons_form_node_default_value($inquiry_node, 'participants_cohort', $form_state, 'cohort_group', 0);

	if ($participants_selection === 'group') {
		$groups = array('' => t('- Select group')) + pi_inquiry_groupings_get_normal_groups();

		$form['target_dependent_section']['wrapped_content']['cohort_group'] = array(
				'#type' => 'select',
				'#title' => t('User group'),
				'#description' => t('Select the group whose participants may join this inquiry.'),
				'#options' => $groups,
				'#default_value' => $participants_cohort,
				'#required' => TRUE,
				'#ahah' => array(
						'path' => ahah_helper_path(array('target_dependent_section', 'wrapped_content', 'cohort_dependent_section')),
						'wrapper' => 'cohort_dependent_section',
						'method' => 'replace',
						'effect' => 'fade',
				),
		);

		if (!$has_changed) {
			$has_changed = $participants_cohort !== $inquiry_node->participants_cohort;
		}
	}


	_pi_inquiry_manager_participants_create_group_join_mode_form_item($form['target_dependent_section']['wrapped_content'], $form_state, $inquiry_node->actor_gid, 'join_mode', t('How to join the inquiry'), t('Select how users can join this inquiry.'), $participants_selection === 'justme');

	nquire_commons_create_form_wrapped_section($form['target_dependent_section']['wrapped_content'], 'cohort_dependent_section');
	if ($has_changed) {
		$to_be_expelled = _pi_inquiry_manager_participants_get_participants_to_be_expelled($inquiry_node, $participants_selection, $participants_cohort);

		if (count($to_be_expelled) > 0) {
			$message = '<div><p>' . t('The following users will be expelled from the inquiry:') . '</p>'
							. theme_item_list(array_map(function($uid) {
																$u = user_load($uid);
																return check_plain($u->name);
															}, $to_be_expelled))
							. '<div>';

			$form['target_dependent_section']['wrapped_content']['cohort_dependent_section']['wrapped_content'] = array(
					'#value' => $message,
			);
		}
	}


	nquire_commons_create_form_save_cancel_buttons($form, 'pi_inquiry_manager_participants_edit_inquiry_group_form_submit', "inquiry/{$inquiry_node->nid}/participants");

	return $form;
}

function _pi_inquiry_manager_participants_get_participants_to_be_expelled($inquiry_node, $new_participant_selection, $new_participant_cohort) {
	$to_be_expelled = array();
	$new_allowed_members = pi_inquiry_groupings_get_allowed_participants($new_participant_selection, $inquiry_node->uid, $new_participant_cohort);
	if ($new_allowed_members !== FALSE) {
		$current_members = pi_inquiry_groupings_get_group_member_uids($inquiry_node->actor_gid);
		foreach ($current_members as $member_uid) {
			if (!in_array($member_uid, $new_allowed_members)) {
				$to_be_expelled[] = $member_uid;
			}
		}
	}
	return $to_be_expelled;
}

function pi_inquiry_manager_participants_edit_inquiry_group_form_submit($form, $form_state) {
	$inquiry_nid = arg(1);
	if (pi_inquiry_manager_participants_group_edit_access($inquiry_nid)) {
		$inquiry = node_load($inquiry_nid);
		$new_participant_selection = $form_state['values']['target'];
		$new_participant_cohort = $form_state['values']['cohort_group'];

		//$to_be_expelled = _pi_inquiry_manager_participants_get_participants_to_be_expelled($inquiry, $new_participant_selection, $new_participant_cohort);

		$inquiry->participants_selection = $new_participant_selection;
		$inquiry->participants_cohort = $new_participant_cohort;

		$inquiry->revision = TRUE;
		node_save($inquiry);


		_pi_inquiry_manager_participants_process_join_mode_form_value($inquiry->actor_gid, $form_state['values']['join_mode']);
	}

	nquire_commons_form_close($form, $form_state);
}

function _pi_inquiry_manager_participants_create_group_join_mode_form_item(&$form, $form_state, $group_nid, $id, $title, $description, $closed = FALSE) {
	if ($closed) {
		
	} else {
		$og = node_load($group_nid);
		og_load_group($og);
		$selective = nquire_commons_form_node_default_value($og, 'og_selective', $form_state, $id, '');

		if ($selective === OG_CLOSED) {
			$selective = '';
		}

		$form[$id] = array(
				'#type' => 'select',
				'#title' => $title,
				'#description' => $description,
				'#options' => array('' => t('- Select join mode'), OG_MODERATED => t('By application (approval needed)'), OG_OPEN => t('By application (automatic approval)'), OG_INVITE_ONLY => t('By invitation from owner')),
				'#default_value' => $selective,
				'#required' => TRUE,
		);
	}
}

function _pi_inquiry_manager_participants_process_join_mode_form_value($group_nid, $value) {
	$og = node_load($group_nid);
	og_load_group($og);
	$og->og_selective = $value;
	node_save($og);
}

/* Participant list */

function theme_pi_inquiry_manager_participants_list($group_nid, $uid, $owner_can_leave = FALSE) {
	$og = node_load($group_nid);
	og_load_group($og);

	$join_mode = $og->og_selective;
	$user_status = pi_inquiry_groupings_get_user_status_in_group($group_nid, $uid);
	$is_owner = $uid === $og->uid;

	$members = pi_inquiry_groupings_get_group_member_uids($group_nid);
	usort($members, function($a, $b) {
						return $a === $uid ? -1 : ($b === $uid ? 1 : 0);
					});

	$members_data = array_map(function($_uid) use ($group_nid, $uid) {
						if ($uid === $_uid) {
							$n = t('Me');
						} else {
							$u = user_load($_uid);
							$n = check_plain($u->name);
						}
						return array(
								'uid' => $_uid,
								'name' => $n,
								'status' => pi_inquiry_groupings_get_user_status_in_group($group_nid, $_uid),
						);
					}, $members);

	$rows = array();

	foreach ($members_data as $member) {
		$its_me = $uid === $member['uid'];

		$status = '';
		$buttons = array();
		if ($member['status'] === 'member') {
			$status .= t('Active');
			if ($member['uid'] === $og->uid) {
				$status .= '&nbsp;' . t('(owner)');
			}

			if ($its_me) {
				if (!$is_owner || $owner_can_leave) {
					$buttons[] = l(t('leave'), '');
				}
			} else {
				if ($is_owner) {
					$buttons[] = l(t('expel'), '');
				}
			}
		} else if ($member['status'] === 'awaiting') {
			if ($join_mode === OG_MODERATED) {
				$status .= t('Awaiting approval');
				if ($is_owner) {
					$buttons[] = l(t('accept'), '');
					$buttons[] = l(t('reject'), '');
				} else if ($its_me) {
					$buttons[] = l(t('cancel request'), '');
				}
			} else if ($join_mode === OG_INVITE_ONLY) {
				$status .= t('Invitation sent');
				if ($is_owner) {
					$buttons[] = l(t('cancel invitation'), '');
				} else if ($its_me) {
					$buttons[] = l(t('accept'), '');
					$buttons[] = l(t('refuse'), '');
				}
			}
		}

		if (count($buttons) > 0) {
			$status .= '<br/><small>' . implode('&nbsp;&nbsp;&nbsp;', $buttons) . '</small>';
		}

		$rows[] = array($member['name'], $status);
	}

	$header = array(t('User'), t('Status'));

	return theme_table($header, $rows);
}

function theme_pi_inquiry_manager_participants_group_status_and_options($gid, $uid, $owner_can_leave = FALSE) {
	global $user;

	$og = node_load($gid);
	og_load_group($og);

	$its_me = $uid === $user->uid;
	$im_owner = $og->uid === $user->uid;
	$its_owner = $uid === $og->uid;
	$join_mode = $og->og_selective;
	$user_status = pi_inquiry_groupings_get_user_status_in_group($gid, $uid);

	$status = '';
	$buttons = array();
	if ($user_status === 'member') {
		$status .= t('Active');
		if ($its_owner) {
			$status .= '&nbsp;' . t('(owner)');
		}

		if ($its_me) {
			if (!$im_owner || $owner_can_leave) {
				$buttons[] = l(t('leave'), '');
			}
		} else {
			if ($im_owner) {
				$buttons[] = l(t('expel'), '');
			}
		}
	} else if ($user_status === 'awaiting') {
		if ($join_mode == OG_MODERATED) {
			$status .= t('Awaiting approval');
			if ($im_owner) {
				$buttons[] = l(t('accept'), '');
				$buttons[] = l(t('reject'), '');
			} else if ($its_me) {
				$buttons[] = l(t('cancel request'), '');
			}
		} else if ($join_mode == OG_INVITE_ONLY) {
			$status .= t('Invitation sent');
			if ($im_owner) {
				$buttons[] = l(t('cancel invitation'), '');
			} else if ($its_me) {
				$buttons[] = l(t('accept'), '');
				$buttons[] = l(t('refuse'), '');
			}
		}
	} else {
		if ($join_mode == OG_OPEN) {
			$buttons[] = l(t('join'), '');
		} else if ($join_mode == OG_MODERATED) {
			$buttons[] = l(t('request membership'), '');
		} 
	}

	if (count($buttons) > 0) {
		$status .= '<br/><small>' . implode('&nbsp;&nbsp;&nbsp;', $buttons) . '</small>';
	}
	
	return $status;
}


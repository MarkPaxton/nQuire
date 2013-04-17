<?php

function pi_inquiry_manager_participants($nid) {
	$inquiry = node_load($nid);
	$output = theme('pi_inquiry_manager_participants_view_inquiry_group', $inquiry);

	$output .= theme('pi_inquiry_manager_participants_list', $inquiry);

	$output .= theme('pi_inquiry_manager_participants_roles', $inquiry);

	$output .= theme('pi_inquiry_manager_participants_groups', $inquiry);

	return $output;
}

function theme_pi_inquiry_manager_participants_view_inquiry_group($inquiry_node) {

	$output = '';

	$gid = NULL;
	$inquiry_selection = pi_inquiry_groupings_get_selection_type($inquiry_node);
	switch ($inquiry_selection) {
		case 'justme':
			$output .= t('This inquiry is just for myself.');
			break;
		case 'group':
			$cohort = node_load(pi_inquiry_groupings_get_cohort_og($inquiry_node));
			og_load_group($cohort);

			$output .= t('This inquiry is for members of group: <b>@group</b>.', array('@group' => $cohort->og_description));
			$gid = pi_inquiry_groupings_get_inquiry_group($inquiry_node);
			break;
		case 'anyone':
			$output .= t('This inquiry is for anyone.');
			$gid = pi_inquiry_groupings_get_inquiry_group($inquiry_node);
			break;
	}

	if ($gid) {
		$group = node_load($gid);
		$output .= '<br/>';

		switch ($group->selective) {
			case OG_OPEN:
				$output .= $inquiry_selection === 'group' ? t('Access to the inquiry is open for group members.') : t('Access to the inquiry is open.');
				break;
			case OG_MODERATED:
				$output .= t('Access needs to be approved by the inquiry owner.');
				break;
			case OG_INVITE_ONLY:
				$output .= t('Access is through invitation only.');
				break;
		}
	}

	if (pi_inquiry_manager_participants_group_configure_inquiry_participants_access($inquiry_node->nid)) {
		$output .= '<br/>' . _pi_inquiry_manager_partipants_group_operation_link(t('Edit'), $inquiry_node->nid, FALSE, "access");
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
		$current_members = nquire_group_get_member_uids($inquiry_node->actor_gid);
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
	if (pi_inquiry_manager_participants_group_configure_inquiry_participants_access($inquiry_nid)) {
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
		$form[$id] = array(
				'#value' => '<p>' . t('Group members are managed by the inquiry owner.') . '</p>',
		);
	} else {
		$group = node_load($group_nid);
		$selective = nquire_commons_form_node_default_value($group, 'selective', $form_state, $id, '');

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
	nquire_group_set_join_mode($group_nid, $value);
}

/* Participant list */

function theme_pi_inquiry_manager_participants_list($inquiry_node, $group_nid = FALSE, $owner_can_leave = FALSE) {
	$group_check = pi_inquiry_groupings_check_group($inquiry_node, $group_nid);
	if ($group_check) {
		global $user;

		$im_owner = $group_check['manager'] === $user->uid;

		$roles = pi_inquiry_groupings_get_inquiry_role_names($inquiry_node);
		$has_roles = count($roles) > 0;

		$members = nquire_group_get_member_uids($group_check['gid']);
		$members_data = array_map(function($uid) use ($group_check, $user) {
							$u = user_load($uid);
							return array(
									'uid' => $uid,
									'status' => nquire_group_get_user_status_in_group($group_check['gid'], $uid),
									'name' => $user->uid === $uid ? t('Me') : check_plain($u->name),
							);
						}, $members);

		usort($members_data, function($a, $b) use($user) {
							if ($a['uid'] === $user->uid) {
								return -1;
							} else if ($b['uid'] === $user->uid) {
								return 1;
							}

							if ($a['status'] !== $b['status']) {
								if ($a['status'] === 'member') {
									return -1;
								} else if ($b['status'] === 'member') {
									return 1;
								}
							}

							return strcmp($a['name'], $b['name']);
						});

		$rows = array();

		foreach ($members_data as $member_data) {
			$status = theme('pi_inquiry_manager_participants_group_status_and_options', $inquiry_node, $member_data['uid'], $group_nid, $owner_can_leave);
			$row = array($member_data['name'], $status);

			if ($has_roles && $member_data['status'] === 'member') {
				$user_roles = nquire_group_get_user_roles($group_check['gid'], $member_data['uid']);
				$user_role_list = array_map(function($role_id) use ($roles, $inquiry_node, $group_nid, $member_data, $im_owner) {
									$role_output = $roles[$role_id];
									if ($im_owner) {
										$role_output .= '<small>&nbsp;&nbsp;&nbsp;&nbsp;'
														. _pi_inquiry_manager_partipants_group_operation_link(t('remove'), $inquiry_node->nid, $group_nid, array('removeuserrole', $member_data['uid'], $role_id))
														. '</small>';
									}
									return $role_output;
								}, $user_roles);

				$user_role_list_output = theme_item_list($user_role_list);
				if ($im_owner) {
					$user_role_list_output .= '<div><small>'
									. _pi_inquiry_manager_partipants_group_operation_link(t('Add role'), $inquiry_node->nid, $group_nid, array('addrolestouser', $member_data['uid']))
									. '</small></div>';
				}
				$row[] = $user_role_list_output;
			}

			$rows[] = $row;
		}

		$header = array(t('User'), t('Status'));
		if ($has_roles) {
			$header[] = t('Roles');
		}

		$output = nquire_commons_create_not_expanding_table_block(theme_table($header, $rows));

		if ($group_check['group']->selective == OG_INVITE_ONLY && pi_inquiry_manager_participants_group_manageuser_access($inquiry_node->nid, $group_nid)) {
			$label = $group_check['normal_management'] ? t('Invite users') : t('Add users');
			$output .= '<p>'
							. _pi_inquiry_manager_partipants_group_operation_link($label, $inquiry_node->nid, $group_nid, 'invite')
							. '</p>';
		}

		return $output;
	}
}

function _pi_inquiry_manager_partipants_group_operation_link($label, $inquiry_nid, $group_nid = FALSE, $elements = FALSE, $add_destination = TRUE) {
	if ($add_destination) {
		return l($label, _pi_inquiry_manager_partipants_group_operation_path($inquiry_nid, $group_nid, $elements), array('query' => drupal_get_destination()));
	} else {
		return l($label, _pi_inquiry_manager_partipants_group_operation_path($inquiry_nid, $group_nid, $elements));
	}
}

function _pi_inquiry_manager_partipants_group_operation_path($inquiry_nid, $group_nid, $elements) {
	$path = "inquiry/$inquiry_nid/participants";
	if ($group_nid) {
		$path .= '/' . $group_nid;
	}
	if ($elements) {
		if (is_array($elements)) {
			$path .= '/' . implode('/', $elements);
		} else {
			$path .= '/' . $elements;
		}
	}
	return $path;
}

function theme_pi_inquiry_manager_participants_group_status_and_options($inquiry_node, $uid, $group_nid = FALSE, $owner_can_leave = FALSE) {
	$group_check = pi_inquiry_groupings_check_group($inquiry_node, $group_nid);
	if ($group_check) {

		global $user;

		$im_owner = $group_check['manager'] === $user->uid;

		$its_me = $uid === $user->uid;
		$its_owner = $uid === $group_check['manager'];
		$join_mode = $group_check['group']->selective;
		$user_status = nquire_group_get_user_status_in_group($group_check['gid'], $uid);

		$status = '';
		$buttons = array();

		$path = function($op, $uid = FALSE) use ($inquiry_node, $group_nid) {
							return _pi_inquiry_manager_partipants_group_operation_path($inquiry_node->nid, $group_nid, $uid ? array($op, $uid) : $op);
						};

		$link_params = array('query' => drupal_get_destination());

		if ($user_status === 'member') {
			$status .= t('Active');
			if ($its_owner) {
				$status .= '&nbsp;' . t('(owner)');
			}

			if ($its_me) {
				if ($group_check['normal_management'] && (!$im_owner || $owner_can_leave)) {
					$buttons[] = l(t('leave'), $path('leave'), $link_params);
				}
			} else {
				if ($im_owner) {
					$buttons[] = l(t('expel'), $path('reject', $uid), $link_params);
				}
			}
		} else if ($user_status === 'awaiting') {
			if ($group_check['normal_management']) {
				if ($join_mode == OG_MODERATED) {
					$status .= t('Awaiting approval');
					if ($im_owner) {
						$buttons[] = l(t('accept'), $path('accept', $uid), $link_params);
						$buttons[] = l(t('reject'), $path('reject', $uid), $link_params);
					} else if ($its_me) {
						$buttons[] = l(t('cancel request'), $path('leave'), $link_params);
					}
				} else if ($join_mode == OG_INVITE_ONLY) {
					$status .= t('Invitation sent');
					if ($im_owner) {
						$buttons[] = l(t('cancel invitation'), $path('reject', $uid), $link_params);
					} else if ($its_me) {
						$buttons[] = l(t('accept'), $path('acceptinvitation'), $link_params);
						$buttons[] = l(t('decline'), $path('leave'), $link_params);
					}
				}
			} else {
				$status .= 'error';
			}
		} else {
			if ($its_me && pi_inquiry_manager_participants_group_join_access($inquiry_node->nid, $group_nid)) {
				if ($join_mode == OG_OPEN) {
					$buttons[] = l(t('join'), $path('join'), $link_params);
				} else if ($join_mode == OG_MODERATED) {
					$buttons[] = l(t('request membership'), $path('join'), $link_params);
				}
			}
		}

		if (count($buttons) > 0) {
			$status .= '<br/><small>' . implode('&nbsp;&nbsp;&nbsp;', $buttons) . '</small>';
		}

		return $status;
	} else {
		return 'bad group!';
	}
}

/* roles list */

function theme_pi_inquiry_manager_participants_roles($inquiry_node, $group_nid = FALSE) {
	$output = '';

	$group_check = pi_inquiry_groupings_check_group($inquiry_node, $group_nid);
	if ($group_check) {

		$roles_data = pi_inquiry_groupings_get_inquiry_roles($inquiry_node);
		$collaboration_type = $group_nid ? 'group' : 'all';


		$phase_nids = pi_phase_load_nids_for_inquiry_and_collaboration_type($inquiry_node->nid, $collaboration_type);

		$role_phase_activities = array();

		$roles_used = FALSE;

		foreach ($phase_nids as $phase_nid) {
			$phase = node_load($phase_nid);

			$activity_nids = pi_get_activities_for_phase($phase_nid);
			$activity_titles = array();
			foreach ($activity_nids as $nid) {
				$a = node_load($nid);
				$activity_titles[$nid] = check_plain($a->title);
			}

			foreach ($roles_data as $role) {
				$activities_for_role = array_filter($activity_nids, function($aid) use($role) {
									return $role['activities'][$aid] === 'edit';
								});
				if (count($activities_for_role) > 0) {
					$role_phase_activities[$role['id']][$phase_nid] = array(
							'data' => check_plain($phase->title),
							'children' => array_map(function($aid) use ($activity_titles) {
												return $activity_titles[$aid];
											}, $activities_for_role),
					);

					$roles_used = TRUE;
				}
			}
		}

		if ($roles_used) {
			$header = array(t('Role'), t('Responsible for'), t('Participants'));
			$rows = array();
			foreach ($roles_data as $role) {
				$title = $role['title'];
				$activities = theme_item_list($role_phase_activities[$role['id']]);

				$users = nquire_group_get_participants_with_role($group_check['gid'], $role['id']);
				$im_manager = pi_inquiry_manager_participants_group_manageuser_access($inquiry_node->nid, $group_nid);

				$user_list = array_map(function($uid) use ($inquiry_node, $group_nid, $im_manager, $role) {
									$user = user_load($uid);
									$output = check_plain($user->name);
									if ($im_manager) {
										$output .= '<small>&nbsp;&nbsp;&nbsp;'
														. _pi_inquiry_manager_partipants_group_operation_link(t('remove'), $inquiry_node->nid, $group_nid, array('removeuserrole', $uid, $role['id']))
														. '</small>';
									}
									return $output;
								}, $users);
				$user_html = theme_item_list($user_list);
				if ($im_manager) {
					$user_html .= _pi_inquiry_manager_partipants_group_operation_link(t('Add participant'), $inquiry_node->nid, $group_nid, array('adduserstorole', $role['id']));
				}

				$rows[] = array($title, $activities, $user_html);
			}

			$output .= nquire_commons_create_not_expanding_table_block(theme_table($header, $rows));
		}
	}

	return $output;
}

function pi_inquiry_manager_participants_group_adduserstorole($inquiry_nid, $role_id, $group_nid = FALSE) {
	$output = drupal_get_form('pi_inquiry_manager_participants_group_adduserstorole_form', $inquiry_nid, $role_id, $group_nid);
	return $output;
}

function pi_inquiry_manager_participants_group_adduserstorole_form($form_state, $inquiry_nid, $role_id, $group_nid) {
	$form = array();

	ahah_helper_register($form, $form_state);
	nquire_commons_load_ahah_fix();

	$inquiry = node_load($inquiry_nid);
	$group_check = pi_inquiry_groupings_check_group($inquiry, $group_nid);
	if ($group_check) {

		$text_params = array(
				'@group' => $group_nid ?
								t("group '@title'", array('@title' => $group_check['group']->title)) :
								t("inquiry '@title'", array('@title' => $inquiry->title)),
				'@role' => pi_inquiry_groupings_get_inquiry_role_name($inquiry, $role_id),
		);

		$all_group_members = nquire_group_get_member_uids($group_check['gid'], TRUE);
		$reject_members = nquire_group_get_participants_with_role($group_check['gid'], $role_id);
		$available_members = array_filter($all_group_members, function($uid) use ($reject_members) {
							return !in_array($uid, $reject_members);
						});

		drupal_set_title(t("Add users to role '@role' in @group", $text_params));

		_pi_inquiry_manager_participants_group_create_search_users_form_item($form, $form_state, $available_members, FALSE, t('Select users to add role'), t("The selected users will be assigned role '@role'.", $text_params));
		nquire_commons_create_form_save_cancel_buttons($form, 'pi_inquiry_manager_participants_group_adduserstorole_form_submit', $_REQUEST['destination'], t('Add users'));
	}

	return $form;
}

function _pi_inquiry_manager_participants_form_submit_decode_path($a = FALSE, $ai = 0, $b = FALSE, $bi = 0) {
	$result = array('inquiry_nid' => arg(1));
	if (is_numeric(arg(3))) {
		$delta = 1;
		$result['group_nid'] = arg(3);
	} else {
		$delta = 0;
		$result['group_nid'] = FALSE;
	}

	if ($a) {
		$result[$a] = arg($delta + $ai);
		if ($b) {
			$result[$b] = arg($delta + $bi);
		}
	}

	return $result;
}

function pi_inquiry_manager_participants_group_adduserstorole_form_submit($form, $form_state) {
	$ids = _pi_inquiry_manager_participants_form_submit_decode_path('role_id', 4);

	$inquiry = node_load($ids['inquiry_nid']);
	$group_check = pi_inquiry_groupings_check_group($inquiry, $ids['group_nid']);
	if ($group_check && isset($form_state['values']['userlist'])) {
		foreach ($form_state['values']['userlist'] as $uid => $add) {
			if ($add) {
				nquire_group_add_user_role($group_check['gid'], $uid, $ids['role_id']);
			}
		}
	}

	drupal_goto($_REQUEST['destination']);
}

function pi_inquiry_manager_participants_group_addrolestouser($inquiry_nid, $uid, $group_nid = FALSE) {
	$user = user_load($uid);
	if (!$user) {
		return '<p>' . t('Not a valid user.') . '</p>';
	}

	$inquiry = node_load($inquiry_nid);
	$group_check = pi_inquiry_groupings_check_group($inquiry, $group_nid);
	if (!$group_check) {
		return '<p>' . t('Not a valid inquiry/group.') . '</p>';
	}

	$text_params = array(
			'@group' => $group_nid ?
							t("group '@title'", array('@title' => $group_check['group']->title)) :
							t("inquiry '@title'", array('@title' => $inquiry->title)),
			'@user' => check_plain($user->name),
	);
	if (nquire_group_get_user_status_in_group($group_check['gid'], $uid) !== 'member') {
		return '<p>' . t('@user is not a member of @group.', $text_params) . '</p>';
	}

	drupal_set_title(t('Add roles to user @user within @group', $text_params));

	$all_roles = pi_inquiry_groupings_get_inquiry_role_names($inquiry);
	$user_roles = nquire_group_get_user_roles($group_check['group']->nid, $user->uid);
	$available_roles = array();
	foreach ($all_roles as $rid => $title) {
		if (!in_array($rid, $user_roles)) {
			$available_roles[$rid] = $title;
		}
	}

	$output = drupal_get_form('pi_inquiry_manager_participants_group_addrolestouser_form', $available_roles);
	return $output;
}

function pi_inquiry_manager_participants_group_addrolestouser_form($form_state, $available_roles) {
	$form = array();

	if (count($available_roles) > 0) {
		$form['roles'] = array(
				'#type' => 'checkboxes',
				'#title' => t('Select roles to add'),
				'#description' => t('The selected roles will be added.'),
				'#required' => TRUE,
				'#options' => $available_roles,
		);

		nquire_commons_create_form_save_cancel_buttons($form, 'pi_inquiry_manager_participants_group_addrolestouser_form_submit', $_REQUEST['destination'], t('Add roles'));
	} else {
		$form['message'] = array(
				'#value' => '<p>' . t('No roles available for this user.') . '</p>',
		);

		nquire_commons_create_form_cancel_button($form, $_REQUEST['destination']);
	}

	return $form;
}

function pi_inquiry_manager_participants_group_addrolestouser_form_submit($form, $form_state) {
	$ids = _pi_inquiry_manager_participants_form_submit_decode_path('uid', 4);

	$inquiry = node_load($ids['inquiry_nid']);
	$group_check = pi_inquiry_groupings_check_group($inquiry, $ids['group_nid']);
	if ($group_check && isset($form_state['values']['roles'])) {
		foreach ($form_state['values']['roles'] as $role_id => $add) {
			if ($add) {
				nquire_group_add_user_role($group_check['gid'], $ids['uid'], $role_id);
			}
		}
	}

	drupal_goto($_REQUEST['destination']);
}

/* confirmation pages */

function pi_inquiry_manager_participants_group_join($inquiry_nid, $group_nid = FALSE) {
	global $user;

	$output = '';

	$inquiry = node_load($inquiry_nid);
	$group_check = pi_inquiry_groupings_check_group($inquiry, $group_nid);
	if ($group_check) {

		$group_param = array(
				'@group' => ($group_nid ?
								t("group '@title'", array('@title' => $group_check['group']->title)) :
								t("inquiry '@title'", array('@title' => $inquiry->title))
				),
		);

		$status = nquire_group_get_user_status_in_group($group_check['gid'], $user->uid);

		if ($status === 'member') {
			$output .= '<p>' . t('You are already a member of @group', $group_param);
		} else if ($status === 'awaiting') {
			if ($group_check['group']->selective == OG_MODERATED) {
				$output .= '<p>' . t('You are already requested membership of @group', $group_param);
			} else {
				$output .= '<p>' . t('You have already been invited to @group', $group_param);
			}
		} else {
			if ($group_check['group']->selective == OG_MODERATED) {
				$label = t('Request membership');
				drupal_set_title(t('Do you want to request membership for @group?', $group_param));
			} else {
				$label = t('Join');
				drupal_set_title(t('Do you want to join @group?', $group_param));
			}

			$output .= drupal_get_form('pi_inquiry_manager_participants_group_confirm_form', 'pi_inquiry_manager_participants_group_join_form_submit', $label);
		}
	}

	return $output;
}

function pi_inquiry_manager_participants_group_join_form_submit() {
	$ids = _pi_inquiry_manager_participants_form_submit_decode_path();

	$inquiry = node_load($ids['inquiry_nid']);
	$group_check = pi_inquiry_groupings_check_group($inquiry, $ids['group_nid']);
	if ($group_check) {
		global $user;
		nquire_group_add_user_to_group($group_check['gid'], $user->uid, $group_check['group']->selective == OG_OPEN);
	}

	drupal_goto($_REQUEST['destination']);
}

function pi_inquiry_manager_participants_group_acceptinvitation($inquiry_nid, $group_nid = FALSE) {
	global $user;

	$output = '';

	$inquiry = node_load($inquiry_nid);
	$group_check = pi_inquiry_groupings_check_group($inquiry, $group_nid);
	if ($group_check) {
		$group_param = array(
				'@group' => ($group_nid ?
								t("group '@title'", array('@title' => $group_check['group']->title)) :
								t("inquiry '@title'", array('@title' => $inquiry->title))
				),
		);

		$status = nquire_group_get_user_status_in_group($group_check['gid'], $user->uid);

		if ($status === 'member') {
			$output .= '<p>' . t('You are already a member of @group', $group_param);
		} else if ($status === 'none') {
			$output .= '<p>' . t('Sorry, you have no invitation for group @group', $group_param);
		} else {
			if ($group_check['group']->selective == OG_INVITE_ONLY) {
				drupal_set_title(t('Do you want to accept the invitation to join @group?', $group_param));
				$output .= drupal_get_form('pi_inquiry_manager_participants_group_confirm_form', 'pi_inquiry_manager_participants_group_acceptinvitation_form_submit', t('Accept invitation'));
			} else {
				$output .= '<p>' . t('Invitations are not used to join @group', $group_param);
			}
		}
	}

	return $output;
}

function pi_inquiry_manager_participants_group_acceptinvitation_form_submit() {
	$ids = _pi_inquiry_manager_participants_form_submit_decode_path('uid', 4);

	$inquiry = node_load($ids['inquiry_nid']);
	$group_check = pi_inquiry_groupings_check_group($inquiry, $ids['group_nid']);
	if ($group_check) {
		global $user;
		nquire_group_add_user_to_group($group_check['gid'], $user->uid, TRUE);
	}

	drupal_goto($_REQUEST['destination']);
}

function pi_inquiry_manager_participants_group_leave($inquiry_nid, $group_nid = FALSE) {
	global $user;

	$output = '';

	$inquiry = node_load($inquiry_nid);
	$group_check = pi_inquiry_groupings_check_group($inquiry, $group_nid);
	if ($group_check) {

		$group_param = array(
				'@group' => ($group_nid ?
								t("group '@title'", array('@title' => $group_check['group']->title)) :
								t("inquiry '@title'", array('@title' => $inquiry->title))
				),
		);

		$status = nquire_group_get_user_status_in_group($group_check['gid'], $user->uid);

		if ($status === 'none') {
			$output .= '<p>' . t('You are not a member of @group', $group_param) . '</p>';
		} else {
			if ($status === 'member') {
				$label = t('Leave');
				$output .= drupal_set_title(t('Do you want to leave @group?', $group_param));
			} else if ($group_check['group']->selective == OG_MODERATED) {
				$label = t('Cancel request');
				$output .= drupal_set_title(t('Do you want to cancel your membership request from @group?', $group_param));
			} else {
				$label = t('Decline');
				$output .= drupal_set_title(t('Do you want to decline the invitation to join @group?', $group_param));
			}
			$output .= drupal_get_form('pi_inquiry_manager_participants_group_confirm_form', 'pi_inquiry_manager_participants_group_leave_form_submit', $label);
		}
	}

	return $output;
}

function pi_inquiry_manager_participants_group_leave_form_submit() {
	$ids = _pi_inquiry_manager_participants_form_submit_decode_path('uid', 4);

	$inquiry = node_load($ids['inquiry_nid']);
	$group_check = pi_inquiry_groupings_check_group($inquiry, $ids['group_nid']);
	if ($group_check) {
		global $user;
		nquire_group_delete_user_from_group($group_check['gid'], $user->uid);
	}

	drupal_goto($_REQUEST['destination']);
}

function pi_inquiry_manager_participants_group_accept_user($inquiry_nid, $uid, $group_nid = FALSE) {
	$output = '';

	$inquiry = node_load($inquiry_nid);
	$group_check = pi_inquiry_groupings_check_group($inquiry, $group_nid);
	if ($group_check) {
		$user = user_load($uid);

		$params = array(
				'@group' => ($group_nid ?
								t("group '@title'", array('@title' => $group_check['group']->title)) :
								t("inquiry '@title'", array('@title' => $inquiry->title))
				),
				'@user' => $user->name,
		);

		$status = nquire_group_get_user_status_in_group($group_check['gid'], $uid);

		if ($status === 'none') {
			$output .= '<p>' . t("User '@user' has not applied for membership in @group.", $params) . '</p>';
		} else if ($status === 'member') {
			$output .= '<p>' . t("User '@user' is already a member of @group.", $params) . '</p>';
		} else {
			if ($group_check['group']->selective == OG_MODERATED) {
				$output .= drupal_set_title(t("Do you want to accept user '@user' in @group?", $params));
				$output .= drupal_get_form('pi_inquiry_manager_participants_group_confirm_form', 'pi_inquiry_manager_participants_group_accept_user_form_submit', t('Accept'));
			} else {
				$output .= '<p>' . t("@group does not accept membership requests.", $params) . '</p>';
			}
		}
	}

	return $output;
}

function pi_inquiry_manager_participants_group_accept_user_form_submit() {
	$ids = _pi_inquiry_manager_participants_form_submit_decode_path('uid', 4);


	$inquiry = node_load($ids['inquiry_nid']);
	$group_check = pi_inquiry_groupings_check_group($inquiry, $ids['group_nid']);
	if ($group_check) {
		if (nquire_group_get_user_status_in_group($group_check['gid'], $ids['uid']) === 'awaiting') {
			nquire_group_add_user_to_group($group_check['gid'], $ids['uid'], TRUE);
		}
	}

	drupal_goto($_REQUEST['destination']);
}

function pi_inquiry_manager_participants_group_reject_user($inquiry_nid, $uid, $group_nid = FALSE) {
	$output = '';

	$inquiry = node_load($inquiry_nid);
	$group_check = pi_inquiry_groupings_check_group($inquiry, $group_nid);
	if ($group_check) {
		$user = user_load($uid);

		$params = array(
				'@group' => ($group_nid ?
								t("group '@title'", array('@title' => $group_check['group']->title)) :
								t("inquiry '@title'", array('@title' => $inquiry->title))
				),
				'@user' => $user->name,
		);

		$status = nquire_group_get_user_status_in_group($group_check['gid'], $uid);


		$label = FALSE;
		$cancel_label = FALSE;
		if ($status === 'member') {
			$label = t('Expel');
			drupal_set_title(t("Do you want to expel user '@user' from @group?", $params));
		} else if ($status === 'awaiting') {
			if ($group_check['group']->selective == OG_MODERATED) {
				$label = t('Reject');
				drupal_set_title(t("Do you want to reject user '@user' from @group?", $params));
			} else {
				$label = t('Cancel invitation');
				$cancel_label = t('Go back');
				drupal_set_title(t("Do you want to cancel the invitation of user '@user' for @group?", $params));
			}
		} else {
			$output .= '<p>' . t("User '@user' has not applied for membership in @group.", $params) . '</p>';
		}

		if ($label) {
			$output .= drupal_get_form('pi_inquiry_manager_participants_group_confirm_form', 'pi_inquiry_manager_participants_group_reject_user_form_submit', $label, $cancel_label);
		}
	}

	return $output;
}

function pi_inquiry_manager_participants_group_reject_user_form_submit() {
	$ids = _pi_inquiry_manager_participants_form_submit_decode_path('uid', 4);

	$inquiry = node_load($ids['inquiry_nid']);
	$group_check = pi_inquiry_groupings_check_group($inquiry, $ids['group_nid']);
	if ($group_check) {
		nquire_group_delete_user_from_group($group_check['gid'], $ids['uid']);
	}

	drupal_goto($_REQUEST['destination']);
}

function pi_inquiry_manager_participants_group_confirm_form($form_status, $save_callback, $save_label, $cancel_label = FALSE) {
	$form = array();
	nquire_commons_create_form_save_cancel_buttons($form, $save_callback, $_REQUEST['destination'], $save_label, $cancel_label);
	return $form;
}

function _pi_inquiry_manager_participants_group_create_search_users_form_item(&$form, $form_state, $available_users, $reject_list, $list_item_title, $list_item_description) {
	$username = nquire_commons_form_default_value($form_state, 'username', '');

	$form['username'] = array(
			'#type' => 'textfield',
			'#title' => $available_users ? t('Filter user names') : t('Search user names'),
			'#description' => $available_users ? t('You can use this field to search users within this group.') : t('You can use this field to search for users in the system.'),
			'#default_value' => $username,
			'#ahah' => array(
					'path' => ahah_helper_path(array('username_dependent_section')),
					'wrapper' => 'username_dependent_section',
					'method' => 'replace',
					'event' => 'keyup',
					'keypress' => TRUE,
			),
	);

	$message = '';

	if ($available_users) {
		$user_names = array();
		foreach ($available_users as $uid) {
			$user = user_load($uid);
			$user_names[$uid] = check_plain($user->name);
		}

		$n0 = count($available_users);
		if (strlen($username) < 1) {
			$message .= t('@n users available: ', array('@n' => $n0));
		} else {
			$user_names = array_filter($user_names, function($name) use ($username) {
								return strpos($name, $username) !== FALSE;
							});
			$message .= t("@n1 users out of $n0 match '@t': ", array('@n0' => $n0, '@n1' => count($user_names), '@t' => $username));
		}
	} else { //search
		$user_names = array();
		if (strlen($username) < 2) {
			$message .= t('Please enter at least two characters in the search field.');
		} else {
			$query = "SELECT uid, name FROM {users} WHERE name LIKE '%%%s%%'";
			$result = db_query($query, $username);
			while ($row = db_fetch_array($result)) {
				if (!$reject_list || !in_array($row['uid'], $reject_list)) {
					$user_names[$row['uid']] = check_plain($row['name']);
				}
			}

			$message .= count($user_names) > 0 ? t('@n users found:', array('@n' => count($user_names))) : t("No users found for '@t'", array('@t' => $username));
		}
	}


	nquire_commons_create_form_wrapped_section($form, 'username_dependent_section');


	$form['username_dependent_section']['wrapped_content']['message'] = array(
			'#value' => '<p><i>' . $message . '</i></p>',
	);

	if ($user_names && count($user_names) > 0) {
		$form['username_dependent_section']['wrapped_content']['userlist'] = array(
				'#type' => 'checkboxes',
				'#title' => $list_item_title,
				'#description' => $list_item_description,
				'#options' => $user_names,
				'#default_value' => isset($form_state['values']['userlist']) ? $form_state['values']['userlist'] : array(),
		);
	}
}

function pi_inquiry_manager_participants_group_invite_users($inquiry_nid, $group_nid = FALSE) {
	$output = drupal_get_form('pi_inquiry_manager_participants_group_invite_users_form', $inquiry_nid, $group_nid);
	return $output;
}

function pi_inquiry_manager_participants_group_invite_users_form($form_state, $inquiry_nid, $group_nid) {
	$form = array();

	ahah_helper_register($form, $form_state);
	nquire_commons_load_ahah_fix();

	$inquiry = node_load($inquiry_nid);
	$group_check = pi_inquiry_groupings_check_group($inquiry, $group_nid);
	if ($group_check) {

		if ($group_check['type'] === 'group') { // invite to group
			$group_name = t("group '@title'", array('@title' => $group_check['group']->title));
			$available_list = pi_inquiry_groupings_get_users_without_subgroup($inquiry);
		} else { // invite to inquiry
			$group_name = t("inquiry '@title'", array('@title' => $inquiry->title));
			$available_list = pi_inquiry_groupings_get_allowed_participants_for_inquiry($inquiry);
		}
		if ($group_check['normal_management']) {
			drupal_set_title(t('Invite users to @group', array('@group' => $group_name)));
		} else {
			drupal_set_title(t('Add users to @group', array('@group' => $group_name)));
		}

		if ($available_list) {
			$available_list = array_filter($available_list, function($uid) use ($group_check) {
								return nquire_group_get_user_status_in_group($group_check['gid'], $uid) === 'none';
							});
		}
	}

	$reject_list = $available_list ? FALSE : nquire_group_get_member_uids($group_check['gid']);

	_pi_inquiry_manager_participants_group_create_search_users_form_item($form, $form_state, $available_list, $reject_list, t('Select users to invite'), t("The selected users will be invited to '@group'.", array('@group' => $group_name)));

	nquire_commons_create_form_save_cancel_buttons($form, 'pi_inquiry_manager_participants_group_invite_users_form_submit', $_REQUEST['destination'], t('Invite'));
	return $form;
}

function pi_inquiry_manager_participants_group_invite_users_form_submit($form, $form_state) {
	$ids = _pi_inquiry_manager_participants_form_submit_decode_path();

	$inquiry = node_load($ids['inquiry_nid']);
	$group_check = pi_inquiry_groupings_check_group($inquiry, $ids['group_nid']);
	if ($group_check && isset($form_state['values']['userlist'])) {
		foreach ($form_state['values']['userlist'] as $uid => $invite) {
			if ($invite && ($group_check['type'] === 'inquiry' || pi_inquiry_groupings_get_user_subgroup($inquiry, $uid) === FALSE)) {
				nquire_group_add_user_to_group($group_check['gid'], $uid, !$group_check['normal_management']);
			}
		}
	}

	drupal_goto($_REQUEST['destination']);
}

function pi_inquiry_manager_participants_group_removeuserrole($inquiry_nid, $uid, $role_id, $group_nid = FALSE) {
	$output = '';

	$inquiry = node_load($inquiry_nid);
	$group_check = pi_inquiry_groupings_check_group($inquiry, $group_nid);
	if ($group_check) {
		$user = user_load($uid);

		$params = array(
				'@group' => ($group_nid ?
								t("group '@title'", array('@title' => $group_check['group']->title)) :
								t("inquiry '@title'", array('@title' => $inquiry->title))
				),
				'@user' => $user->name,
				'@role' => pi_inquiry_groupings_get_inquiry_role_name($inquiry, $role_id),
		);

		$status = nquire_group_get_user_status_in_group($group_check['gid'], $uid);

		if ($status === 'none') {
			$output .= '<p>' . t("@user is not a member of @group.", $params) . '</p>';
		} else if ($status === 'awaiting') {
			$output .= '<p>' . t("@user is not yet a member of @group.", $params) . '</p>';
		} else {
			if (!nquire_group_user_has_role($group_check['gid'], $uid, $role_id)) {
				$output .= '<p>' . t("@user has not role 'role' in @group.", $params) . '</p>';
			} else {
				drupal_set_title(t("Do you want to remove role '@role' for @user in @group?", $params));
				$output .= drupal_get_form('pi_inquiry_manager_participants_group_confirm_form', 'pi_inquiry_manager_participants_group_removeuserrole_form_submit', t('Remove'));
			}
		}
	}

	return $output;
}

function pi_inquiry_manager_participants_group_removeuserrole_form_submit() {
	$ids = _pi_inquiry_manager_participants_form_submit_decode_path('uid', 4, 'role_id', 5);

	$inquiry = node_load($ids['inquiry_nid']);
	$group_check = pi_inquiry_groupings_check_group($inquiry, $ids['group_nid']);
	if ($group_check) {
		nquire_group_remove_user_role($group_check['gid'], $ids['uid'], $ids['role_id']);
	}

	drupal_goto($_REQUEST['destination']);
}

/* groups */

function _pi_inquiry_manager_participants_user_names($uids, $myuid) {
	return array_map(function($uid) use ($myuid) {
						if ($uid === $myuid) {
							return '<b>' . t('Me') . '</b>';
						} else {
							$u = user_load($uid);
							return check_plain($u->name);
						}
					}, $uids);
}

function theme_pi_inquiry_manager_participants_groups($inquiry_node) {
	$output = '';

	$has_phases = pi_inquiry_groupings_has_collaboration_phases($inquiry_node->nid, 'group');
	$group_check = pi_inquiry_groupings_check_group($inquiry_node);

	if ($group_check && $has_phases) {
		global $user;

		$normal_group_management = $inquiry_node->group_creation === 'normal';
		$can_edit = pi_inquiry_manager_participants_subgroups_edit_policy_access($inquiry_node->nid);

		$output .= '<p>'
						. t('Some of the inquiries are carried out in groups.')
						. '<br/>';
		$output .= $normal_group_management ?
						t('Group management is open for inquiry members.') :
						($can_edit ? t('Groups are managed by me.') : t('Groups are managed by inquiry owner.'));

		if ($can_edit) {
			$output .= '<br/>' . _pi_inquiry_manager_partipants_group_operation_link(t('Edit'), $inquiry_node->nid, FALSE, 'groupmanagement');
		}
		$output .= '</p>';

		$my_subgroup = pi_inquiry_groupings_get_user_subgroup($inquiry_node, $user->uid);

		$subgroups = pi_inquiry_groupings_subgroups($inquiry_node);
		usort($subgroups, function($a, $b) use ($my_subgroup) {
							if ($a->nid === $my_subgroup)
								return -1;
							else if ($b->nid === $my_subgroup)
								return 1;
							else
								return strcmp($a->title, $b->title);
						});

		$rows = array_map(function($group) use ($user, $inquiry_node) {
							$title = _pi_inquiry_manager_partipants_group_operation_link(check_plain($group->title), $inquiry_node->nid, $group->nid, FALSE, FALSE);
							$title .= '<br/>' . theme('pi_inquiry_manager_participants_group_status_and_options', $inquiry_node, $user->uid, $group->nid, FALSE, FALSE);

							$members = nquire_group_get_member_uids($group->nid, TRUE);
							$members_names = _pi_inquiry_manager_participants_user_names($members, $user->uid);
							$members_text = implode(', ', $members_names);
							return array($title, $members_text);
						}, $subgroups);
		if (count($rows) > 0) {
			$header = array('Group', 'Members');
			$output .= nquire_commons_create_not_expanding_table_block(theme_table($header, $rows));
		}

		if (pi_inquiry_manager_participants_subgroups_create_access($inquiry_node->nid)) {
			if ($normal_group_management) {
				if (!$my_subgroup) {
					$output .= '<p>' . _pi_inquiry_manager_partipants_group_operation_link(t('Create my group'), $inquiry_node->nid, FALSE, 'createsubgroup') . '</p>';
				}
			} else {
				$output .= '<p>' . _pi_inquiry_manager_partipants_group_operation_link(t('Create new subgroup'), $inquiry_node->nid, FALSE, 'createsubgroup') . '</p>';
			}
		}

		$unasigned_users = pi_inquiry_groupings_get_users_without_subgroup($inquiry_node);
		if (count($unasigned_users) > 0) {
			$output .= '<p>' . t('Some users havent been assigned to groups:<br/>') . implode(', ', _pi_inquiry_manager_participants_user_names($unasigned_users, $user->uid)) . '</p>';
		}
	}

	return $output;
}

function pi_inquiry_manager_participants_subgroups_edit_policy($inquiry_nid) {
	$inquiry_node = node_load($inquiry_nid);
	return drupal_get_form('pi_inquiry_manager_participants_subgroups_edit_policy_form', $inquiry_node);
}

function pi_inquiry_manager_participants_subgroups_edit_policy_form(&$form_state, $inquiry_node) {
	$form = array();

	ahah_helper_register($form, $form_state);

	nquire_commons_load_ahah_fix();

	$policy = nquire_commons_form_node_default_value($inquiry_node, 'group_creation', $form_state, 'group_creation', 'controlled');

	$form['group_creation'] = array(
			'#type' => 'select',
			'#title' => t('Who can join this inquiry'),
			'#description' => t('Select the group of users that may become participants of this inquiry.'),
			'#options' => array('controlled' => t('Controlled by owner'), 'normal' => t('Free')),
			'#default_value' => $policy,
			'#required' => TRUE,
	);

	nquire_commons_create_form_save_cancel_buttons($form, 'pi_inquiry_manager_participants_subgroups_edit_policy_form_submit', $_REQUEST['destination']);

	return $form;
}

function pi_inquiry_manager_participants_subgroups_edit_policy_form_submit($form, $form_state) {
	$inquiry_nid = arg(1);
	if (pi_inquiry_manager_participants_subgroups_edit_policy_access($inquiry_nid)) {
		$inquiry = node_load($inquiry_nid);
		$new_policy = $form_state['values']['group_creation'];

		$inquiry->group_creation = $new_policy;

		$inquiry->revision = TRUE;
		node_save($inquiry);
	}

	nquire_commons_form_close($form, $form_state);
}

function pi_inquiry_manager_participants_subgroups_create($inquiry_nid) {
	global $user;
	$output = '';
	$subgroup = pi_inquiry_groupings_get_user_subgroup($inquiry_nid, $user->uid);

	if ($subgroup) {
		$output .= '<p>' . t('You already belong to a group in this inquiry!') . '</p>';
	} else {
		$output .= drupal_get_form('pi_inquiry_manager_participants_subgroup_form', node_load($inquiry_nid));
	}

	return $output;
}

function pi_inquiry_manager_participants_subgroup_edit($inquiry_nid, $group_nid) {
	$output = drupal_get_form('pi_inquiry_manager_participants_subgroup_form', node_load($inquiry_nid), $group_nid);
	return $output;
}

function pi_inquiry_manager_participants_subgroup_form(&$form_state, $inquiry_node, $group_nid = FALSE) {
	$form = array();

	if ($group_nid) {
		$group = node_load($group_nid);
		drupal_set_title(t("Edit group '@group'"), array('@group' => $group->title));
	} else {
		drupal_set_title(t('Create new group'));
	}

	$form['group_title'] = array(
			'#type' => 'textfield',
			'#title' => t('Group name'),
			'#description' => t('Enter the group name.'),
			'#default_value' => nquire_commons_form_node_default_value($group, 'title', $form_state, 'group_title', ''),
			'#required' => TRUE,
	);

	_pi_inquiry_manager_participants_create_group_join_mode_form_item($form, $form_state, $group_nid, 'join_mode', t('Join mode'), t('Select how inquiry members can join this node.'), $inquiry_node->group_creation === 'controlled');

	nquire_commons_create_form_save_cancel_buttons($form, 'pi_inquiry_manager_participants_subgroup_form_submit', $_REQUEST['destination']);

	return $form;
}

function pi_inquiry_manager_participants_subgroup_form_validate($form, $form_state) {
	$ids = _pi_inquiry_manager_participants_form_submit_decode_path();
	$inquiry = node_load($ids['inquiry_nid']);
	$subgroup = pi_inquiry_groupings_subgroup_with_name($inquiry, $form_state['values']['group_title']);
	if ($subgroup && $subgroup != $ids['group_nid']) {
		form_set_error('group_title', t('A group with the same already exists in this inquiry.'));
	}
}

function pi_inquiry_manager_participants_subgroup_form_submit($form, $form_state) {
	$ids = _pi_inquiry_manager_participants_form_submit_decode_path();

	$save_group_nid = FALSE;

	if ($ids['group_nid']) {
		if (pi_inquiry_manager_participants_subgroups_edit_access($ids['inquiry_nid'], $ids['group_nid'])) {
			nquire_group_set_title($ids['group_nid'], $form_state['values']['group_title']);
			$save_group_nid = $ids['group_nid'];
		}
	} else {
		if (pi_inquiry_manager_participants_subgroups_create_access($ids['inquiry_nid'])) {
			global $user;
			$inquiry = node_load($ids['inquiry_nid']);
			if ($inquiry && $inquiry->actor_gid) {
				if ($inquiry->group_creation === 'controlled' || !pi_inquiry_groupings_get_user_subgroup($inquiry, $user->uid)) {
					$save_group_nid = pi_inquiry_groupings_create_inquiry_subgroup($ids['inquiry_nid'], $user->uid, $form_state['values']['group_title'], $inquiry->group_creation === 'normal');
				}
			}
		}
	}

	if ($save_group_nid && isset($form_state['values']['join_mode'])) {
		_pi_inquiry_manager_participants_process_join_mode_form_value($save_group_nid, $form_state['values']['join_mode']);
	}

	nquire_commons_form_close($form, $form_state);
}

function pi_inquiry_manager_participants_subgroup($inquiry_nid, $group_nid) {
	$inquiry = node_load($inquiry_nid);
	$output = theme('pi_inquiry_manager_participants_group_access_policy', $inquiry, $group_nid);
	$output .= theme('pi_inquiry_manager_participants_list', $inquiry, $group_nid);
	$output .= theme('pi_inquiry_manager_participants_roles', $inquiry, $group_nid);

	$output .= '<p>' . _pi_inquiry_manager_partipants_group_operation_link(t('Go back'), $inquiry_nid, FALSE, FALSE, FALSE) . '</p>';
	return $output;
}

function theme_pi_inquiry_manager_participants_group_access_policy($inquiry_node, $group_nid) {

	$output = '';

	$group = node_load($group_nid);

	switch ($group->selective) {
		case OG_OPEN:
			$output .= t('Access to this group is open for inquiry members.');
			break;
		case OG_MODERATED:
			$output .= t('Access needs to be approved by the group owner.');
			break;
		case OG_INVITE_ONLY:
			$output .= t('Access is through invitation only.');
			break;
	}


	if (pi_inquiry_manager_participants_subgroups_edit_access($inquiry_node->nid, $group_nid)) {
		$output .= '<br/>' . _pi_inquiry_manager_partipants_group_operation_link(t('Edit group'), $inquiry_node->nid, $group_nid, "edit", TRUE);
	}
	return $output;
}

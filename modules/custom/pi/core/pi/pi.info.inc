<?php

class PiInfo {

  private $path;
  private $inquiries;
  private $content;
  private $access;

  function __construct() {
    $this->path = new PiInquiryPath();
    $this->inquiries = array();
    $this->content = NULL;
    $this->access = array();
  }

  /**
   * 
   * @return \PiInquiryPath
   */
  function getCurrentPath() {
    return $this->path;
  }

  /**
   * 
   * @return \PiInquiryInfo
   */
  function getCurrentInquiryInfo() {
    if ($this->path->inquiry_nid) {
      return $this->getInquiryInfo($this->path->inquiry_nid);
    } else {
      return FALSE;
    }
  }

  /**
   * 
   * @param type $inquiry_id
   * @return \PiInquiryInfo
   */
  function getInquiryInfo($inquiry_id) {
    if (!isset($this->inquiries[$inquiry_id])) {
      $this->inquiries[$inquiry_id] = new PiInquiryInfo($inquiry_id);
    }

    return $this->inquiries[$inquiry_id];
  }

  /**
   * 
   * @param type $inquiry_id
   * @return \PiInquiryInfo
   */
  function getInquiryInfoForContent($content_nid) {
    $activity_nid = pi_activity_get_activity_nid_for_content_nid($content_nid);
    return $this->getInquiryInfoForActivity($activity_nid);
  }

  /**
   * 
   * @param type $inquiry_id
   * @return \PiInquiryInfo
   */
  function getInquiryInfoForActivity($activity_nid) {
    $activity = node_load($activity_nid);
    $phase = node_load($activity->phase_nid);

    return $this->getInquiryInfo($phase->inquiry_nid);
  }

  /**
   * 
   * @param type $inquiry_id
   * @return \PiInquiryContent
   */
  function getContentManager() {
    if (!$this->content) {
      $this->content = new PiInquiryContent();
    }

    return $this->content;
  }

  /**
   * 
   * @param type $inquiry_nid
   * @param type $uid
   * @return \PiInquiryAccess
   */
  function getAccessManager($inquiry_nid, $uid) {
    $key = "{$uid}:{$inquiry_nid}";
    if (!isset($this->access[$key])) {
      $this->access[$key] = new PiInquiryAccess($this->getInquiryInfo($inquiry_nid), $uid);
    }

    return $this->access[$key];
  }

}

class PiInquiryPath {

  public $inquiry_nid;
  public $phase_nid;
  public $activity_nid;

  function __construct() {
    $path = get_inquiry_details_from_current_path();
    $this->activity_nid = $path->activity_id;
    $this->phase_nid = $path->phase_id;
    $this->inquiry_nid = $path->inquiry_id;
  }

}

class PiInquiryInfo {

  private $inquiry;
  private $phases;
  private $phase_activities;
  private $activity_phase;
  private $activities;
  private $use_groups;
  private $collaboration_level;
  private $stage;
  private $stage_count;
  private $roles;
  private $all_stages_structure;

  function __construct($inquiry_id) {
    $this->inquiry = node_load($inquiry_id);
    $this->roles = nquire_commons_decode_data($this->inquiry->roles);

    $this->phases = array();
    $this->activity_phase = array();
    $this->phase_activities = array();
    $this->activities = array();

    $this->collaboration_level = 'individual';

    $query = "SELECT stage_id FROM {pi_stage_current} WHERE inquiry_id=%d";
    $row = db_fetch_array(db_query($query, $this->inquiry->nid));
    $this->stage = FALSE;
    if ($row) {
      $this->stage = node_load($row['stage_id']);
    } else {
      $query = "SELECT {node}.nid FROM {node} LEFT JOIN {pi_stage} ON {node}.vid={pi_stage}.vid WHERE {pi_stage}.inquiry_nid=%d ORDER BY {pi_stage}.weight ASC LIMIT 1";
      $row = db_fetch_array(db_query($query, $this->inquiry->nid));
      if ($row) {
        $this->stage = node_load($row['nid']);
      }
    }

    $stage_count_row = db_fetch_array(db_query("SELECT COUNT({node}.nid) as c FROM {node} LEFT JOIN {pi_stage} ON {node}.vid={pi_stage}.vid WHERE {pi_stage}.inquiry_nid=%d", $this->inquiry->nid));
    $this->stage_count = $stage_count_row ? $stage_count_row['c'] : 0;


    $phase_nids = pi_phase_load_nids_for_inquiry($this->inquiry->nid);
    foreach ($phase_nids as $phase_nid) {
      $phase = node_load($phase_nid);
      $this->phases[$phase_nid] = $phase;

      if ($phase->collaboration === 'group') {
        $this->use_groups = TRUE;
        if ($this->collaboration_level === 'individual') {
          $this->collaboration_level = 'group';
        }
      } else if ($phase->collaboration === 'all') {
        $this->collaboration_level = 'all';
      }

      $this->phase_activities[$phase_nid] = array();
      $activity_nids = pi_get_activities_for_phase($phase_nid);
      foreach ($activity_nids as $activity_nid) {
        $activity = node_load($activity_nid);
        $this->activities[$activity_nid] = $activity;
        $this->phase_activities[$phase_nid][$activity_nid] = $activity;
        $this->activity_phase[$activity_nid] = $phase_nid;
      }
    }
  }

  public function getInquiryNid() {
    return $this->inquiry->nid;
  }

  public function getInquiryNode() {
    return $this->inquiry;
  }

  public function userIsInquiryOwner($uid) {
    return $this->inquiry->uid === $uid;
  }
  
  public function userIsInquiryAdmin($uid) {
    return $this->userIsInquiryOwner($uid) || nquire_group_user_is_admin($this->inquiry->actor_gid, $uid);
  }

  public function getInquiryActorGid() {
    return $this->inquiry->actor_gid;
  }

  public function useSubgroups() {
    return $this->use_groups;
  }

  public function getRoles() {
    return $this->roles;
  }

  public function getSubgroup($uid) {
    return pi_inquiry_groupings_get_user_subgroup($this->inquiry, $uid);
  }

  public function hasSeveralStages() {
    return $this->stage_count > 1;
  }

  public function getCurrentStage() {
    return $this->stage;
  }

  public function getActivity($activity_nid) {
    return $this->activities[$activity_nid];
  }

  public function getActivities() {
    return $this->activities;
  }

  public function getActivityByType($type) {
    foreach ($this->activities as $activity) {
      if ($activity->activity_type === $type) {
        return $activity;
      }
    }
    return FALSE;
  }

  public function getActivityNidsByType($type) {
    $nids = array();
    foreach ($this->activities as $activity_nid => $activity) {
      if ($activity->activity_type === $type) {
        $nids[] = $activity_nid;
      }
    }
    return $nids;
  }

  public function getPhase($phase_nid) {
    return isset($this->phases[$phase_nid]) ? $this->phases[$phase_nid] : FALSE;
  }

  public function getPhaseIndex($phase_nid) {
    return array_search($phase_nid, array_keys($this->phases));
  }

  public function getPhaseKey($phase_nid) {
    $phase_count = count($this->phases);
    $keys = $this->getPhaseKeys($phase_count);
    $index = $this->getPhaseIndex($phase_nid);
    return $keys[$index % strlen($keys)];
  }

  public function getPhases() {
    return $this->phases;
  }

  public function getActivitiesForPhase($phase_nid) {
    return $this->phase_activities[$phase_nid];
  }

  public function getPhaseActor($phase_nid, $uid) {
    $phase = $this->getPhase($phase_nid);

    $group = $this->getSubgroup($uid);
    $inquiry_actor = $this->inquiry->actor_gid;

    switch ($phase->collaboration) {
      case 'individual':
        $actor = $uid;
        break;
      case 'group':
        $actor = $group;
        break;
      default:
        $actor = $inquiry_actor;
        break;
    }
    return array(
        'collaboration' => $phase->collaboration,
        'actor' => $actor,
    );
  }

  public function getPhaseNidForActivity($activity_nid) {
    if (isset($this->activity_phase[$activity_nid])) {
      return $this->activity_phase[$activity_nid];
    } else {
      $activity_node = node_load($activity_nid);
      return $activity_node->phase_nid;
    }
  }

  public function getPhaseForActivity($activity_nid) {
    return $this->phases[$this->getPhaseNidForActivity($activity_nid)];
  }

  public function getActivityActor($activity_nid, $uid) {
    return $this->getPhaseActor($this->getPhaseNidForActivity($activity_nid), $uid);
  }

  public function getActivityCollaboration($activity_nid) {
    return $this->getPhaseForActivity($activity_nid)->collaboration;
  }

  public function getStageNodes() {
    $nodes = array();
    $query = "SELECT {node}.nid FROM {node} LEFT JOIN {pi_stage} ON {node}.vid={pi_stage}.vid WHERE inquiry_nid=%d";
    $result = db_query($query, $this->inquiry->nid);
    while ($row = db_fetch_array($result)) {
      $nodes[$row['nid']] = node_load($row['nid']);
    }
    return $nodes;
  }

  public function getStageStructure() {
    if (!isset($this->all_stages_structure)) {
      $this->all_stages_structure = array('structure' => array(), 'values' => array());

      foreach ($this->getStageNodes() as $stage) {
        foreach ($this->phases as $pnid => $phase_node) {
          if (!isset($stage->phases[$pnid])) {
            $stage->phases[$pnid] = 'edit';
          }
        }

        $this->all_stages_structure['values'][] = array(
            'id' => $stage->nid,
            'title' => check_plain($stage->title),
            'values' => $stage->phases,
        );
      }

      foreach ($this->phases as $pnid => $phase_node) {
        $phase_policy = array(
            'id' => $pnid,
            'title' => check_plain($phase_node->title),
            'activities' => array(),
        );

        foreach ($this->getActivitiesForPhase($pnid) as $anid => $activity_node) {
          $activity_policy = array(
              'id' => $anid,
              'title' => check_plain($activity_node->title),
          );

          $phase_policy['activities'][] = $activity_policy;
        }
        $this->all_stages_structure['structure'][] = $phase_policy;
      }
    }

    return $this->all_stages_structure;
  }

  public function getUserSharePolicy($uid) {
    $share_policy = array('structure' => array(), 'members' => array(), 'supervisor' => array());
    $query = "SELECT share, mode FROM pi_activity_share WHERE activity_id=%d AND actor=%d";

    foreach ($this->phases as $pnid => $phase_node) {
      $phase_policy = array(
          'id' => $pnid,
          'title' => check_plain($phase_node->title) . ' <small>(' . $phase_node->collaboration . ')</small>',
          'activities' => array(),
      );

      $valid = TRUE;
      switch ($phase_node->collaboration) {
        case 'individual':
          $default = $this->use_groups ? 'individual_group' : 'individual_no';
          $actor = $uid;
          $not_editable = FALSE;
          break;
        case 'group':
          $actor = $this->getSubgroup($uid);
          $group = node_load($actor);
          if ($group) {
            $default = 'group_no';
            $not_editable = $group->uid != $uid;
          } else {
            $valid = FALSE;
          }
          break;
        default:
          $default = 'all_no';
          $actor = $this->inquiry->actor_gid;
          $not_editable = $this->inquiry->uid != $uid;
          break;
      }

      foreach ($this->getActivitiesForPhase($pnid) as $anid => $activity_node) {
        if ($valid) {
          $result = db_query($query, $anid, $actor);
          while ($row = db_fetch_array($result)) {
            $share_policy[$row['mode']][$anid] = $row['share'];
          }

          if (!isset($share_policy['members'][$anid])) {
            $share_policy['members'][$anid] = $default;
          }
          if (!isset($share_policy['supervisor'][$anid])) {
            $share_policy['supervisor'][$anid] = 'supervisor_no';
          }
        }

        $activity_policy = array(
            'id' => $anid,
            'title' => check_plain($activity_node->title),
            'attributes' => array(
                'not_editable' => $not_editable || !$valid,
                'collaboration' => $phase_node->collaboration,
                'use_groups' => $this->use_groups ? '1' : '0',
            ),
        );

        $phase_policy['activities'][] = $activity_policy;
      }

      $share_policy['structure'][] = $phase_policy;
    }

    return $share_policy;
  }

  private function getPhaseKeys($n) {
    switch ($n) {
      case 1:
        return 'a';
      case 2:
        return 'ag';
      case 3:
        return 'acg';
      case 4:
        return 'acfg';
      case 5:
        return 'acdfg';
      case 6:
        return 'acdefg';
      case 7:
        return 'abcdefg';
      default:
        return 'abcdefgh';
    }
  }

}

class PiInquiryContent {

  public function __construct() {
    
  }

  public function getContentForActor($activity_nid, $actor) {
    $activity = pi_info()->getInquiryInfoForActivity($activity_nid)->getActivity($activity_nid);

    if ($activity->referenced_node) {
      return $activity->referenced_node;
    }

    $query = "SELECT nid FROM {pi_activity_node} WHERE activity_id=%d AND actor=%d";

    $row = db_fetch_array(db_query($query, $activity_nid, $actor));
    if ($row) {
      $nid = $row['nid'];
      return $nid;
    }

    return FALSE;
  }

  public function getContent($activity_nid, $uid, $try_to_create) {
    $inquiry_info = pi_info()->getInquiryInfoForActivity($activity_nid);
    $activity = $inquiry_info->getActivity($activity_nid);

    if ($activity->referenced_node) {
      return $activity->referenced_node;
    }

    $actor_data = $inquiry_info->getActivityActor($activity_nid, $uid);
    $query = "SELECT nid FROM {pi_activity_node} WHERE activity_id=%d AND collaboration='%s' AND actor=%d";


    $row = db_fetch_array(db_query($query, $activity_nid, $actor_data['collaboration'], $actor_data['actor']));
    if ($row) {
      $nid = $row['nid'];
      return $nid;
    } else if ($try_to_create) {
      $new_node = $this->createContentOnVisit($activity_nid, $uid);
      if ($new_node) {
        return $new_node->nid;
      }
    }

    return FALSE;
  }

  private function createContentOnVisit($activity_nid, $uid) {
    $activity = pi_info()->getInquiryInfoForActivity($activity_nid)->getActivity($activity_nid);

    $types = array(
        'pi_methodology',
        'pi_data_spreadsheet',
        'pi_sort_data',
        'pi_sort_key_answers',
        'pi_sort_key_questions',
        'pi_sort_result_presentations',
    );
    if (in_array($activity->activity_type, $types)) {
      $node = new stdClass();
      $node->uid = $uid;
      $node->type = $activity->activity_type;
      $node->body = '';
      node_save($node);

      $this->linkContentWithActivity($node, $activity_nid, $uid);

      return $node;
    }
  }

  public function linkContentWithActivity($content_node, $activity_nid, $uid) {
    $inquiry_info = pi_info()->getInquiryInfoForActivity($activity_nid);
    $actor_data = $inquiry_info->getActivityActor($activity_nid, $uid);

    $activity_node = array(
        'activity_id' => $activity_nid,
        'nid' => $content_node->nid,
        'collaboration' => $actor_data['collaboration'],
        'actor' => $actor_data['actor'],
    );

    drupal_write_record('pi_activity_node', $activity_node);

    $this->setDefaultSharePolicy($activity_nid, $uid);
  }

  public function getChildrenContentForContentNids($content_nids) {
    $nids = array();
    foreach ($content_nids as $nid) {
      $nids = array_merge($nids, $this->getChildrenContentForContentNid($nid));
    }
    return $nids;
  }

  public function getChildrenContentForContentNid($content_nid) {
    return pi_node_hierarchy_get_children($content_nid);
  }

  public function getContentFromForUsingContent($activity_nid, $using_content_nid) {
    $author = $this->getContentAuthor($using_content_nid);
    return $this->aggregateContentForActivityAndActorData($activity_nid, $author);
  }

  public function getContentFromForUsingActivityActor($activity_nid, $using_activity_id, $using_uid) {
    $inquiry_info = pi_info()->getInquiryInfoForActivity($activity_nid);
    $using_actor_data = $inquiry_info->getActivityActor($using_activity_id, $using_uid);
    return $this->aggregateContentForActivityAndActorData($activity_nid, $using_actor_data);
  }

  public function getContributionContent($contribution_node, $activity_nid) {
    $actor_data = array('collaboration' => $contribution_node->collaboration, 'actor' => $contribution_node->actor);
    return $this->aggregateContentForActivityAndActorData($activity_nid, $actor_data);
  }

  public function setDefaultSharePolicy($activity_nid, $viewer_uid) {
    $inquiry_info = pi_info()->getInquiryInfoForActivity($activity_nid);
    $activity_collaboration = $inquiry_info->getActivityCollaboration($activity_nid);

    if ($activity_collaboration === 'individual') {
      $actor = $viewer_uid;
    } else if ($activity_collaboration === 'group') {
      $actor = $inquiry_info->getSubgroup($viewer_uid);
    }

    if ($actor) {
      $check = "SELECT share FROM {pi_activity_share} WHERE mode='%s' AND activity_id=%d AND actor=%d";
      $insert = "INSERT INTO pi_activity_share (activity_id, actor, mode, share) VALUES (%d, %d, '%s', '%s')";
      $defaults = array(
          'members' => $activity_collaboration === 'individual' ? 'individual_group' : 'group_no',
          'supervisor' => 'supervisor_no',
      );
      foreach ($defaults as $mode => $default) {
        $check_result = db_query($check, $mode, $activity_nid, $actor);
        if (!$check_result || $check_result->num_rows == 0) {
          db_query($insert, $activity_nid, $actor, $mode, $default);
        }
      }
    }
  }

  public function getSharedContent($activity_nid, $viewer_uid, $include_any_for_supervisor = FALSE) {
    $inquiry_info = pi_info()->getInquiryInfoForActivity($activity_nid);
    $activity_collaboration = $inquiry_info->getActivityCollaboration($activity_nid);

    $sharing_actors = array();

    $query = "SELECT actor FROM {pi_activity_share} WHERE mode='%s' AND activity_id=%d AND share='%s'";

    $get_actors = function(&$actors, $mode, $share, $restrict_actors = FALSE) use ($query, $activity_nid) {
              $result = db_query($query, $mode, $activity_nid, $share);
              while ($row = db_fetch_array($result)) {
                if (!in_array($row['actor'], $actors) && ($restrict_actors === FALSE || in_array($row['actor'], $restrict_actors))) {
                  $actors[] = $row['actor'];
                }
              }
            };

    if ($activity_collaboration === 'individual') {
      $group = $inquiry_info->getSubgroup($viewer_uid);
      $group_members = nquire_group_get_member_uids($group, TRUE);
      $get_actors($sharing_actors, 'members', 'individual_group', $group_members);
      $get_actors($sharing_actors, 'members', 'individual_all');
    } else if ($activity_collaboration === 'group') {
      $get_actors($sharing_actors, 'members', 'group_all');
    }

    if ($include_any_for_supervisor) {
      $get_actors($sharing_actors, 'supervisor', 'supervisor_yes');
    }
    
    $nids = array();

    $query = "SELECT DISTINCT filtered_pi_activity_node.nid FROM "
            . "(SELECT * FROM pi_activity_node WHERE actor=%d) AS filtered_pi_activity_node INNER JOIN "
            . "(SELECT node.nid as activity_node_nid FROM node LEFT JOIN pi_activity ON node.vid=pi_activity.vid WHERE pi_activity.nid=%d) AS activity_node_table "
            . "ON filtered_pi_activity_node.activity_id=activity_node_table.activity_node_nid ";
    foreach ($sharing_actors as $actor) {
      $result = db_query($query, $actor, $activity_nid);
      while ($row = db_fetch_array($result)) {
        $nids[] = $row['nid'];
      }
    }

    return $nids;
  }

  public function getContentAuthor($content_nid) {
    $query = "SELECT collaboration, actor FROM {pi_activity_node} WHERE `nid` = %d";
    return db_fetch_array(db_query($query, $content_nid));
  }

  private function aggregateContentForActivityAndActorData($activity_nid, $viewer_actor_data) {
    $inquiry_info = pi_info()->getInquiryInfoForActivity($activity_nid);
    $activity_collaboration = $inquiry_info->getActivityCollaboration($activity_nid);
    $viewer_collaboration = $viewer_actor_data['collaboration'];

    if ($viewer_collaboration === $activity_collaboration) {
      $mode = 'single_actor';
      $single_actor = $viewer_actor_data['actor'];
    } else if ($activity_collaboration === 'all') {
      $mode = 'single_actor';
      $single_actor = $inquiry_info->getInquiryActorGid();
    } else if ($viewer_collaboration === 'individual' && $activity_collaboration === 'group') {
      $mode = 'single_actor';
      $single_actor = $inquiry_info->getSubgroup($viewer_actor_data['actor']);
    } else if ($viewer_collaboration === 'group' && $activity_collaboration === 'individual') {
      $mode = 'shared_within_group';
      $group = $inquiry_info->getSubgroup($viewer_actor_data['actor']);
      $group_members = nquire_group_get_member_uids($group, TRUE);
    } else if ($viewer_collaboration === 'all' && $activity_collaboration === 'individual') {
      $mode = 'shared_with_inquiry';
      $all_share_requisite = 'individual_all';
    } else if ($viewer_collaboration === 'all' && $activity_collaboration === 'group') {
      $mode = 'shared_with_inquiry';
      $all_share_requisite = 'group_all';
    }


    $sharing_actors = array();
    switch ($mode) {
      case 'single_actor';
        $sharing_actors[] = $single_actor;
        break;
      case 'shared_within_group';
        $share_query = "SELECT share FROM pi_activity_share WHERE activity_id=%d AND actor=%d";
        foreach ($group_members as $uid) {
          $row = db_fetch_array(db_query($share_query, $activity_nid, $uid));
          $share = !$row || $row['share'] !== 'individual_no';
          if ($share) {
            $sharing_actors[] = $uid;
          }
        }
        break;
      case 'shared_with_inquiry':
        $share_query = "SELECT actor FROM {pi_activity_share} WHERE activity_id=%d AND mode='members' AND share='%s'";
        $result = db_query($share_query, $activity_nid, $all_share_requisite);
        while ($row = db_fetch_array($result)) {
          $sharing_actors[] = $row['actor'];
        }
        break;
    }

    $nids = array();

    $query = "SELECT DISTINCT filtered_pi_activity_node.nid FROM "
            . "(SELECT * FROM pi_activity_node WHERE actor=%d) AS filtered_pi_activity_node INNER JOIN "
            . "(SELECT node.nid as activity_node_nid FROM node LEFT JOIN pi_activity ON node.vid=pi_activity.vid WHERE pi_activity.nid=%d) AS activity_node_table "
            . "ON filtered_pi_activity_node.activity_id=activity_node_table.activity_node_nid ";
    foreach ($sharing_actors as $actor) {
      $result = db_query($query, $actor, $activity_nid);
      while ($row = db_fetch_array($result)) {
        $nids[] = $row['nid'];
      }
    }

    return $nids;
  }

}

class PiInquiryAccess {

  /**
   *
   * @var \PiInquiryInfo
   */
  private $inquiry_info;
  private $uid;
  private $use_roles;
  private $inquiry_roles;
  private $group_roles;
  private $subgroup;
  private $is_group_owner;
  private $is_owner;
  private $inquiry_access;
  private $item_access;

  public function __construct($inquiry_info, $uid) {
    $this->inquiry_info = $inquiry_info;
    $this->uid = $uid;
    $this->is_owner = $this->inquiry_info->userIsInquiryAdmin($this->uid);
    $this->inquiry_access = $this->is_owner || nquire_group_get_user_status_in_group($this->inquiry_info->getInquiryActorGid(), $this->uid) === 'member';
    $this->item_access = array();
    $this->use_roles = count($this->inquiry_info->getRoles()) > 0;
    $this->subgroup = $this->inquiry_info->getSubgroup($this->uid);
  }

  private function getGroupRoles() {
    if (!isset($this->group_roles)) {
      $this->group_roles = $this->subgroup ? nquire_group_get_user_roles($this->subgroup, $this->uid) : FALSE;
    }
    return $this->group_roles;
  }

  private function getInquiryRoles() {
    if (!isset($this->inquiry_roles)) {
      $this->inquiry_roles = nquire_group_get_user_roles($this->inquiry_info->getInquiryActorGid(), $this->uid);
    }
    return $this->inquiry_roles;
  }

  public function hasInquiryAccess() {
    return $this->inquiry_access;
  }

  public function isInquiryOwner() {
    return $this->is_owner;
  }

  public function isGroupOwner() {
    if (!isset($this->is_group_owner)) {
      $group_nid = $this->getSubgroup();
      $group_node = node_load($group_nid);
      $this->is_group_owner = $group_node === $this->uid;
    }
    return $this->is_group_owner;
  }

  public function getAccessReason($item) {
    $access = $this->getItemAccess($item);
    return $access['reason'];
  }

  public function getAccessToItem($item) {
    $access = $this->getItemAccess($item);
    return $access['access'];
  }

  private function getItemAccess($item) {
    return $this->calculateAccess(($item->type === 'pi_activity' && $item->parent_activity) ? $this->inquiry_info->getActivity($item->parent_activity) : $item);
  }

  private function calculateAccess($item) {
    if (!isset($this->item_access[$item->nid])) {
      $access = array();
      if ($this->inquiry_access) {
        if ($item->type === 'pi_phase') {
          $stage = $this->inquiry_info->getCurrentStage();
          if ($stage) {
            $access['access'] = isset($stage->phases[$item->nid]) ? $stage->phases[$item->nid] : 'edit';
            $access['reason'] = 'stage';
          } else {
            $access['access'] = 'edit';
            $access['reason'] = 'none';
          }
        } else if ($item->type === 'pi_activity') {
          $phase_node = $this->inquiry_info->getPhaseForActivity($item->nid);
          $phase_access = $access = $this->calculateAccess($phase_node);

          switch ($phase_node->collaboration) {
            case 'individual';
              $access = $phase_access;
              break;
            case 'group':
              if (!$this->subgroup) {
                $access['access'] = 'hidden';
                $access['reason'] = 'no_group';
              } else {
                $role_access = $this->use_roles ? $this->maxAccessForRoles($this->getGroupRoles(), $item->nid) : FALSE;
                if ($role_access && $this->compareAccess($role_access, $phase_access['access']) < 0) {
                  $access['access'] = $role_access;
                  $access['reason'] = 'group_roles';
                } else {
                  $access = $phase_access;
                }
              }
              break;
            default:
              $role_access = $this->use_roles ? $this->maxAccessForRoles($this->getInquiryRoles(), $item->nid) : FALSE;
              if ($role_access && $this->compareAccess($role_access, $phase_access['access']) < 0) {
                $access['access'] = $role_access;
                $access['reason'] = 'inquiry_roles';
              } else {
                $access = $phase_access;
              }

              break;
          }

          if ($access['access'] === 'edit' && !pi_activity_is_editable($item)) {
            $access['access'] = 'view';
            $access['reason'] = 'none';
          }
        }
      } else {
        $access['access'] = 'hidden';
        $access['reason'] = 'no_inquiry_access';
      }

      $this->item_access[$item->nid] = $access;
    }

    return $this->item_access[$item->nid];
  }

  public function getSubgroup() {
    return $this->subgroup;
  }

  private function maxAccess($a, $b) {
    if ($a === 'edit' || $b === 'edit') {
      return 'edit';
    } else if ($a === 'view' || $b === 'view') {
      return 'view';
    } else {
      return 'hidden';
    }
  }

  private function minAccess($a, $b) {
    if ($a === 'edit' || $b === 'edit') {
      return 'edit';
    } else if ($a === 'view' || $b === 'view') {
      return 'view';
    } else {
      return 'hidden';
    }
  }

  private function compareAccess($a, $b) {
    if ($a === $b) {
      return 0;
    } else if ($a === 'edit' || $b === 'hidden') {
      return 1;
    } else {
      return -1;
    }
  }

  private function maxAccessForRoles($user_roles, $activity_nid) {
    if (count($this->inquiry['roles']) === 0) {
      return 'edit';
    } else {
      $access = 'hidden';
      foreach ($this->inquiry_info->getRoles() as $role) {
        if (in_array($role['id'], $user_roles) && isset($role['activities'][$activity_nid])) {
          $access = $this->maxAccess($access, $role['activities'][$activity_nid]);
        }
      }

      return $access;
    }
  }

}

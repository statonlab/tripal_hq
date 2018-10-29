<?php

/**
 * Tripal jobs callback
 *
 * @param $args .  requires 'uid' key.
 */
function tripal_hq_permissions_infer_permissions($args) {
  $uid = $args['uid'];

  //Get parent user permissions.

  $parents = tripal_hq_permissions_get_root_permissions($uid);

  //For each permission, find all child records and add them.

  if (!$parents) {
    tripal_log('user !uid had no permissions to set.', TRIPAL_WARNING);
    return;
  }
  foreach ($parents as $parent) {
    tripal_hq_permissions_set_child_permissions($parent);
  }
}

/**
 * @param $uid
 *
 * @return array
 */
function tripal_hq_permissions_get_root_permissions($uid) {


  $permissions = db_select('tripal_hq_permissions', 't')
    ->fields('t')
    ->condition('uid', $uid)
    ->execute()
    ->fetchAll();

  return $permissions;
}

function tripal_hq_permissions_set_child_permissions($parent) {
  $base = $parent->base_table;
  $record_id = $parent->record_id;


  return TRUE;
}

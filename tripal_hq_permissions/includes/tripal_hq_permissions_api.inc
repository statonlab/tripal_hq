<?php

/**
 * Tripal jobs callback
 * @param $args.  requires 'uid' key.
 */
function tripal_hq_permissions_infer_permissions($args){
  $uid = $args['uid'];

  //Get parent user permissions.

  //For each permission, find all child records and add them.

}

/**
 * @param $uid
 *
 * @return array
 */
function tripal_hq_permissions_get_root_permissions($uid){

  $permissions = [];

  return $permissions;
}
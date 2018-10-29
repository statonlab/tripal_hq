<?php
/**
 * @file:  This form is a menu of all admins with Chado-specific permissions.
 *   The superadmin can grant subadmins control over specific regions of Chado.
 *   To start this is controlled via EITHER organisms or projects.
 */

/**
 * Implements hook_form()
 */
function tripal_hq_permissions_manage_admins_form($form, &$form_state) {

  $instructions = '';

  $form['instructions'] = [
    '#markup' => $instructions,
  ];

  $header = [
    'User Name',
    'Number of admin records',
    'Set Permissions'
  ];
  $deputies = tripal_hq_permissions_get_deputies_query($header);

  $rows = [];

  foreach ($deputies as $entry) {

   $permissions = tripal_hq_permissions_get_users_specific_permissions($entry->uid);

$link = l('Assign', '/admin/tripal/tripal_hq/chado_permissions/' .$entry->uid);
    $row = [
      $entry->name,
      count($permissions),
      $link
    ];

    $rows[] = $row;

  }

  $table = theme('table', ['rows' => $rows, 'header' => $header]);
  $form['table'] = [
    '#markup' => $table,
  ];

  return $form;
}

/**
 *  * Get users elligible to admin Chado content and their specific permissions.
 *
 * @param $header
 *
 * @return mixed
 */
function tripal_hq_permissions_get_deputies_query($header) {

  $query = db_select('users', 'u');
  $query->join('users_roles', 'ur', 'ur.uid = u.uid');
  $query->join('role_permission', 'p', 'ur.rid = p.rid');
  $query->condition('u.uid', 0, '!=');
  $query->condition('u.uid', 1, '!=');
  $query->condition('p.permission', 'tripal_hq_permissions deputy');
  $query->fields('u', ['uid', 'name', 'mail']);

  //add paging
  $results = $query->extend('TableSort')
    ->orderByHeader($header)
    ->extend('PagerDefault')
    ->limit(10)
    ->execute()
    ->fetchAll();

  return $results;
}

/**
 * Get all permissions set for the given user.
 *
 * @param $uid
 *
 * @return mixed
 */
function tripal_hq_permissions_get_users_specific_permissions($uid) {

  $query = db_select('tripal_hq_permissions', 'ths');
  $query->fields('ths');
  return $query->execute()->fetchAll();

}
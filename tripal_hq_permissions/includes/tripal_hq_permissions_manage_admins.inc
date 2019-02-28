<?php

/**
 * @file
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
    'Name' => ['data' => t('User Name'), 'field' => 'u.name'],
    'Number of admin records' => ['data' => t('Number of Base Permissions'), 'field' => 'thqp.count'],
    'Set Permissions',
  ];

  $deputies = tripal_hq_permissions_get_deputies_query($header);

  $rows = [];

  foreach ($deputies as $entry) {

    $link = l('Assign', '/admin/tripal/tripal_hq/chado_permissions/' . $entry->uid);
    $row = [
      $entry->name,
      $entry->count,
      $link,
    ];

    $rows[] = $row;

  }

  $table = theme('table', ['rows' => $rows, 'header' => $header]);
  $table .= theme('pager');

  $form['table'] = [
    '#markup' => $table,
  ];

  return $form;
}

/**
 * Get users elligible to admin Chado content and their specific permissions.
 *
 * @param array $header
 *   Passed into the query orderByHeader() method.
 *
 * @return mixed
 *   The table query.
 */
function tripal_hq_permissions_get_deputies_query(array $header) {

  $query = db_select('users', 'u')
    ->extend('PagerDefault')
    ->extend('TableSort');

  $query->join('users_roles', 'ur', 'ur.uid = u.uid');
  $query->join('role_permission', 'p', 'ur.rid = p.rid');
  $query->condition('u.uid', 0, '!=');
  $query->condition('u.uid', 1, '!=');
  $query->condition('p.permission', 'tripal_hq_permissions deputy');
  $query->fields('u', ['uid', 'name', 'mail']);

  $query->leftJoin('tripal_hq_permissions', 'thqp', 'thqp.uid = u.uid');
  $query->groupBy('u.uid');
  $query->addExpression('COUNT(thqp.id)', 'count');

  $results = $query
    ->orderByHeader($header)
    ->limit(25)
    ->execute();

  return $results;
}

/**
 * Get all BASE permissions set for the given user.
 *
 * @param string $uid
 *   Drupal user UID.
 *
 * @return mixed
 *   Array of HQ permissions for given user.
 */
function tripal_hq_permissions_get_users_specific_permissions(string $uid) {

  $query = db_select('tripal_hq_permissions', 'ths');
  $query->fields('ths');
  $query->condition('uid', $uid);
  return $query->execute()->fetchAll();

}

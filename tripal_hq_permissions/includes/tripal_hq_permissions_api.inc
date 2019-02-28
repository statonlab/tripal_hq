<?php

/**
 * @file
 * API for interacting with HQ permissions.
 */

/**
 * Tripal jobs callback.
 *
 * @param int $uid
 *   Drupal user id.
 */
function tripal_hq_permissions_infer_permissions(int $uid) {

  // Get parent user permissions.
  $parents = tripal_hq_permissions_get_root_permissions($uid);

  // For each permission, find all child records and add them.
  if (!$parents) {
    tripal_log('user !uid had no permissions to set.', TRIPAL_WARNING);
    return;
  }

  // Set hq_submission permissions.
  $submissions = db_select('tripal_hq_submission', 't')
    ->fields('t')
    ->execute();

  foreach ($submissions as $submission) {

    $data = unserialize($submission->data);

    tripal_hq_permissions_process_entity_permissions($submission->id, $data);
  }
}

/**
 * Get HQ permissions set for a user.
 *
 * @param int $uid
 *   Drupal user ID.
 *
 * @return array
 *   Array of tripal_hq_permissions.
 */
function tripal_hq_permissions_get_root_permissions(int $uid) {

  $permissions = db_select('tripal_hq_permissions', 't')
    ->fields('t')
    ->condition('uid', $uid)
    ->execute()
    ->fetchAll();

  return $permissions;
}

/**
 * Checks new submission again HQ permission.
 *
 *    When a new submission is created, check its fields and see if this falls
 *    under a parent permission.  For example, an organism created in a project
 *    where that project has hq_permissions setup.
 *
 * @param int $submission_id
 *   Tripal HQ permission ID.
 */
function tripal_hq_permissions_process_submission(int $submission_id) {
  $submission = db_select('tripal_hq_submission', 't')
    ->fields('t')
    ->condition('id', $submission_id)
    ->execute()
    ->fetchObject();
  $entity = unserialize($submission->data);

  tripal_hq_permissions_process_entity_permissions($submission_id, $entity);
}

/**
 * Links a request to relevant records.
 *
 *   For a given record specified via record_id and base table, retrieve all
 *  users signed up for permissions for it in the tripal_hq_permissions table.
 *  Link this submission to that user via the tripal_hq_submission_permissions
 *  table.
 *
 * @param string $base
 *   Chado base table.  organism, project.
 * @param string $record_id
 *   the record ID.
 * @param string $submission_id
 *   HQ Submission ID.
 */
function tripal_hq_permissions_link_request(string $base, string $record_id, string $submission_id) {

  $permissions = db_select('tripal_hq_permissions', 't')
    ->fields('t', ['id'])
    ->condition('base_table', $base)
    ->condition('record_id', $record_id)
    ->execute()
    ->fetchAll();

  if (!$permissions) {
    return;
  }
  foreach ($permissions as $permission) {

    db_insert('tripal_hq_submission_permissions')
      ->fields([
        'hq_permission_id' => $permission->id,
        'submission_id' => $submission_id,
      ])
      ->execute();
  }
}

/**
 * Process permissions associated with an HQ submission.
 *
 * @param string $submission_id
 *   Tripal HQ submission ID.
 * @param TripalEntity $entity
 *   The proposed Tripal entity.
 */
function tripal_hq_permissions_process_entity_permissions(string $submission_id, TripalEntity $entity) {

  $base = $entity->chado_table;
  if ($base == 'organism' || $base == 'project') {
    // Do we care if its an organism or project?  I actually dont think so.
  }

  // Organism base table field?
  if (isset($entity->obi__organism)) {

    $organisms = $entity->obi__organism;

    foreach ($organisms['und'] as $organism) {

      $organism_id = $organism['value'];

      tripal_hq_permissions_link_request('organism', $organism_id, $submission_id);
    }
  }

  if (isset($entity->obi__organism_linker)) {

    $organisms = $entity->obi__organism_linker;

    foreach ($organisms['und'] as $organism) {

      // field:  see https://github.com/statonlab/tripal_manage_analyses/blob/7ca149463cad59dcb626389dc713cf0bc4a5610e/includes/TripalFields/obi__organism_linker/obi__organism_linker.inc#L173-L184
      $organism_id = $organism['value']['organism_id'];

      tripal_hq_permissions_link_request('organism', $organism_id, $submission_id);

    }
  }
  // TODO: project fields.  THey don't exist yet though i think.
  return;
}

/**
 * Get relevant admins for a given submission.
 *
 * @param string $record_id
 *   HQ submission record id.
 * @param string $record_type
 *   Chado base table string name.
 *
 * @return array
 *   An array of admins structured [uid => email].
 */
function tripal_hq_permissions_get_admins(string $record_id, string $record_type) {
  $query = db_select('tripal_hq_permissions', 't')
    ->fields('t', ['uid'])
    ->condition('base_table', $record_type)
    ->condition('record_id', $record_id);
  $query->join('users', 'u', 'u.uid = t.uid');
  $query->fields('u', ['mail']);
  $results = $query->execute()
    ->fetchAll();

  $admins = [];

  foreach ($results as $result) {
    $admins[$result->uid] = $result->mail;
  }
  return $admins;
}

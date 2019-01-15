<?php

/**
 * @file
 * HQ API Access points.
 */

/**
 * Gets a submission object by ID.
 *
 * @param int $submission_id
 *   HQ submission ID.
 *
 * @return mixed
 *   The submission object (or false if no match).
 */
function tripal_hq_get_submission_by_id(int $submission_id) {
  return db_query(
    'SELECT * FROM {tripal_hq_submission} WHERE id=:id', [
      ':id' => $submission_id,
    ]
  )->fetchObject();
}

/**
 * Fetch all submissions for a given user (minus serialized data).
 *
 * @param mixed $user
 *   Drupal user object.
 *
 * @return mixed
 */
function tripal_hq_get_user_submissions($user) {
  $uid = $user->uid;

  $results =
    db_select('public.tripal_hq_submission', 't')->fields('t')->condition(
      'uid', $uid
    )->orderBy('id', 'desc')->execute()->fetchAll();

  return $results;
}

/**
 * Save and publish the entity.
 *
 * @param object $entity
 *   The entity object. Normally, this comes from
 *   unserializing the data field in tripal_hq_submissions table.
 *
 * @return bool|int The entity id or FALSE on failure.
 */
function tripal_hq_publish_entity($entity) {
  $controller = entity_get_controller('TripalEntity');
  return $controller->save($entity);
}

/**
 * Approve and publish a submission.
 *
 * @param object $submission
 *   Submission object.
 *
 * @return object|boolean Entity. The published entity or FALSE on failure.
 */
function tripal_hq_approve_submission($submission) {
  $entity = unserialize($submission->data);
  $entity = tripal_hq_publish_entity($entity);

  if (!$entity) {
    return FALSE;
  }

  db_update('tripal_hq_submission')->fields(
    [
      'status' => 'approved',
      'entity_id' => $entity->id,
    ]
  )->condition('id', $submission->id)->execute();

  return $entity;
}

/**
 * Reject a submission.
 *
 * @param object $submission
 *   Submission object.
 *
 * @return boolean TRUE on success or FALSE otherwise.
 */
function tripal_hq_reject_submission($submission) {

  if ($submission->status !== 'pending') {
    throw new Exception('Unable to reject a non-pending submission');
  }

  db_update('tripal_hq_submission')->fields([
    'status' => 'rejected',
  ])->condition('id', $submission->id)->execute();

  return TRUE;
}

/**
 * Fetch all submissions sorted by ID.
 *
 * @param string $status
 *   One of "approved", "rejected", "pending".
 *
 * @return array of requests, or null if none.
 */
function tripal_hq_submissions($status = NULL) {
  $submissions =
    db_select('tripal_hq_submission', 't')->fields('t')->orderBy('id', 'desc');

  if (!is_null($status)) {
    $submissions->condition('status', $status);
  }

  $submissions = $submissions->execute()->fetchAll();
  return $submissions;
}

/**
 * Retrieve a submission by entity_id.
 *
 * @param $entity_id
 *
 * @return mixed
 */
function tripal_hq_submission_by_entity_id($entity_id) {
  return db_select('tripal_hq_submission', 't')
    ->fields('t')
    ->orderBy('id', 'desc')
    ->condition('entity_id', $entity_id)
    ->execute()
    ->fetchObject();
}

/**
 * Delete a submission.
 *
 * @param $submission
 *
 * @return int
 */
function tripal_hq_delete_submission($submission) {
  tripal_hq_delete_comment_node($submission);

  return db_delete('tripal_hq_submission')
    ->condition('id', $submission->id)
    ->execute();
}

/**
 * @param $submission_id
 *   - the tripal_hq_submission.id
 *
 * @param $event
 *   -  the event key. Valid keys:  "submit", "approve", "reject"
 */
function tripal_hq_send_emails($submission_id, $event) {

  $submission = tripal_hq_get_submission_by_id($submission_id);

  $user_object = user_load($submission->uid);

  $user_email = $user_object->mail;

  $site_email = variable_get('site_mail');
  $relevant_admins = tripal_hq_get_curator_emails();

  switch ($event) {
    case "submit":

      $site_settings = variable_get('tripal_hq_submit_request');

      if (!empty($site_settings['User'])) {
        // From site, to end user.
        drupal_mail(
          'tripal_hq', 'user_submit_notice', $user_email, language_default(),
          $submission, $site_email, TRUE
        );
      }
      // From site to site_admin.
      if (!empty($site_settings['Admin'])) {

        drupal_mail(
          'tripal_hq', 'admin_submit_notice', $relevant_admins,
          language_default(), $submission, $site_email, TRUE
        );
      }

      break;

    case "approve":
      $site_settings = variable_get('tripal_hq_approval');

      if (!empty($site_settings['User'])) {

        drupal_mail(
          'tripal_hq', 'user_approve_notice', $user_email, language_default(),
          $submission, $site_email, TRUE
        );
      }
      // From site to site_admin.
      if (!empty($site_settings['Admin'])) {

        drupal_mail(
          'tripal_hq', 'admin_approve_notice', $relevant_admins,
          language_default(), $submission, $site_email, TRUE
        );
      }

      break;

    case "reject":
      $site_settings = variable_get('tripal_hq_denied');

      if (!empty($site_settings['User'])) {

        drupal_mail(
          'tripal_hq', 'user_reject_notice', $user_email, language_default(),
          $submission, $site_email, TRUE
        );
      }
      // From site to site_admin.
      if (!empty($site_settings['Admin'])) {

        drupal_mail(
          'tripal_hq', 'admin_reject_notice', $relevant_admins,
          language_default(), $submission, $site_email, TRUE
        );
      }

      break;
  }
}

/**
 * Gets all users with tripal_hq admin permission.
 * In the future this could/would use chado-specific roles.
 *
 * @return string, list of emails suitable for drupal_mail hook
 */
function tripal_hq_get_curator_emails() {

  $query = db_select('users', 'u')->fields('u', ['mail']);
  $query->join('users_roles', 'ur', 'ur.uid = u.uid');
  $query->join('role_permission', 'rp', 'rp.rid = ur.rid');
  $query->condition('rp.permission', 'access tripal_hq admin');
  $query->condition('rp.module', 'tripal_hq');

  $results = $query->execute()->fetchAll();
  $mail = [];

  if (!$results) {
    return '';
  }
  foreach ($results as $result) {
    $mail[] = $result->mail;
  }
  return implode(', ', $mail);
}

/**
 * Submission comments count.
 *
 * @param \stdClass $submission
 *   The submission object.
 *
 * @return int
 *   The number of comments related to a given submission.
 */
function tripal_hq_get_comments_count(stdClass $submission) {
  if ($submission->nid) {
    $comment_stats =
      db_select('node_comment_statistics', 'NCS')
        ->fields('NCS')
        ->condition('nid', $submission->nid, '=')
        ->execute()
        ->fetchObject();

    if ($comment_stats) {
      return $comment_stats->comment_count;
    }
  }

  return 0;
}

/**
 * Delete the comments node.
 *
 * @param \stdClass $submission
 *   The submission object.
 *
 *   This function does not return a value.
 */
function tripal_hq_delete_comment_node(stdClass $submission) {
  $nid = $submission->nid;

  if (!$nid) {
    return;
  }

  node_delete($nid);
}

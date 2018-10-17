<?php

/**
 * @param $submission_id
 *
 * @return mixed
 */
function tripal_hq_get_submission_by_id($submission_id) {
  return db_query('SELECT * FROM {tripal_hq_submission} WHERE id=:id', [
    ':id' => $submission_id,
  ])->fetchObject();
}

/**
 * Fetch all submissions for a given user (minus serialized data).
 *
 * @param $user
 *
 * @return mixed
 */
function tripal_hq_get_user_submissions($user) {
  $uid = $user->uid;

  $results = db_select('public.tripal_hq_submission', 't')
    ->fields('t')
    ->condition('uid', $uid)
    ->orderBy('id', 'desc')
    ->execute()
    ->fetchAll();

  return $results;
}

/**
 * Save and publish the entity.
 *
 * @param object $entity The entity object. Normally, this comes from
 *        unserializing the data field in tripal_hq_submissions table.
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
 * @param object $submission Submission object.
 *
 * @return object|boolean Entity. The published entity or FALSE on failure.
 */
function tripal_hq_approve_submission($submission) {
  $entity = unserialize($submission->data);
  $entity = tripal_hq_publish_entity($entity);

  if (!$entity) {
    return FALSE;
  }

  db_update('tripal_hq_submission')->fields([
    'status' => 'approved',
    'entity_id' => $entity->id,
  ])->condition('id', $submission->id)->execute();

  return $entity;
}

/**
 * Reject a submission.
 *
 * @param object $submission Submission object.
 *
 * @return boolean TRUE on success or FALSE otherwise.
 */
function tripal_hq_reject_submission($submission) {
  db_update('tripal_hq_submission')->fields([
    'status' => 'rejected',
  ])->condition('id', $submission->id)->execute();

  return TRUE;
}

/**
 * Fetch all submissions sorted by ID.
 *
 * @param string $status One of "approved", "rejected", "pending"
 *
 * @return array of requests, or null if none.
 */
function tripal_hq_submissions($status = NULL) {
  $submissions = db_select('tripal_hq_submission', 't')
    ->fields('t')
    ->orderBy('id', 'desc');

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
  return db_delete('tripal_hq_submission')
    ->condition('id', $submission->id)
    ->execute();
}


/**
 * @param $submission_id - the tripal_hq_submission.id
 *
 * @param $event -  the event key. Valid keys:  "submit", "approve", "reject"
 */

function tripal_hq_send_emails($submission_id, $event) {

  $submission = tripal_hq_get_submission_by_id($submission_id);

  $user_email = db_select('user', 't')
    ->addField('t', 'mail')
    ->condition('uid', $submission->uid)
    ->execute()
    ->fetchobject();

  $site_email = variable_get('site_mail');
  $relevant_admins = tripal_hq_get_curator_emails();

  switch ($event) {
    case "submit":
      
      //From site, to end user
      drupal_mail('tripal_hq', 'user_submit_notice', $user_email, language_default(), $values, $site_email, TRUE);
      //from site to site_admin
      drupal_mail('tripal_hq', 'admin_submit_notice', $relevant_admins, language_default(), $values, $site_email, TRUE);

      break;
    case "approve":

      drupal_mail('tripal_hq', 'user_approve_notice', $user_email, language_default(), $values, $site_email, TRUE);
      //from site to site_admin
      drupal_mail('tripal_hq', 'admin_approve_notice', $relevant_admins, language_default(), $values, $site_email, TRUE);

      break;
    case "reject":
      drupal_mail('tripal_hq', 'user_reject_notice', $user_email, language_default(), $values, $site_email, TRUE);
      //from site to site_admin
      drupal_mail('tripal_hq', 'admin_reject_notice', $relevant_admins, language_default(), $values, $site_email, TRUE);


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

  $query = db_select('users', 'u')
    ->fields('u', ['mail']);
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

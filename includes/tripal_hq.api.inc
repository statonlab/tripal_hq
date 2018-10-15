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
  $requests = db_select('public.tripal_hq_submission', 't')
    ->fields('t')
    ->orderBy('id', 'desc');

  if (!is_null($status)) {
    $requests->condition('status', $status);
  }

  $requests = $requests->execute()->fetchAll();
  return $requests;
}

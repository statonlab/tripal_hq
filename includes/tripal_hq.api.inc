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
 * @return bool|int
 */
function tripal_hq_publish_entity($entity) {
  /** @var \TripalEntityController $controller */
  $controller = entity_get_controller('TripalEntity');
  return $controller->save($entity);
}

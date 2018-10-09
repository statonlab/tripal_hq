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

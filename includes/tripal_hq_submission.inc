<?php

function tripal_hq_submission_view($submission_id) {
  $submission = tripal_hq_get_submission_by_id($submission_id);

  if (empty($submission)) {
    drupal_not_found();
    return;
  }

  $entity = unserialize($submission->data);
  $view = tripal_view_entity($entity);

  return $view;
}

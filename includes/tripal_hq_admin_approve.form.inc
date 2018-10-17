<?php

/**
 * Approve or reject a submission based on $op.
 *
 * @param array $form
 * @param array $form_state
 * @param string $op Either "approve" or "reject"
 * @param int $sid Submission id
 *
 * @return array
 */
function tripal_hq_admin_control_form($form, &$form_state, $op, $sid) {
  $submission = tripal_hq_get_submission_by_id($sid);
  if (empty($submission)) {
    drupal_not_found();
    return [];
  }

  $bundle_id = $submission->bundle_id;
  if ($op === 'reject') {
    $message = "Are you sure you want to reject " . l($submission->title,
        '/tripal_hq/bio_data/edit/' . $bundle_id . '/' . $sid) . "?";
  }
  else {
    $message = "Are you sure you want to approve " . l($submission->title,
        '/tripal_hq/bio_data/edit/' . $bundle_id . '/' . $sid) . "?";
  }

  $form['submission_id'] = [
    '#type' => 'hidden',
    '#value' => $sid,
  ];

  $form['operation'] = [
    '#type' => 'hidden',
    '#value' => $op,
  ];

  $form['confirmation_message'] = [
    '#type' => 'item',
    '#markup' => $message,
  ];

  $form['submit'] = [
    '#type' => 'submit',
    '#value' => $op === 'reject' ? 'Reject Submission' : 'Approve and Publish Submission',
  ];

  return $form;
}

/**
 * Validate the request.
 *
 * @param $form
 * @param $form_state
 */
function tripal_hq_admin_control_form_validate($form, &$form_state) {
  $values = $form_state['values'];
  $op = isset($values['operation']) ? $values['operation'] : '';
  $sid = isset($values['submission_id']) ? $values['submission_id'] : '';

  if (empty($sid) || empty($op)) {
    form_set_error('submission_id',
      'Please provide a valid submission id and a valid operation type.');
  }
}

/**
 * Approve and publish or reject a submission.
 *
 * @param $form
 * @param $form_state
 */
function tripal_hq_admin_control_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  $op = $values['operation'];
  $sid = $values['submission_id'];

  $submission = tripal_hq_get_submission_by_id($sid);

  if ($op === 'reject') {
    $rejected = tripal_hq_reject_submission($submission);
    if ($rejected) {

      tripal_hq_send_emails($sid, 'accept');

      drupal_set_message('Submission rejected successfully');
      drupal_goto('tripal_hq/admin');
    }
    else {
      drupal_set_message('Unable to reject submission', 'error');
    }

    return;
  }

  $entity = tripal_hq_approve_submission($submission);

  if ($entity) {

    tripal_hq_send_emails($sid, 'accept');

    drupal_set_message('Entity published successfully');
    drupal_goto("bio_data/$entity->id");
    return;
  }

  drupal_set_message('Unable to publish entity', 'error');
}

<?php

/**
 * @file
 * Administrator form to approve requests.
 */

/**
 * Approve or reject a submission based on $op.
 *
 * @param array $form
 * @param array $form_state
 * @param string $op
 *   Either "approve" or "reject".
 * @param int $sid
 *   Submission id.
 *
 * @return array
 */
function tripal_hq_imports_admin_control_form($form, &$form_state, $op, $sid) {

  $submission = tripal_hq_imports_get_submission_by_id($sid);
  if (empty($submission)) {
    $form['warning'] = [
      '#type' => 'markup',
      '#markup' => '<div class="messages error">We were unable to find the submission. Please contact an administrator.</div>',
    ];
    return $form;
  }

  $importer_class = $submission->class;

  // Determine the label.
  tripal_hq_load_include_importer_class($importer_class);
  $importer_label = $importer_class::$name;

  // Title.
  $submission_title = $importer_label . ' submitted on ' . date('M d Y', $submission->created_at);

  // Message based on approve/reject.
  if ($op === 'reject') {
    $message = "Are you sure you want to reject the " . l($submission_title,
        '/tripal_hq/bio_data/import-data/edit/' . $importer_class . '/' . $sid) . "?";
  }
  else {
    $message = "Are you sure you want to approve the " . l($submission_title,
        '/tripal_hq/bio_data/import-data/edit/' . $importer_class . '/' . $sid) . "?";
  }

  // Now for the form.
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
    '#value' => $op === 'reject' ? 'Reject Submission' : 'Approve and Import Submission',
  ];

  return $form;
}

/**
 * Validate the request.
 *
 * @param $form
 * @param $form_state
 */
function tripal_hq_imports_admin_control_form_validate($form, &$form_state) {
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
function tripal_hq_imports_admin_control_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  $op = $values['operation'];
  $sid = $values['submission_id'];

  $submission = tripal_hq_imports_get_submission_by_id($sid);

  if ($op === 'reject') {
    // @todo implement api function.
    $rejected = tripal_hq_imports_reject_submission($submission);
    if ($rejected) {


      // @todo currently we cant use tripal_hq_send_emails() since it retrieves
      //       the submission with the assumption of db table.
      //tripal_hq_send_emails($sid, 'reject');

      drupal_set_message('Submission rejected successfully');
      if (!isset($form_state['no_redirect'])) {
        drupal_goto('tripal_hq/admin');
      }
      return TRUE;
    }
    else {
      drupal_set_message('Unable to reject submission', 'error');
      return FALSE;
    }

  }

  tripal_hq_imports_approve_submission($submission);

  // @todo currently we cant use tripal_hq_send_emails() since it retrieves
  //       the submission with the assumption of db table.
  //tripal_hq_send_emails($sid, 'approve');

  if (!isset($form_state['no_redirect'])) {
    drupal_goto('tripal_hq/admin');
  }

}

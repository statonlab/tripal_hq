<?php

function tripal_hq_admin_dashboard_form($form, &$form_state) {

  $form['instructions'] = ['#markup' => '<p>User data submissions are listed below.  Click on any row to view that submission.</p>'];

  $requests = tripal_hq_submissions('pending');

  if (!$requests) {
    $form['message'] = [
      '#markup' => '<h4>No Requests</h4><p>There are no pending requests.</p>',
    ];
    return $form;
  }

  $header = [
    'User',
    'Title',
    'Type',
    'Status',
    'Date Created',
    'Approve',
    'Reject',
  ];
  $rows = [];
  $date_format = 'M d Y H:i:s';

  foreach ($requests as $submission) {
    $title = $submission->title;
    $status = $submission->status;
    $created_at = date($date_format, $submission->created_at);
    $bundle_id = $submission->bundle_id;
    $bundle = tripal_load_bundle_entity(['id' => $bundle_id]);
    $updated_at = $submission->updated_at ? date($date_format,
      $submission->updated_at) : '';

    $submitter = user_load($submission->uid);

    if (!$submitter) {
      tripal_set_message(t("Error looking up user !user",
        ['!user' => $submission->uid], TRIPAL_WARNING));
      continue;
    }

    $user_row = l($submitter->name . ' (' . $submitter->mail . ')',
      'user/' . $submitter->uid);

    $row = [
      $user_row,
      l($title,
        '/tripal_hq/bio_data/edit/' . $bundle_id . '/' . $submission->id),
      $bundle->label,
      $status,
      $created_at,
      l('Approve', 'tripal_hq/admin/approve/' . $submission->id),
      l('Reject', 'tripal_hq/admin/reject/' . $submission->id),
    ];
    $rows[] = $row;
  }

  $output = theme('table', [
    'header' => $header,
    'rows' => $rows,
  ]);
  $output .= theme('pager', ['quantity', 20]);

  $form['table'] = ['#markup' => $output];
  return $form;
}

function tripal_hq_admin_dashboard_form_validate($form, &$form_state) {

}

function tripal_hq_admin_dashboard_form_submit($form, &$form_state) {

}

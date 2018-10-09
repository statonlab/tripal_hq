<?php

function tripal_hq_admin_dashboard_form($form, &$form_state) {

  $form['instructions'] = ['#markup' => '<p>User data submissions are listed below.  Click on any row to view that submission.</p>'];

  $requests = tripal_hq_fetch_all_requests();

  if (!$requests) {
    $form['null'] = [
      '#markup' => '<h4>No Requests</h4>
<p>There are no pending or complete requests.</p>',
    ];
    return $form;
  }

  $header = ['User', 'Title', 'Status', 'Date Created', 'Approve', 'Reject'];
  $rows = [];
  $date_format = 'M d Y H:i:s';

  foreach ($requests as $submission) {
    $title = $submission->title;
    $status = $submission->status;
    $created_at = date($date_format, $submission->created_at);
    $updated_at = $submission->updated_at ? date($date_format,
      $submission->updated_at) : '';

    $submbitter = user_load($submission->uid);

    if (!$submbitter) {
      tripal_set_message(t("Error looking up user !user",
        ['!user' => $submission->uid], TRIPAL_WARNING));
      continue;
    }

    $user_row = l($submbitter->name . ' (' . $submbitter->mail . ')',
      'user/' . $submbitter->uid);

    $row = [
      $user_row,
      l($title, 'tripal_hq/bio_data/edit/bundle/' . $submission->id),
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

/**
 * Fetch all submissions whose status is pending.
 *
 * @return array if pending requests, or null if none.
 */
function tripal_hq_fetch_pending_requests() {

  global $user;
  //Todo: allow for only fetching requests relevant to this user.

  $requests = db_select('public.tripal_hq_submission', 't')
    ->fields('t', ['id', 'uid', 'title', 'status', 'timestamp', 'entity_id'])
    ->orderBy('id', 'desc')
    ->condition('t.status', 'pending')
    ->execute()
    ->fetchAll();
  return $requests;
}

/**
 * Fetch all submissions sorted by ID.
 *
 * @return array of requests, or null if none.
 */
function tripal_hq_fetch_all_requests() {
  $requests = db_select('public.tripal_hq_submission', 't')
    ->fields('t')
    ->orderBy('id', 'desc')
    ->execute()
    ->fetchAll();
  return $requests;
}

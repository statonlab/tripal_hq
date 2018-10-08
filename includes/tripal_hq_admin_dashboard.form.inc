<?php

function tripal_hq_admin_dashboard_form($form, &$form_state) {

  $form['instructions'] = ['#markup' => '<p>User data submissions are listed below.  Click on any row to view that submission.</p>'];


  $requests = tripal_hq_fetch_pending_requests();

  if (!$requests) {
    $form['null'] = [
      '#markup' =>
        '<h4>No Requests</h4>
<p>There are no pending or complete requests.</p>',
    ];
    return $form;
  }

  $header = ['User', 'Title', 'Status', 'Date Created', 'View'];
  $rows = [];

  foreach ($requests as $request) {
    $id = $request->id;
    $user_id = $request->uid;
    $title = $request->title;
    $status = $request->status;
    $created = $request->timestamp;
    $entity_id = ISSET($request->entity_id) ? $request->entity_id : NULL;

    $user = db_select('public.users', 'u')
      ->fields('u', ['name', 'mail'])
      ->condition('uid', $user_id)
      ->execute()
      ->fetchObject();

    if (!$user) {
      tripal_set_message(t("Error looking up user !user", ['!user' => $user_id], TRIPAL_WARNING));
      continue;
    }

    $user_row = $user->name . ' (' . $user->mail . ')';

    $row = [
      $user_row,
      $title,
      $status,
      $created,
      'View',
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
    ->fields('t', ['id', 'uid', 'title', 'status', 'timestamp', 'entity_id'])
    ->orderBy('id', 'desc')
    ->execute()
    ->fetchAll();
  return $requests;
}
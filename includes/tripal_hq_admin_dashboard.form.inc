<?php

/**
 * @file
 */

/**
 * Implements hook form().
 * Provides a dashboard for site or content admins to view all user-submitted
 * content.
 *
 * @param $form
 * @param $form_state
 * @param $status
 *   - Valid statuses are: pending, published, rejected, obsolete.
 *
 * @return mixed
 */
function tripal_hq_admin_dashboard_form($form, &$form_state, $status = NULL) {
  $message = '<p>User data submissions are listed below.
                 The tabs above will let you filter submissions
                 based on their status.  Click on any row to view
                 that submission.</p>';

  // Check if the user is a deputy.
  // If so, they only get access to a subset of data.
  $deputy = user_access('tripal_hq_permissions deputy');

  global $user;
  $uid = $user->uid;

  $form['instructions'] = ['#markup' => $message];

  $header = [
    'User' => [
      'data' => t('User'),
      'field' => 't.uid',
    ],
    'Title' => [
      'data' => t('Title'),
      'field' => 'title',
    ],
    'Type' => [
      'data' => t('Content Type'),
      'field' => 'bundle_id',
    ],
    'Status' => [
      'data' => t('Approval Status'),
      'field' => 'status',
    ],

    'Date Created' => [
      'data' => t('Date Created'),
      'field' => 'created_at',
      'sort' => 'dsc',
    ],
    'Comments' => [
      'data' => t('Comments'),
    ],
    'Approve',
    'Reject',
  ];

  if ($deputy && $uid != 0 && $uid != 1) {

    $query = db_select('tripal_hq_submission', 't')
      ->extend('TableSort')
      ->orderByHeader($header)
      ->extend('PagerDefault');
    $query->join('tripal_hq_submission_permissions', 'thsp', 'thsp.submission_id = t.id');
    $query->join('tripal_hq_permissions', 'thp', 'thp.id = thsp.hq_permission_id');
    $query->condition('thp.uid', $uid);

    if ($status) {
      $query->condition('t.status', $status);
    }

    $requests = $query->fields('t')
      ->orderBy('id', 'desc')
      ->limit(10)
      ->execute()
      ->fetchAll();
  }
  else {
    $query = db_select('tripal_hq_submission', 't')
      ->extend('TableSort')
      ->orderByHeader($header)
      ->extend('PagerDefault');

    if ($status) {
      $query->condition('t.status', $status);
    }
    $requests = $query->fields('t')
      ->orderBy('id', 'desc')
      ->limit(10)
      ->execute()
      ->fetchAll();
  }

  if (!$requests) {
    $message = '<h4>no ' . $status . '  requests</h4>';

    $form['message'] = [
      '#markup' => $message,
    ];
    return $form;
  }

  $rows = [];
  $date_format = 'M d Y H:i:s';

  foreach ($requests as $submission) {
    $title = $submission->title;
    $status = $submission->status;
    $created_at = date($date_format, $submission->created_at);
    $bundle_id = $submission->bundle_id;
    $bundle = tripal_load_bundle_entity(['id' => $bundle_id]);
    $updated_at = $submission->updated_at ? date($date_format, $submission->updated_at) : '';
    $comment_count = tripal_hq_get_comments_count($submission);

    $submitter = user_load($submission->uid);

    if (!$submitter) {
      tripal_set_message(t("Error looking up user !user",
        ['!user' => $submission->uid], TRIPAL_WARNING));
      continue;
    }

    $user_row = l(t($submitter->name . ' (' . $submitter->mail . ')'),
      'user/' . $submitter->uid);

    $row = [
      $user_row,
      l(t($title),
        '/tripal_hq/bio_data/edit/' . $bundle_id . '/' . $submission->id),
      $bundle->label,
      $status,
      $created_at,
      $submission->nid ? l(
        'Add Comments (' . $comment_count . ')', 'node/' . $submission->nid
      ) : $comment_count,
      l(t('Approve'), 'tripal_hq/admin/approve/' . $submission->id),
      l(t('Reject'), 'tripal_hq/admin/reject/' . $submission->id),
    ];
    $rows[] = $row;
  }

  $output = theme('table', [
    'header' => $header,
    'rows' => $rows,
  ]);

  $output .= theme('pager', ['quantity', 5]);

  $form['table'] = ['#markup' => $output];
  return $form;
}

/**
 *
 */
function tripal_hq_admin_dashboard_form_validate($form, &$form_state) {

}

/**
 *
 */
function tripal_hq_admin_dashboard_form_submit($form, &$form_state) {

}

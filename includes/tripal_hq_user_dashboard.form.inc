<?php

function tripal_hq_user_dashboard_form($form, &$form_state) {

  global $user;
  $submissions = tripal_hq_get_user_submissions($user);
  

  //TODO: combine with user_data.form.inc

  $header = ['Title', 'Status', 'Date Submitted', 'View/Edit'];
  $rows = [];
  foreach ($submissions as $submission) {
    $id = $submission->id;
    $title = $submission->title;
    $status = $submission->status;
    $timestamp = $submission->timestamp;
    $entity_id = $submission->entity_id;

    $link = l('View/Edit', '/somepath/' . $id);

    if ($entity_id) {
      $link = l('View/Edit', '/bio_data/' . $entity_id);
    }

    $rows[] = [
      $title,
      $status,
      $timestamp,
      $link,
    ];
  }

  $output = theme('table', [
    'header' => $header,
    'rows' => $rows,
  ]);
  $output .= theme('pager', ['quantity', 20]);

  $form['my_submissions'] = ['#markup' => $output];
  return $form;
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
    ->fields('t', ['id', 'title', 'status', 'timestamp', 'entity_id'])
    ->condition('uid', $uid)
    ->execute()
    ->fetchAll();

  return $results;
}

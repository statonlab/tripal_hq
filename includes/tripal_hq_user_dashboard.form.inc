<?php

function tripal_hq_user_dashboard_form($form, &$form_state) {

  global $user;
  $submissions = tripal_hq_get_user_submissions($user);

  //TODO: combine with user_data.form.inc

  $header = ['Title', 'Status', 'Date Submitted', 'Date Updated', 'View/Edit'];
  $rows = [];
  $date_format = 'M d Y H:i:s';
  foreach ($submissions as $submission) {
    $id = $submission->id;
    $title = $submission->title;
    $status = $submission->status;
    $created_at = date($date_format, $submission->created_at);
    $updated_at = $submission->updated_at ? date($date_format, $submission->updated_at) : '';
    $entity_id = $submission->entity_id;

    $link = l('View/Edit', '/tripal_hq/bio_data/edit/bundle/' . $id);

    if ($entity_id) {
      $link = l('View', '/bio_data/' . $entity_id) . ' ' . l('Edit',
          'tripal_hq/bio_data/edit/bundle/' . $id);
    }

    $rows[] = [
      $title,
      ucwords($status),
      $created_at,
      $updated_at,
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

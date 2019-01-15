<?php

/**
 * @file
 * End user dashboard.
 *
 *  Provides a unified view for users to view submissions and submit new
 *   content.
 */

/**
 * Implements hook_form().
 */
function tripal_hq_user_dashboard_form($form, &$form_state) {

  global $user;
  $uid = $user->uid;

  $header = [
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
    'Date Updated',
    'Comments',
    'View/Edit',
  ];

  $submissions = db_select('public.tripal_hq_submission', 't')
    ->fields('t')
    ->condition('uid', $uid)
    ->extend('TableSort')
    ->orderByHeader($header)
    ->extend('PagerDefault')
    ->limit(20)
    ->execute()
    ->fetchAll();

  $rows = [];
  $date_format = 'M d Y H:i:s';

  $bundle_labels = [];

  foreach ($submissions as $submission) {
    $id = $submission->id;
    $title = $submission->title;
    $status = $submission->status;
    $bundle_id = $submission->bundle_id;
    $comment_count = tripal_hq_get_comments_count($submission);

    if (!isset($bundle_labels[$bundle_id])) {
      $label = db_select('tripal_bundle', 't')
        ->fields('t', ['label'])
        ->condition('id', $bundle_id)
        ->execute()
        ->fetchField();

      $bundle_labels[$bundle_id] = $label;
    }

    $label = $bundle_labels[$bundle_id];

    $created_at = date($date_format, $submission->created_at);
    $updated_at = $submission->updated_at ? date(
      $date_format, $submission->updated_at
    ) : '';
    $entity_id = $submission->entity_id;

    $link = l(t('Edit'), '/tripal_hq/bio_data/edit/' . $bundle_id . '/' . $id);

    if ($entity_id) {
      $link = l(t('View'), '/bio_data/' . $entity_id);
    }

    $rows[] = [
      $title,
      $label,
      ucwords($status),
      $created_at,
      $updated_at,
      $submission->nid ? l(
        'Add Comments (' . $comment_count . ')', 'node/' . $submission->nid
      ) : $comment_count,
      $link,
    ];
  }

  $output = theme(
    'table', [
      'header' => $header,
      'rows' => $rows,
    ]
  );

  $form['my_submissions'] = ['#markup' => $output];
  return $form;
}

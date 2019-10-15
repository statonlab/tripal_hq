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

  $items_per_page = 5;

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
    ->limit($items_per_page)
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

    $comments_link = $comment_count;
    if ($submission->nid) {
      $comments_link = l('Add/View Comments (' . $comment_count . ')', 'node/' . $submission->nid);
    }

    $rows[] = [
      $title,
      $label,
      ucwords($status),
      $created_at,
      $updated_at,
      $comments_link,
      $link,
    ];
  }

  $form['my_submissions'] = [
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t("You have no pending content submissions."),
  ];

  $form['pager'] = [
    '#theme' => 'pager',
  ];

  // Now we add in an alter hook to allow submodules to add to this dashboard.
  // To use this after hook in your own module create a function named
  // [yourmodule]_tripal_hq_user_dashboard_alter($form, $form_state) {}
  // and change or add to the form as needed.
  drupal_alter('tripal_hq_user_dashboard', $form, $form_state);

  return $form;
}

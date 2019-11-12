<?php

/**
 * @file
 * User Dashboard.
 */

/**
 * Override the user dashboard to add data import submissions.
 *
 * @param array $form
 *   The default form to be altered.
 * @param array $form_state
 *   The state of the form to be altered.
 * @param string $status
 *   The status of the submissions to show (e.g. pending).
 * @return array
 *   The altered form array.
 */
function tripal_hq_imports_tripal_hq_user_dashboard_alter(&$form, &$form_state) {
   global $user;

   // Number of items per page.
   $items_per_page = 5;

   // Add a caption to differentiate the two tables.
   $form['my_submissions']['#caption'] = '<h3>' . t('Content') . '</h3>';

   // Now start to build my table.
   $header = $form['my_submissions']['#header'];
   $header['Title']['field'] = 'class';

   $rows = [];
   $submissions = db_select('tripal_hq_importer_submission', 't')
     ->fields('t')
     ->condition('uid', $user->uid)
     ->extend('TableSort')
     ->orderByHeader($header)
     ->extend('PagerDefault')
     ->limit($items_per_page)
     ->execute()
     ->fetchAll();

   $date_format = 'M d Y H:i:s';
   $importer_labels = [];

   foreach ($submissions as $submission) {

     $id = $submission->id;
     $status = $submission->status;
     $importer_class = $submission->class;
     $comment_count = tripal_hq_get_comments_count($submission);

     // Determine the label.
     if (!isset($importer_labels[$importer_class])) {

       tripal_hq_load_include_importer_class($importer_class);
       $label = $importer_class::$name;

       $importer_labels[$importer_class] = $label;
     }
     $label = $importer_labels[$importer_class];

     // Dates.
     $created_at = date($date_format, $submission->created_at);
     $updated_at = $submission->updated_at ? date(
       $date_format, $submission->updated_at
     ) : '';

     // Comments.
     $comments_link = $comment_count;
     if ($submission->nid) {
       $comments_link = l(
         t('Add/View Comments (@count)', ['@count' => $comment_count]),
         'node/' . $submission->nid
       );
     }

     // Edit or View link.
     $link = l(t('Edit'), '/tripal_hq/bio_data/import-data/edit/' . $importer_class . '/' . $id);
     if ($status != 'pending') {
       $link = l(t('View'), '/tripal_hq/bio_data/import-data/view/' . $importer_class . '/' . $id);
     }


     // Now compile the row.
     $rows[] = [
       $label,
       'Data File',
       ucwords($status),
       $created_at,
       $updated_at,
       $comments_link,
       $link,
     ];
   }

   $form['my_importer_submissions'] = [
     '#theme' => 'table',
     '#header' => $header,
     '#rows' => $rows,
     '#caption' => '<h3>' . t('Data Files') . '</h3>',
     '#empty' => t("You have no pending data file submissions."),
   ];

   $form['importer_pager'] = [
     '#theme' => 'pager',
     '#element' => 1,
   ];

   return $form;
}

<?php
/**
 * @file
 *
 */

 /**
  * Alter the administration dashboard to add our submissions.
  */
 function tripal_hq_imports_tripal_hq_admin_dashboard_alter(&$form, &$form_state, $status) {
   global $user;
   $uid = $user->uid;

   $items_per_page = 10;

   // Add a caption to differentiate the two tables.
   $form['table']['#caption'] = '<h3>' . t('Content') . '</h3>';

   // Check if the user is a deputy.
   // If so, they only get access to a subset of data.
   $deputy = user_access('tripal_hq_permissions deputy');

   $header = [
     'User' => [
       'data' => t('User'),
       'field' => 't.uid',
     ],
     'Title' => [
       'data' => t('Title'),
       'field' => 'class',
     ],
     'Type' => [
       'data' => t('Content Type'),
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

   // @todo support HQ deputy permissmissions!
   //if ($deputy && $uid != 0 && $uid != 1)

   $query = db_select('tripal_hq_importer_submission', 't')
     ->extend('TableSort')
     ->orderByHeader($header)
     ->extend('PagerDefault');

   if ($status) {
     $query->condition('t.status', $status);
   }
   $requests = $query->fields('t')
     ->orderBy('id', 'desc')
     ->limit($items_per_page)
     ->execute()
     ->fetchAll();

   $date_format = 'M d Y H:i:s';
   $importer_labels = [];

   $rows = [];
   foreach ($requests as $request) {

     $id = $request->id;
     $status = $request->status;
     $importer_class = $request->class;
     $comment_count = tripal_hq_get_comments_count($request);

     // Determine the label.
     if (!isset($importer_labels[$importer_class])) {

       tripal_hq_load_include_importer_class($importer_class);
       $label = $importer_class::$name;

       $importer_labels[$importer_class] = $label;
     }
     $label = $importer_labels[$importer_class];

     // User.
     $submitter = user_load($request->uid);
     if (!$submitter) {
       tripal_set_message(t("Error looking up user !user",
         ['!user' => $request->uid]), TRIPAL_WARNING);
       continue;
     }
     $user_row = l($submitter->name . ' (' . $submitter->mail . ')', 'user/' . $submitter->uid);

     // Dates.
     $created_at = date($date_format, $request->created_at);

     // Comments.
     $comments_link = $comment_count;
     if ($request->nid) {
       $comments_link = l('Add/View Comments (' . $comment_count . ')', 'node/' . $request->nid);
     }

     // Approve or Deny.
     $approve = $reject = '';
     if ($status == 'pending') {
       $approve = l(t('Approve'), 'tripal_hq/admin/data-imports/approve/' . $request->id);
       $reject = l(t('Reject'), 'tripal_hq/admin/data-imports/reject/' . $request->id);
     }


     // Now compile the row.
     $rows[] = [
       $user_row,
       $label,
       'Data File',
       ucwords($status),
       $created_at,
       $comments_link,
       $approve,
       $reject
     ];
   }

   $form['my_importer_requests'] = [
     '#theme' => 'table',
     '#header' => $header,
     '#rows' => $rows,
     '#caption' => '<h3>' . t('Data Files') . '</h3>',
     '#empty' => t("There are no $status data file submissions."),
   ];

   $form['importer_pager'] = [
     '#theme' => 'pager',
     '#element' => 1,
   ];

   return $form;
 }

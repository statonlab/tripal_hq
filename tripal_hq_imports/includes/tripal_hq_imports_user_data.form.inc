<?php

/**
 * @file
 * User submissions.
 */

 /**
  * Provides the page view for user importer submissions.
  *
  * @return array
  *   Array of markup describing bundles.
  */
 function tripal_hq_import_list_importers_page() {
   global $user;
   $page = [];

   // We might also add a warning for *admin* users to use the actual tripal
   // import forms.
   $page['description'] = [
     '#type' => 'markup',
     '#prefix' => '<p>',
     '#markup' => t('Please click a data type below to submit a new data file and associated metadata for admin approval.'),
     '#suffix' => '</p>',
   ];

   $user_has_stuff = 0;
   $importers = tripal_hq_get_importers();
   foreach ($importers as $importer_class) {

     // Pull some important info out of the class.
     $importer_label = $importer_class::$name;
     $importer_machine_name = $importer_class::$machine_name;
     $importer_description = $importer_class::$description;

     // Publication importer is not supported.
     if ($importer_class == 'PubBulkImporter' OR $importer_class == 'OBOImporter') {
       continue;
     }

     // Ensure the user has permission to propose that file type.
     if (($user->uid === 0) OR (!user_access("propose $importer_label"))) {
       continue;
     }
     $user_has_stuff = 1;


     $link = l($importer_label,
        "tripal_hq/bio_data/import-data/$importer_class");
     $page[$importer_machine_name] = [
       '#type' => 'item',
       '#markup' => $link,
       '#description' => $importer_description,
     ];
   }

   if ($user_has_stuff === 0) {
     $page['description'] = [
       '#type' => 'markup',
       '#prefix' => '<p>',
       '#markup' => t('You do not have site permissions to submit data files for consideration.'),
       '#suffix' => '</p>',
     ];

   }

   return $page;
 }

 /**
  * TripalImporter submission form.
  *
  * @param int $importer_class
  *   The class name for the importer to show the form for.
  * @param int $sid
  *   Submission ID.
  *
  * @return array
  *   Renderable array.
  */
function tripal_hq_user_importer_form($form, &$form_state) {
  $importer_class = $form_state['build_info']['args'][0];
  $success = tripal_hq_load_include_importer_class($importer_class);
  $importer_label = $importer_class::$name;
  global $user;

  // Tripal Bulk publication importer not supported.
  if ($importer_class == 'PubBulkImporter' OR $importer_class == 'OBOImporter') {
    return drupal_not_found();
  }

  // Check if we should allow editing/viewing of an existing submission.
  if (isset($form_state['build_info']['args'][1])) {
    $submission_id = $form_state['build_info']['args'][1];
    $op = $form_state['build_info']['args'][2];

    $submission = tripal_hq_imports_get_submission_by_id($submission_id);
    if (is_object($submission)) {
      $importer_form_state = unserialize($submission->data);
      $status = $submission->status;

      $form['submission_id'] = [
        '#type' => 'hidden',
        '#value' => $submission_id,
      ];
    }
    else {
      $form['warning'] = [
        '#type' => 'markup',
        '#markup' => '<div class="messages error">Unable to find your previous submission. Please contact an administrator.</div>',
      ];
      return $form;
    }
  }
  else {
    $importer_form_state = $form_state;
  }

  // Ensure the user has permission to propose that file type.
  if (($user->uid === 0) or (!user_access("propose $importer_label"))) {
    $form['warning'] = [
      '#type' => 'markup',
      '#markup' => '<div class="messages error">You do not have access to propose data files of this type. Please contact an administrator.</div>',
    ];
    return $form;
  }

  drupal_set_title($importer_class::$name);

  // Retrieve the form for the importer.
  module_load_include('inc', 'tripal', 'includes/tripal.importer');
  $form = tripal_get_importer_form($form, $importer_form_state, $importer_class);

  // Most importers don't actually fill in defaults since they expect to be
  // sumbitted only once. As such, we should still fill in the defaults ourselves.
  if (isset($importer_form_state['values'])) {

    // Ensure people do not try to edit submissions which are no longer pending.
    if (($op != 'edit') OR ($status != 'pending')) {
      $op = 'view';
    }

    // Fill the default for the full form and disable if $op is view.
    foreach (element_children($form) as $element_key) {
      tripal_hq_editview_form_field($form[$element_key], $importer_form_state['values'], $element_key, $op);
    }
  }

  // Change the name of the button!
  $form['button']['#value'] = 'Submit';

  return $form;
}

/**
 * Validate the user submission.
 */
function tripal_hq_user_importer_form_validate($form, &$form_state) {
  $importer_class = $form_state['build_info']['args'][0];
  tripal_hq_load_include_importer_class($importer_class);

  // Validate the user input in the same manner as the original importer.
  module_load_include('inc', 'tripal', 'includes/tripal.importer');
  tripal_get_importer_form_validate($form, $form_state);
}

/**
 * Submit the submission.
 */
function tripal_hq_user_importer_form_submit($form, $form_state) {

  $sid = tripal_hq_imports_save_submission($form_state);
  if ($sid) {

    // @todo currently we cant use tripal_hq_send_emails() since it retrieves
    //       the submission with the assumption of db table.
    //tripal_hq_send_emails($sid, 'submit');

    if (isset($form_state['values']['submission_id'])) {
      drupal_set_message(
        'Submission created successfully. We will review your submission and get
         back to you shortly.'
      );
    }
    else {
      drupal_set_message(
        'Submission updated successfully. We will review your submission and get
         back to you shortly.'
      );
    }

    if (!isset($form_state['no_redirect'])) {
      drupal_goto('tripal_hq/bio_data');
    }
  }
  else {
    drupal_set_message(
      'Unable to save your submission. Please contact us to fix the issue.',
      'error'
    );
  }
}

/**
 * Save the submission.
 */
function tripal_hq_imports_save_submission($form_state) {

  global $user;
  $importer_class = $form_state['build_info']['args'][0];
  tripal_hq_load_include_importer_class($importer_class);

  // If we have a submission id then update.
  if (isset($form_state['values']['submission_id'])) {
    $sid = $form_state['values']['submission_id'];
    $data = serialize($form_state);
    $success = db_update('tripal_hq_importer_submission')
      ->fields([
        'data' => $data,
        'updated_at' => time(),
      ])
      ->condition('id', $sid)
      ->execute();

  }
  // Otherwise, insert.
  else {
    // Create the comments node.
    $title = $importer_class::$name . ' Submission from ' . date('M d Y H:i:s');
    $nid = tripal_hq_create_node($title);

    // Save it to the tripal_hq_imports_submission table!
    $data = serialize($form_state);
    $sid = db_insert('tripal_hq_importer_submission')
      ->fields([
        'uid' => $user->uid,
        'nid' => $nid,
        'class' => $importer_class,
        'data' => $data,
        'status' => 'pending',
        'created_at' => time(),
      ])->execute();
  }

  return $sid;
}

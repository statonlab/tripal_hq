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
   $page = [];

   // We might also add a warning for *admin* users to use the actual tripal import forms.
   $page['description'] = [
     '#type' => 'markup',
     '#prefix' => '<p>',
     '#markup' => t('Please click a data type below to submit a new data file and associated metadata for admin approval.'),
     '#suffix' => '</p>',
   ];

   $user_has_stuff = 0;
   $importers = tripal_get_importers();
   foreach ($importers as $importer_class) {

     // Pull some important info out of the class.
     $importer_label = $importer_class::$name;
     $importer_machine_name = $importer_class::$machine_name;
     $importer_description = $importer_class::$description;

     // @todo Ensure the user has permission to propose that file type.
     //if (!user_access("propose $importer_machine_name")) {
       //continue;
     //}
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
function tripal_hq_user_importer_form($form, $form_state) {
  $importer_class = $form_state['build_info']['args'][0];
  tripal_load_include_importer_class($importer_class);

  drupal_set_title($importer_class::$name);

  // Retrieve the form for the importer.
  module_load_include('inc', 'tripal', 'includes/tripal.importer');
  $form = tripal_get_importer_form($form, $form_state, $importer_class);

  return $form;
}

/**
 * Validate the user submission.
 */
function tripal_hq_user_importer_form_validate($form, $form_state) {
  $importer_class = $form_state['build_info']['args'][0];
  tripal_load_include_importer_class($importer_class);

  // Validate the user input in the same manner as the original importer.
  module_load_include('inc', 'tripal', 'includes/tripal.importer');
  tripal_get_importer_form_validate($form, $form_state);
}

/**
 * Submit the submission.
 */
function tripal_hq_user_importer_form_submit($form, $form_state) {
  global $user;
  $importer_class = $form_state['build_info']['args'][0];

  // Save it to the tripal_hq_imports_submission table!
  $data = serialize($form_state);
  db_insert('tripal_hq_importer_submission')
    ->fields([
      'uid' => $user->uid,
      'class' => $importer_class,
      'data' => $data,
      'status' => 'pending',
      'created_at' => time(),
      'updated_at' => time(),
    ])->execute();

}

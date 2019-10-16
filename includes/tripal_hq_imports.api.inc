<?php
/**
 * @file
 * Provides utility functions.
 */

/**
 * Retrieve a list of importers.
 */
function tripal_hq_get_importers() {

  $importers = tripal_get_importers();

  // If the api function doesn't work then this is an older version of Tripal.
  // Instead we will retrieve the reporters ourselves so this works in testing.
  if (empty($importers)) {
    $importers = [];
    $modules = module_list(TRUE);
    foreach ($modules as $module) {
      // Find all of the files in the tripal_chado/includes/fields directory.
      $loader_path = DRUPAL_ROOT . '/' . drupal_get_path('module', $module) . '/includes/TripalImporter';
      $loader_files = file_scan_directory($loader_path, '/.inc$/');
      // Iterate through the fields, include the file and run the info function.
      foreach ($loader_files as $file) {
        $class = $file->name;
        module_load_include('inc', $module, 'includes/TripalImporter/' . $class);
        if (class_exists($class) and is_subclass_of($class, 'TripalImporter')) {
          $importers[] = $class;
        }
      }
    }
  }

  return $importers;
}

/**
 * Loads the TripalImporter class file into scope.
 *
 * @param $class
 *   The TripalImporter class to include.
 *
 * @return
 *   TRUE if the field type class file was found, FALSE otherwise.
 *
 * @ingroup tripal_importer_api
 */
function tripal_hq_load_include_importer_class($class) {

  $success = tripal_load_include_importer_class($class);
  if (!$success) {
    $modules = module_list(TRUE);
    foreach ($modules as $module) {
      $file_path = DRUPAL_ROOT . '/' . drupal_get_path('module', $module) . '/includes/TripalImporter/' . $class . '.inc';
      if (file_exists($file_path)) {
        module_load_include('inc', $module, 'includes/TripalImporter/' . $class);
        if (class_exists($class)) {
          return TRUE;
        }
      }
    }
  }

  return $success;
}

/**
 * Returns the full submission.
 *
 * @param $submission_id
 *   The id of the submission you would like returned.
 * @return
 *   An object describing the submission.
 */
function tripal_hq_imports_get_submission_by_id($submission_id) {

  $submission = db_select('tripal_hq_importer_submission', 't')
    ->fields('t')
    ->condition('id', $submission_id)
    ->execute()->fetchObject();
  return $submission;
}

/**
 * Reject a specific submission.
 *
 * @param $submission
 *   An object describing the submission to be rejected.
 * @return
 *   True if the rejection was successful, FALSE otherwise.
 */
function tripal_hq_imports_reject_submission($submission) {

  if (!is_object($submission)) {
    return FALSE;
  }
  if ($submission->status !== 'pending') {
    return FALSE;
  }

  $submission_id = $submission->id;
  return db_update('tripal_hq_importer_submission')
    ->fields([
      'status' => 'rejected',
      'updated_at' => time(),
    ])
    ->condition('id', $submission_id)
    ->execute();
}

/**
 * Approve a specific submission.
 *
 * @param $submission
 *   An object describing the submission to be approved.
 */
function tripal_hq_imports_approve_submission($submission) {

  if (!is_object($submission)) {
    return FALSE;
  }
  if ($submission->status !== 'pending') {
    return FALSE;
  }
  $submission_id = $submission->id;
  $importer_class = $submission->class;

  // Grab the form state from the submission.
  $form_state = unserialize($submission->data);

  // Retrieve the form for the importer.
  module_load_include('inc', 'tripal', 'includes/tripal.importer');
  $form = [];
  $form = tripal_get_importer_form($form, $form_state, $importer_class);

  // Now submit the tripal importer which will create the job.
  tripal_get_importer_form_submit($form, $form_state);

  db_update('tripal_hq_importer_submission')
    ->fields([
      'status' => 'approved',
      'updated_at' => time(),
    ])
    ->condition('id', $submission_id)
    ->execute();
}

/**
 * Updates a given form field for edit (default value) or view (read-only) mode.
 *
 * @param $form_field
 *   A form field defined using the Drupal Form API.
 * @param $values
 *  The full values list for the form.
 * @param $op
 *  One of 'edit' or 'view' where the second will be read-only.
 */
function tripal_hq_editview_form_field(&$form_field, $values, $element_key, $op) {

  if (isset($form_field['#type']) AND ($form_field['#type'] == 'fieldset')) {
    foreach(element_children($form_field) as $child_element) {
      tripal_hq_editview_form_field($form_field[$child_element], $values, $child_element, $op);
    }
  }
  else {
    if (isset($values[$element_key])) {
      $form_field['#default_value'] = $values[$element_key];
    }

    if ($op == 'view') {
      $form_field['#disabled'] = TRUE;
      $form_field['#attributes'] = array('readonly' => 'readonly');
    }
  }

}

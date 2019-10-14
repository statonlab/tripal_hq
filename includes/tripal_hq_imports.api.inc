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

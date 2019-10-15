<?php
namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

module_load_include('inc', 'tripal_hq_imports', 'includes/tripal_hq_imports_user_data.form');

class userDataFormTest extends TripalTestCase {
  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;

  /**
   * Basic test example.
   * Tests must begin with the word "test".
   * See https://phpunit.readthedocs.io/en/latest/ for more information.
   */
  public function testListImportersPage() {

    // First test as the anonymous user.
    $page = tripal_hq_import_list_importers_page();
    $this->assertArrayHasKey('description', $page,
      "Ensure the listing page shows help text to the user.");
    $this->assertStringContainsString('do not', $page['description']['#markup'],
      "Make sure we actually tell them they do not have permission.");

    // Then test as an administrator.
    $this->actingAs(1);
    $page = tripal_hq_import_list_importers_page();

    // This page should at least list the Tripal core importers.
    $this->assertArrayHasKey('chado_gff3_loader', $page,
      "GFF3 Importer was not present on the listing page.");
    $this->assertArrayHasKey('chado_fasta_loader', $page,
      "FASTA Importer was not present on the listing page.");
    $this->assertArrayHasKey('description', $page,
      "Ensure the listing page shows help text to the user.");
  }

  /**
   * Tests tripal_hq_user_importer_form().
   */
  public function testUserImporterForm() {

    // Mock the form state specifying the GFF3 importer.
    $form_state = [
      'build_info' => [
        'args' => [
          'GFF3Importer',
        ],
        'form_id' => 'tripal_hq_user_importer_form',
        'files' => [
          'menu' => 'sites/all/modules/tripal_hq_imports/includes/tripal_hq_imports_user_data.form.inc',
        ],
      ],
      'rebuild' => FALSE,
      'rebuild_info' => [],
      'redirect' => NULL,
      'temporary' => [],
      'submitted' => FALSE,
      'executed' => FALSE,
      'programmed' => FALSE,
      'programmed_bypass_access_check' => TRUE,
      'cache' => FALSE,
      'method' => 'post',
      'groups' => [],
      'buttons' => [],
      'input' => [],
    ];
    // Now execute the form function with the mock form state.
    $form = [];
    $form = tripal_hq_user_importer_form($form, $form_state);

    // Now check key parts of the GFF3 Importer form are present.
    $this->assertArrayHasKey('importer_class', $form,
      "Form array did not specify the importer class.");
    $this->assertEquals('GFF3Importer', $form['importer_class']['#value'],
      "Form array importer class did not match what we expected.");
    $this->assertArrayHasKey('file', $form,
      "Form array did not specify the file.");
    $this->assertArrayHasKey('analysis_id', $form,
      "Form array did not specify the analysis.");
    $this->assertArrayHasKey('organism_id', $form,
      "Form array did not specify the organism.");

  }

  /**
   * Tests tripal_hq_user_importer_form().
   */
  public function testUserImporterEDITForm() {
    global $user;

    // Create some elements to use in our mock.
    $organism = factory('chado.organism')->create();
    $analysis = factory('chado.analysis')->create();
    $data = serialize([1,2,3]);
    $fields = [
      'uid' => $user->uid,
      'nid' => 1,
      'class' => 'GFF3Importer',
      'data' => $data,
      'status' => 'approved',
      'created_at' => time(),
      'updated_at' => time(),
    ];
    db_insert('tripal_hq_importer_submission')
      ->fields($fields)->execute();
    $submission_id = db_select('tripal_hq_importer_submission', 't')
      ->fields('t', ['id'])
      ->condition('class', $fields['class'])
      ->condition('created_at', $fields['created_at'])
      ->execute()->fetchField();

    // Mock the form state specifying the GFF3 importer.
    $form_state = [
      'build_info' => [
        'args' => [
          'GFF3Importer',
          $submission_id,
          'view'
        ],
        'form_id' => 'tripal_hq_user_importer_form',
        'files' => [
          'menu' => 'sites/all/modules/tripal_hq_imports/includes/tripal_hq_imports_user_data.form.inc',
        ],
      ],
      'rebuild' => FALSE,
      'rebuild_info' => [],
      'redirect' => NULL,
      'temporary' => [],
      'submitted' => FALSE,
      'executed' => FALSE,
      'programmed' => FALSE,
      'programmed_bypass_access_check' => TRUE,
      'cache' => FALSE,
      'method' => 'post',
      'groups' => [],
      'buttons' => [],
      'input' => [],
      'values' => [
        'submission_id' => $submission_id,
        'analysis_id' => $analysis->analysis_id,
        'organism_id' => $organism->organism_id,
      ],
    ];
    db_update('tripal_hq_importer_submission')
      ->fields(['data' => serialize($form_state)])
      ->condition('id', $submission_id)
      ->execute();

    // Now execute the form function with the mock form state.
    $form = [];
    $form = tripal_hq_user_importer_form($form, $form_state);

    // Now check key parts of the GFF3 Importer form are present.
    $this->assertArrayHasKey('importer_class', $form,
      "Form array did not specify the importer class.");
    $this->assertEquals('GFF3Importer', $form['importer_class']['#value'],
      "Form array importer class did not match what we expected.");
    $this->assertArrayHasKey('file', $form,
      "Form array did not specify the file.");
    $this->assertArrayHasKey('analysis_id', $form,
      "Form array did not specify the analysis.");
    $this->assertArrayHasKey('organism_id', $form,
      "Form array did not specify the organism.");

    // Try again with a non-existent submission_id.
    $max_id = db_query('SELECT max(id) FROM {tripal_hq_importer_submission}')->fetchField();
    $form_state['values']['submission_id'] = $max_id + 100;
    $form_state['build_info']['args'][1] = $max_id + 100;
    db_update('tripal_hq_importer_submission')
      ->fields(['data' => serialize($form_state)])
      ->condition('id', $submission_id)
      ->execute();
    $form = tripal_hq_user_importer_form($form, $form_state);
    $this->assertArrayHasKey('warning', $form);

  }

  /**
   * Tests tripal_hq_user_importer_form_validate().
   */
  public function testUserImporterFormValidate() {

    // Mock the form state specifying the GFF3 importer.
    $form_state = [
      'build_info' => [
        'args' => [
          'GFF3Importer',
        ],
        'form_id' => 'tripal_hq_user_importer_form',
        'files' => [
          'menu' => 'sites/all/modules/tripal_hq_imports/includes/tripal_hq_imports_user_data.form.inc',
        ],
      ],
      'rebuild' => FALSE,
      'redirect' => NULL,
      'values' => [
        'importer_class' => 'GFF3Importer',
      ],
    ];
    // Now execute the form function with the mock form state.
    $form = [];
    tripal_hq_user_importer_form_validate($form, $form_state);

    // The form state we provided did not have all the expected values
    // Therefore, there should be errors!
    $errors = form_get_errors();
    $this->assertNotEmpty($errors,
      "The form validate did not return errors even though we did not submit all values.");
  }

  /**
   * Tests tripal_hq_user_importer_form_submit().
   */
  public function testUserImporterFormSubmit() {

    // Mock the form state specifying the GFF3 importer.
    $form_state = [
      'build_info' => [
        'args' => [
          'GFF3Importer',
        ],
        'form_id' => 'tripal_hq_user_importer_form',
        'files' => [
          'menu' => 'sites/all/modules/tripal_hq_imports/includes/tripal_hq_imports_user_data.form.inc',
        ],
      ],
      'rebuild' => FALSE,
      'redirect' => NULL,
      'values' => [
        'importer_class' => 'GFF3Importer',
      ],
    ];
    // Now execute the form function with the mock form state.
    $form = [];
    tripal_hq_user_importer_form_submit($form, $form_state);

    // Now check the submission was created.
    $record = db_select('tripal_hq_importer_submission', 'T')
      ->fields('T')
      ->condition('class', 'GFF3Importer')
      ->orderby('T.id', 'DESC')
      ->execute()->fetchObject();
    $this->assertIsObject($record,
      "We were unable to select the new submission.");
    $this->assertEquals(serialize($form_state), $record->data,
      "The most recent GFF3Importer submission data, did not match our submitted form state.");
  }

  /**
   * Tests tripal_hq_user_importer_form_submit().
   */
  public function testUserImporterEDITFormSubmit() {
    global $user;

    // Create some elements to use in our mock.
    $organism = factory('chado.organism')->create();
    $analysis = factory('chado.analysis')->create();
    $data = serialize([1,2,3]);
    $fields = [
      'uid' => $user->uid,
      'nid' => 1,
      'class' => 'GFF3Importer',
      'data' => $data,
      'status' => 'approved',
      'created_at' => time(),
      'updated_at' => time(),
    ];
    db_insert('tripal_hq_importer_submission')
      ->fields($fields)->execute();
    $submission_id = db_select('tripal_hq_importer_submission', 't')
      ->fields('t', ['id'])
      ->condition('class', $fields['class'])
      ->condition('created_at', $fields['created_at'])
      ->execute()->fetchField();

    // Mock the form state specifying the GFF3 importer.
    $form_state = [
      'build_info' => [
        'args' => [
          'GFF3Importer',
          $submission_id,
          'view'
        ],
        'form_id' => 'tripal_hq_user_importer_form',
        'files' => [
          'menu' => 'sites/all/modules/tripal_hq_imports/includes/tripal_hq_imports_user_data.form.inc',
        ],
      ],
      'rebuild' => FALSE,
      'rebuild_info' => [],
      'redirect' => NULL,
      'temporary' => [],
      'submitted' => FALSE,
      'executed' => FALSE,
      'programmed' => FALSE,
      'programmed_bypass_access_check' => TRUE,
      'cache' => FALSE,
      'method' => 'post',
      'groups' => [],
      'buttons' => [],
      'input' => [],
      'values' => [
        'submission_id' => $submission_id,
        'analysis_id' => $analysis->analysis_id,
        'organism_id' => $organism->organism_id,
      ],
    ];
    db_update('tripal_hq_importer_submission')
      ->fields(['data' => serialize($form_state)])
      ->condition('id', $submission_id)
      ->execute();
    // Now execute the form function with the mock form state.
    $form = [];
    tripal_hq_user_importer_form_submit($form, $form_state);

    // Now check the submission was created.
    $record = db_select('tripal_hq_importer_submission', 'T')
      ->fields('T')
      ->condition('class', 'GFF3Importer')
      ->orderby('T.id', 'DESC')
      ->execute()->fetchObject();
    $this->assertIsObject($record,
      "We were unable to select the new submission.");
    $this->assertEquals(serialize($form_state), $record->data,
      "The most recent GFF3Importer submission data, did not match our submitted form state.");
  }
}

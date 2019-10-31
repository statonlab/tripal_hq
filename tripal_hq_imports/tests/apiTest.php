<?php
namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;
use Faker\Factory;

class apiTest extends TripalTestCase {
  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;

  /**
   * Tests tripal_hg_get_importers().
   *
   * @group api
   */
  public function testGetImporters() {

    $importers = tripal_hq_get_importers();
    $this->assertIsArray($importers);
    $this->assertNotEmpty($importers);
  }

  /**
   * Tests tripal_hq_load_include_importer_class().
   *
   * @group api
   */
  public function testLoadImporterClass() {

    // First test one which should work.
    $success = tripal_hq_load_include_importer_class('GFF3Importer');
    $this->assertTrue($success);

    // Next test one which fails.
    $success = tripal_hq_load_include_importer_class(uniqid());
    $this->assertFalse($success);
  }

  /**
   * Tests tripal_hq_imports_get_submission_by_id().
   *
   * @group api
   */
  public function testGetSubmissionByID() {
    global $user;

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
    $sid = db_insert('tripal_hq_importer_submission')
      ->fields($fields)->execute();

    $submission = tripal_hq_imports_get_submission_by_id($sid);
    $this->assertIsObject($submission,
      "Uanble to retrieve submission with tripal_hq_imports_get_submission_by_id().");
    $this->assertEquals($sid, $submission->id,
      "We retrieved a submission but it is not the one we asked for?");
    $this->assertEquals($fields['class'], $submission->class,
      "We retrieved a submission but it is not the one we asked for?");

  }

  /**
   * Tests tripal_hq_imports_reject_submission().
   */
  public function testRejectSubmission() {

    // Test an invalid submission.
    $success = tripal_hq_imports_reject_submission([]);
    $this->assertFalse($success,
      "Must pass an object to tripal_hq_imports_reject_submission().");

    // Try rejecting a non-pending submission.
    $faker = Factory::create();
    $submission = [
      'uid' => 1,
      'id' => $faker->randomDigit(),
      'nid' => $faker->randomDigit(),
      'class' => 'GFF3Importer',
      'data' => serialize([1,2,3]),
      'status' => 'approved',
      'created_at' => $faker->unixTime(),
      'updated_at' => $faker->unixTime(),
    ];
    $submission = (object) $submission;
    $success = tripal_hq_imports_reject_submission($submission);
    $this->assertFalse($success,
        "Must be pending to reject a submission.");

    // Try really rejecting something ;-p.
    $submission = [
      'uid' => 1,
      'nid' => $faker->randomDigit(),
      'class' => 'GFF3Importer',
      'data' => serialize([1,2,3]),
      'status' => 'pending',
      'created_at' => $faker->unixTime(),
      'updated_at' => $faker->unixTime(),
    ];
    $sid = db_insert('tripal_hq_importer_submission')
      ->fields($submission)->execute();
    $submission = tripal_hq_imports_get_submission_by_id($sid);
    $success = tripal_hq_imports_reject_submission($submission);

    $retrieved_submission = tripal_hq_imports_get_submission_by_id($sid);
    $this->assertEquals(1, $success,
        "Unable to reject a valid submission.");
    $this->assertEquals('rejected', $retrieved_submission->status,
      "The retrieved submission is not showing rejected.");

  }

  /**
   * Tests tripal_hq_imports_approve_submission().
   */
  public function testApproveSubmission() {

    // Test an invalid submission.
    $success = tripal_hq_imports_approve_submission([]);
    $this->assertFalse($success,
      "Must pass an object to tripal_hq_imports_reject_submission().");

    // Try rejecting a non-pending submission.
    $faker = Factory::create();
    $submission = [
      'uid' => 1,
      'id' => $faker->randomDigit(),
      'nid' => $faker->randomDigit(),
      'class' => 'GFF3Importer',
      'data' => serialize([1,2,3]),
      'status' => 'approved',
      'created_at' => $faker->unixTime(),
      'updated_at' => $faker->unixTime(),
    ];
    $submission = (object) $submission;
    $success = tripal_hq_imports_approve_submission($submission);
    $this->assertFalse($success,
        "Must be pending to reject a submission.");

    // Try really approving something ;-p.
    // -- Firsst create a submission.
    $submission = [
      'uid' => 1,
      'nid' => $faker->randomDigit(),
      'class' => 'GFF3Importer',
      'data' => serialize([1,2,3]),
      'status' => 'pending',
      'created_at' => $faker->unixTime(),
      'updated_at' => $faker->unixTime(),
    ];
    $sid = db_insert('tripal_hq_importer_submission')
      ->fields($submission)->execute();

    // -- Mock the form state specifying the GFF3 importer.
    $analysis = factory('chado.analysis')->create();
    $organism = factory('chado.organism')->create();
    $form_state = [
      'build_info' => [
        'args' => [
          'GFF3Importer',
          $sid,
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
        'submission_id' => $sid,
        'analysis_id' => $analysis->analysis_id,
        'organism_id' => $organism->organism_id,
        'importer_class' => 'GFF3Importer',
      ],
    ];

    // -- Update the submission with the new form state.
    db_update('tripal_hq_importer_submission')
      ->fields(['data' => serialize($form_state)])
      ->condition('id', $sid)
      ->execute();

    // -- Finally retrieve and approve it!
    $submission = tripal_hq_imports_get_submission_by_id($sid);
    $success = tripal_hq_imports_approve_submission($submission);

    $retrieved_submission = tripal_hq_imports_get_submission_by_id($sid);
    $this->assertNotFalse($success,
        "Unable to approve a valid submission.");
    $this->assertEquals('approved', $retrieved_submission->status,
      "The retrieved submission is not showing approved.");

  }

  /**
   * Tests tripal_hq_editview_form_field().
   *
   * @group api
   */
  public function testEditViewFormField() {
    $form = [
      'element1' => [
        '#type' => 'textfield',
        '#disabled' => FALSE,
      ],
      'element2' => [
        '#type' => 'select',
        '#options' => [1,2,3,4],
      ],
      'containerblahblah' => [
        '#type' => 'fieldset',
        'element3' => [
          '#type' => 'textfield',
          '#disabled' => FALSE,
        ],
        'element4' => [
          '#type' => 'select',
          '#options' => [1,2,3,4],
        ],
        'container222' => [
          '#type' => 'fieldset',
          'element5' => [
            '#type' => 'select',
            '#options' => [1,2,3,4],
          ],
        ],
      ],
    ];
    $values = [
      'element1' => 'fred',
      'element2' => 3,
      'element3' => 'sarah',
      'element4' => 1,
      'element5' => 4,
    ];

    foreach (element_children($form) as $element_key) {
      tripal_hq_editview_form_field($form[$element_key], $values, $element_key, 'view');
    }

    // Check that everything is disabled.
    $this->assertTrue($form['element1']['#disabled'], 'element1 was not disabled.');
    $this->assertTrue($form['element2']['#disabled'], 'element2 was not disabled.');
    $this->assertTrue($form['containerblahblah']['element3']['#disabled'], 'element3 was not disabled.');
    $this->assertTrue($form['containerblahblah']['element4']['#disabled'], 'element4 was not disabled.');
    $this->assertTrue($form['containerblahblah']['container222']['element5']['#disabled'], 'element5 was not disabled.');

    // Check that their default value is set correctly.
    $this->assertEquals('fred', $form['element1']['#default_value'],
      'element1 default value was not set properly.');
    $this->assertEquals(3, $form['element2']['#default_value'],
      'element2 default value was not set properly.');
    $this->assertEquals('sarah', $form['containerblahblah']['element3']['#default_value'],
      'element3 default value was not set properly.');
    $this->assertEquals(1, $form['containerblahblah']['element4']['#default_value'],
      'element4 default value was not set properly.');
    $this->assertEquals(4, $form['containerblahblah']['container222']['element5']['#default_value'],
      'element5 default value was not set properly.');
  }
}

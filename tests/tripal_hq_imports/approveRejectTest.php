<?php
namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

module_load_include('inc', 'tripal_hq_imports', 'includes/tripal_hq_imports_approve.form');

class approveRejectTest extends TripalTestCase {
  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;

  /**
   * Tests tripal_hq_imports_admin_control_form().
   *
   * @group admin
   * @group approve-reject
   * @group hq-imports
   */
  public function testAdminControlForm() {
    $this->actingAs(1);
    global $user;

    // Mock the args.
    // -- form state.
    $form_state = [
      'build_info' => [
        'form_id' => 'tripal_hq_imports_admin_control_form',
        'files' => [
          'menu' => 'sites/all/modules/tripal_hq_imports/includes/tripal_hq_imports_approve.form.inc',
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
    // -- form.
    $form = [];
    // -- operation.
    $op = 'approve';
    // -- submission.
    $data = serialize([1,2,3]);
    $fields = [
      'uid' => $user->uid,
      'nid' => 1,
      'class' => 'GFF3Importer',
      'data' => $data,
      'status' => 'pending',
      'created_at' => time(),
      'updated_at' => time(),
    ];
    $sid = db_insert('tripal_hq_importer_submission')
      ->fields($fields)->execute();

    // Then execute the form function.
    $form = tripal_hq_imports_admin_control_form($form, $form_state, $op, $sid);

    $this->assertIsArray($form, 'Unable to return the form array.');
    $this->assertArrayHasKey('submission_id', $form,
      "The returned form did not have the submission id available?");
    $this->assertEquals($sid, $form['submission_id']['#value'],
      "The submission id was not corrent???");
    $this->assertStringContainsString('approve', $form['confirmation_message']['#markup'],
      "The confirmation message should contain the word approve.");

    // Now do the same for reject.
    $op = 'reject';
    $form = tripal_hq_imports_admin_control_form($form, $form_state, $op, $sid);

    $this->assertIsArray($form, 'Unable to return the form array.');
    $this->assertArrayHasKey('submission_id', $form,
      "The returned form did not have the submission id available?");
    $this->assertEquals($sid, $form['submission_id']['#value'],
      "The submission id was not corrent???");
    $this->assertStringContainsString('reject', $form['confirmation_message']['#markup'],
      "The confirmation message should contain the word reject.");

    // Try with a non-existent submission.
    $max_id = db_query('SELECT max(id) FROM {tripal_hq_importer_submission}')
      ->fetchField();
    $form = tripal_hq_imports_admin_control_form($form, $form_state, $op, $max_id+100);
    $this->assertIsArray($form, 'Unable to return the form array.');
    $this->assertArrayHasKey('warning', $form,
      "The form should show a warning to the user that the submission could not be found.");

  }

  /**
   * Tests tripal_hq_imports_admin_control_form_validate().
   *
   * @group admin
   * @group approve-reject
   * @group hq-imports
   */
  public function testAdminControlFormValidate() {
    $this->actingAs(1);
    global $user;

    // Mock the args.
    // -- form state.
    $form_state = [
      'build_info' => [
        'form_id' => 'tripal_hq_imports_admin_control_form',
        'files' => [
          'menu' => 'sites/all/modules/tripal_hq_imports/includes/tripal_hq_imports_approve.form.inc',
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
    // -- form.
    $form = [];
    // -- operation.
    $op = 'approve';
    // -- submission.
    $data = serialize([1,2,3]);
    $fields = [
      'uid' => $user->uid,
      'nid' => 1,
      'class' => 'GFF3Importer',
      'data' => $data,
      'status' => 'pending',
      'created_at' => time(),
      'updated_at' => time(),
    ];
    $sid = db_insert('tripal_hq_importer_submission')
      ->fields($fields)->execute();
    // -- Finally update form state.
    $form_state['build_info']['args'][0] = $sid;
    $form_state['values'] = [
      'operation' => 'approve',
      'submission_id' => $sid,
    ];

    // Now test a good submission.
    tripal_hq_imports_admin_control_form_validate($form, $form_state);

    // The form state we provided did not have all the expected values
    // Therefore, there should be errors!
    $errors = form_get_errors();
    $this->assertEmpty($errors,
      "The form validate should not return errors for a valid confirmation.");

    // Now remove the values and watch it fail.
    $form_state['values'] = [];
    tripal_hq_imports_admin_control_form_validate($form, $form_state);
    $errors = form_get_errors();
    $this->assertNotEmpty($errors,
      "The form validate did not return errors even though we did not submit all values.");
  }

  /**
   * Tests tripal_hq_imports_admin_control_form_submit().
   */
  public function testAdminControlFormSubmit() {
    $this->actingAs(1);
    global $user;

    // Mock the args.
    // -- form state.
    $form_state = [
      'build_info' => [
        'form_id' => 'tripal_hq_imports_admin_control_form',
        'files' => [
          'menu' => 'sites/all/modules/tripal_hq_imports/includes/tripal_hq_imports_approve.form.inc',
        ],
      ],
      'rebuild' => FALSE,
      'rebuild_info' => [],
      'redirect' => NULL,
      'no_redirect' => TRUE,
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
    // -- form.
    $form = [];
    // -- operation.
    $op = 'approve';
    // -- submission.
    $data = serialize([1,2,3]);
    $fields = [
      'uid' => $user->uid,
      'nid' => 1,
      'class' => 'GFF3Importer',
      'data' => $data,
      'status' => 'pending',
      'created_at' => time(),
      'updated_at' => time(),
    ];
    $sid = db_insert('tripal_hq_importer_submission')
      ->fields($fields)->execute();
    // -- Finally update form state.
    $form_state['build_info']['args'][0] = $sid;
    $form_state['values'] = [
      'operation' => 'approve',
      'submission_id' => $sid,
      'importer_class' => 'GFF3Importer',
    ];
    db_update('tripal_hq_importer_submission')->fields([
      'data' => serialize($form_state),
      'updated_at' => time(),
    ])->condition('id', $sid)->execute();

    // Now test a good approval.
    $success = tripal_hq_imports_admin_control_form_submit($form, $form_state);
    $this->assertNotFalse($success,
      "Submission failed when it shouldn't have.");

    // And test a bad rejection (submission is not pending).
    $form_state['values']['operation'] = 'reject';
    db_update('tripal_hq_importer_submission')->fields([
      'data' => serialize($form_state),
      'updated_at' => time(),
    ])->condition('id', $sid)->execute();
    $success = tripal_hq_imports_admin_control_form_submit($form, $form_state);
    $this->assertFalse($success,
      "Submission should have failed since it is no longer pending.");

    // And a good rejection.
    db_update('tripal_hq_importer_submission')->fields([
      'status' => 'pending',
      'updated_at' => time(),
    ])->condition('id', $sid)->execute();
    $success = tripal_hq_imports_admin_control_form_submit($form, $form_state);
    $this->assertNotFalse($success,
      "Submission failed when it shouldn't have.");

  }
}

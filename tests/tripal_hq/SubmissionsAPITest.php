<?php

namespace Tests\tripal_hq;

use Faker\Factory;
use Faker\Generator;
use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;
use Tests\DatabaseSeeders\UsersTableSeeder;

class SubmissionsAPITest extends TripalTestCase {

  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;

  /**
   * @throws \Exception
   */
  public function testThatWeCanGetSubmissions() {
    $submission = factory('tripal_hq_submission')->create();

    $sub2 = tripal_hq_get_submission_by_id($submission->id);

    $this->assertEquals($sub2->id, $submission->id);
  }

  /**
   * @throws \Exception
   */
  public function testThatWeCanRejectASubmission() {
    $submission = factory('tripal_hq_submission')->create(
      [
        'status' => 'pending',
      ]
    );

    $this->assertTrue(tripal_hq_reject_submission($submission));
    $this->assertEquals(
      tripal_hq_get_submission_by_id($submission->id)->status, 'rejected'
    );
  }

  /**
   * @throws \Exception
   */
  public function testThatWeCantRejectANonPendingSubmission() {
    $submission = factory('tripal_hq_submission')->create(
      [
        'status' => 'rejected',
      ]
    );

    $this->expectException('Exception');
    tripal_hq_reject_submission($submission);
  }

  /**
   * @throws \Exception
   */
  public function testThatWeCanGetASubmissionGivenAnEntityId() {
    $submission = factory('tripal_hq_submission')->create(
      [
        'entity_id' => 11,
      ]
    );

    $sub2 = tripal_hq_submission_by_entity_id($submission->entity_id);

    $this->assertEquals($submission->id, $sub2->id);
  }

  /**
   * @throws \Exception
   */
  public function testThatWeCanDeleteASubmission() {
    $submission = factory('tripal_hq_submission')->create();
    tripal_hq_delete_submission($submission);

    $this->assertEmpty(tripal_hq_get_submission_by_id($submission->id));
  }

  /**
   * @throws \Exception
   */
  public function testThatWeCanGetSubmissionsGivenAUser() {
    $faker                   = Factory::create();
    UsersTableSeeder::$email = $faker->email;

    $user = (new UsersTableSeeder())->up();

    factory('tripal_hq_submission', 10)->create(['uid' => $user->uid]);

    $this->assertGreaterThanOrEqual(10, count(tripal_hq_get_user_submissions($user)));
  }
}

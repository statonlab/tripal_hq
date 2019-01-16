<?php

namespace Tests\tripal_hq;

use StatonLab\TripalTestSuite\DBTransaction;
use Tests\TestCase;

class UserFormsTest extends TestCase {

  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;

  /** @test */
  public function testThatUsersCantAccessOtherPeoplesSubmissions() {
    $user = $this->createUser();
    $this->actingAs($user);

    $user2 = $this->createUser();

    $submission = factory('tripal_hq_submission')->create([
      'uid' => $user2->uid,
      'status' => 'pending',
    ]);

    $response = $this->get('tripal_hq/bio_data/edit/' . $submission->bundle_id . '/' . $submission->id);

    $response->assertStatus(403);
  }

  /** @test */
  public function testThatUsersCanAccessTheirOwnSubmissions() {
    $user = $this->createUser();
    $this->actingAs($user->uid);

    $rid = current(array_keys($user->roles));
    user_role_grant_permissions($rid, ['access tripal_hq user']);
    $this->assertTrue(user_has_role($rid));

    $submission = factory('tripal_hq_submission')->create([
      'uid' => $user->uid,
      'status' => 'pending',
    ]);

    $response = $this->get('tripal_hq/bio_data/edit/' . $submission->bundle_id . '/' . $submission->id);

    $response->assertSuccessful();
  }
}

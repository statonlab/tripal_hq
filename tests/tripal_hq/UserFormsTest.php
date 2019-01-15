<?php

namespace Tests\tripal_hq;

use StatonLab\TripalTestSuite\DBTransaction;
use Tests\TestCase;

class UserFormsTest extends TestCase {

  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;

//  public function testThatUserCanAccessTheirOwnSubmissions() {
//    $user = $this->createUser('administrator');
//
//    user_role_grant_permissions(current(array_keys($user->roles)), ['access tripal_hq user']);
//
//    $this->actingAs($user);
//
//    $submission = factory('tripal_hq_submission')->create([
//      'uid' => $user->uid,
//      'status' => 'pending',
//    ]);
//
//    $response = $this->get('/tripal_hq/bio_data/edit/' . $submission->bundle_id . '/' . $submission->id);
//
//    $response->assertSuccessful();
//  }
}

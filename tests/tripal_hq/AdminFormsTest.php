<?php

namespace Tests\tripal_hq;

use StatonLab\TripalTestSuite\DBTransaction;
use Tests\TestCase;

class AdminFormsTest extends TestCase {

  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;

  /** @test */
  public function testThatAdminDashboardFormIsAccessibleToAdmins() {
    $this->actingAs(1);

    $urls = [
      'tripal_hq/admin',
      'tripal_hq/admin/pending',
      'tripal_hq/admin/approved',
      'tripal_hq/admin/all',
    ];

    foreach ($urls as $url) {
      $this->get($url)->assertStatus(200);
    }
  }

  /** @test */
  public function testThatAdminDashboardFormIsNotAccessibleToNonAdmins() {
    $user = $this->createUser();
    $this->actingAs($user);

    $urls = [
      'tripal_hq/admin',
      'tripal_hq/admin/pending',
      'tripal_hq/admin/approved',
      'tripal_hq/admin/all',
    ];

    foreach ($urls as $url) {
      $this->get($url)->assertStatus(403);
    }
  }
}

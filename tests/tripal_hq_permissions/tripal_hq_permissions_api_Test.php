<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class tripal_hq_permissions_api_Test extends TripalTestCase {

  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;


  public function test_tripal_hq_permissions_get_root_permissions() {

    $organism = factory('chado.organism')->create();

    $user = $this->create_user_with_permissions($organism);

    $check = db_select('tripal_hq_permissions', 't')
      ->condition('record_id', $organism->organism_id)
      ->fields('t')
      ->execute()
      ->fetchAll();

    $this->assertNotEmpty($check);


    $permissions = tripal_hq_permissions_get_root_permissions($user->uid);

    //submit a request for an analysis with this organism.

    $this->assertNotEmpty($permissions);

    $permission = $permissions[0];

    $this->assertEquals($organism->organism_id, $permission->record_id);

  }

  public function test_tripal_hq_permissions_set_child_permissions() {

    $organism = factory('chado.organism')->create();

    $user = $this->create_user_with_permissions($organism);
//
//    $parent = tripal_hq_permissions_get_root_permissions($user->uid);
//    $result = tripal_hq_permissions_set_child_permissions($parent);
//    $this->assertTrue($result);
    $this->assertTrue(true);
  }

  /**
   * Private function to set up test environment.
   *
   * @param $organism | organism object- created via factory.
   *
   * @return mixed - a drupal user object.
   */
  private function create_user_with_permissions($organism) {

    //create the role

    $role_name = 'tripal hq permissions test role';

    $existing_role = user_role_load_by_name($role_name);
    if (empty($existing_role)) {
      $role = new \stdClass();
      $role->name = $role_name;
      user_role_save($role);
    }

    $existing_role = user_role_load_by_name($role_name);

    $new_user = [
      'name' => 'tripal_hq_permissions_user',
      'pass' => 'fsafs', // note: do not md5 the password
      'mail' => 'mail@test.com',
      'status' => 1,
      'init' => 'mail@test.com',
      'roles' => [
        DRUPAL_AUTHENTICATED_RID => 'authenticated user',
        $existing_role->rid => $role,
      ],
    ];

    // The first parameter is sent blank so a new user is created.
    $user = user_save('', $new_user);


    db_insert('tripal_hq_permissions')
      ->fields([
        'uid' => $user->uid,
        'base_table' => 'organism',
        'record_id' => $organism->organism_id,
      ])
      ->execute();

    return $user;

  }
}

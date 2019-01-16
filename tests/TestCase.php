<?php

namespace Tests;

use Faker\Factory;
use StatonLab\TripalTestSuite\TripalTestCase;
use Tests\DatabaseSeeders\UsersTableSeeder;

class TestCase extends TripalTestCase {

  /**
   * Create a new user.
   *
   * @param string $role
   *     The role (one of 'authenticated user' or 'administrator').
   *
   * @return \stdClass
   *     The created user object.
   */
  public function createUser($role = 'authenticated user') {
    $faker = Factory::create();

    $seeder = new UsersTableSeeder();

    $seeder->email = uniqid() . $faker->email;
    $seeder->role = $role;
    $seeder->name = $faker->name . uniqid();

    return (new UsersTableSeeder())->up();
  }
}

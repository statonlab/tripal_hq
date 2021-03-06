<?php

namespace Tests\DatabaseSeeders;

use Faker\Factory;
use StatonLab\TripalTestSuite\Database\Seeder;

class UsersTableSeeder extends Seeder {

  public $email = 'test@example.com';

  public $role = 'authenticated user';

  public $name = NULL;

  /**
   * Seeds the database with users.
   */
  public function up() {
    if (!$this->name) {
      $faker = Factory::create();
      $this->name = $faker->name;
    }

    $new_user = [
      'name' => $this->name,
      'pass' => 'secret',
      'mail' => $this->email,
      'status' => 1,
      'init' => 'Email',
      'roles' => [
        DRUPAL_AUTHENTICATED_RID => 'authenticated user',
      ],
    ];
    // The first parameter is sent blank so a new user is created.
    $user = user_save(new \stdClass(), $new_user);

    //seed a couple of data requests

    //TODO: use the API instead so we can ahe serialized data...
    //todo shouldnt assume user is 2

    //    $user_id = db_select('public.users', 'u')->fields('u', ['uid'])->condition(
    //        'name', 'test user with requests'
    //      )->execute()->fetchField();
    //
    //    $organism =
    //      factory('chado.organism')->create(['genus' => 'HQ_test_organism']);
    //
    //    $this->publish('organism');

    //    $eid = chado_get_record_entity_by_table('organism', $organism->organism_id);

    //    $bundle_id =
    //      db_select('tripal_bundle', 't')->fields('t', ['id'])->condition(
    //          'label', 'Organism'
    //        )->execute()->fetchField();
    //
    //    db_insert('public.tripal_hq_submission')->fields(
    //        [
    //          'uid'        => $user_id,
    //          'title'      => "this request was seeded",
    //          'status'     => 'approved',
    //          'created_at' => time(),
    //          'data'       => '',
    //          'entity_id'  => $eid,
    //          'bundle_id'  => $bundle_id,
    //        ]
    //      )->execute();
    //
    //    db_insert('public.tripal_hq_submission')->fields(
    //        [
    //          'uid'        => $user_id,
    //          'title'      => "this request was seeded and its not approved",
    //          'status'     => 'pending',
    //          'created_at' => time(),
    //          'data'       => '',
    //          'bundle_id'  => $bundle_id,
    //          'entity_id'  => NULL,
    //        ]
    //      )->execute();

    return $user;
  }
}

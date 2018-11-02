<?php

/**
 * @file used to set chado-specific permissions for a given user.
 *
 */

/**
 * Implements hook_form().
 */
function tripal_hq_permissions_edit_admin_form($form, &$form_state, $uid) {

  $form['instructions'] = [
    '#markup' => '
  <p>This form lets you set Chado specific administrative rights for this user.</p>
  <p>This module supports permissions for child content for organisms and projects, meaning if, for example, an analysis is created linked to an organism, and the user is an admin of the organism, this user will have admin permissions for that analysis as well.</p>',
  ];


  $user = user_load($uid);

  $form['uid'] = ['#type' => 'value', '#value' => $uid];

  $header = ['Field', 'Value'];

  $rows = [['User Name', $user->name], ['User Email', $user->mail]];

  $table = theme('table', ['header' => $header, 'rows' => $rows]);
  $form['user_table'] = ['#markup' => $table];

  $o_options = [];
  $p_options = [];

  $orgs = db_select('chado.organism', 'o')->fields('o', [
    'organism_id',
    'abbreviation',
  ])->execute();

  foreach ($orgs as $organism) {
    $o_options[$organism->organism_id] = $organism->abbreviation;
  }


  $projects = db_select('chado.project', 'p')->fields('p', [
    'project_id',
    'name',
  ])->execute();

  foreach ($projects as $project) {
    $o_options[$project->project_id] = $project->name;
  }

  //Get pre-existing options

  $organism_records = db_select('tripal_hq_permissions', 't')->fields('t', ['record_id'])
    ->condition('uid', $uid)
    ->condition('base_table', 'organism')
    ->execute()
    ->fetchAll();

  $pre_org = [];
  foreach ($organism_records as $org){
    $org = $org->record_id;
    $pre_org[$org] = $org;
  }
  
  $project_records = db_select('tripal_hq_permissions', 't')->fields('t', ['record_id'])
    ->condition('uid', $uid)
    ->condition('base_table', 'project')
    ->execute()
    ->fetchAll();

  $pre_project = [];
  foreach ($project_records as $project){
    $pre_project[$project] = $project;
  }



  $form['organism'] = [
    '#type' => "select",
    '#title' => 'Organisms',
    '#multiple' => TRUE,
    '#options' => $o_options,
    '#default_value' => $pre_org
  ];


  $form['project'] = [
    '#type' => "select",
    '#title' => 'Projects',
    '#multiple' => TRUE,
    '#options' => $p_options,
    '#default_value' => $pre_project
  ];


  if (empty($o_options)) {
    $form['organism'] = [
      '#title' => 'Organisms',
      '#type' => 'item',
      '#description' => "You have no Organisms in the database",
    ];

  }

  if (empty($p_options)) {
    $form['project'] = [
      '#title' => 'Projects',
      '#type' => 'item',
      '#description' => "You have no Projects in the database",
    ];
  }

  $form['other'] = [
    '#type' => "fieldset",
    '#title' => t('Other'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,

  ];

  $form['other'] = tripal_hq_permissions_build_other($form['other'], $form_state);

  $form['submit'] = ['#type' => 'submit', '#value' => "Submit"];

  return $form;

}

function tripal_hq_permissions_edit_admin_form_validate($form, &$form_state) {

}

function tripal_hq_permissions_edit_admin_form_submit($form, &$form_state) {
  $uid = $form_state['values']['uid'];
  $organisms = isset($form_state['values']['organism']) ? $form_state['values']['organism'] : NULL;
  $projects = isset($form_state['values']['project']) ? isset($form_state['values']['project']) : NULL;


  //clear user permissions and inferred permissions

  $perms = db_select('tripal_hq_permissions', 't')
    ->fields('t', ['id'])
    ->condition('uid', $uid)
    ->execute()
    ->fetchAll();
  $perm_array = [];
  foreach ($perms as $perm) {
    $perm_array[] = $perm->id;

  }

  if (!empty($perm_array)) {

    db_delete('tripal_hq_submission_permissions')
      ->condition('hq_permission_id', $perm_array, 'IN');
  }

  db_delete('tripal_hq_permissions')
    ->condition('uid', $uid)->execute();


  //TODO: other modules extending this permission will need to hook into this.


  if ($organisms) {
    foreach ($organisms as $organism => $label) {

      db_insert('tripal_hq_permissions')
        ->fields([
          'uid' => $uid,
          'base_table' => 'organism',
          'record_id' => $organism,
        ])->execute();
    }
  }

  if ($projects) {
    foreach ($projects as $project => $label) {

      db_insert('tripal_hq_permissions')
        ->fields([
          'uid' => $uid,
          'base_table' => 'project',
          'record_id' => $project,
        ]);

    }
  }

  //Now, submit job to infer all new permissions.
  $args = ['uid' => $uid];

  global $user;

  tripal_add_job('infer_permissions_user_' . $uid, 'tripal_hq_permissions', 'tripal_hq_permissions_infer_permissions', $args, $user->uid, 10, $includes = [], FALSE);
}


function tripal_hq_permissions_build_other($form, &$form_state) {

  $form['instructions'] = ['#markup' => '<p>This section will let you assign permissions to custom Chado records. Note this module only offers limited support for these associations.  Instead, consider grouping content into projects for easier permission management.</p>'];


  //get all base tables

  //remove organism and project

  //remove nonsensical tables??? which is most.

  return $form;
}

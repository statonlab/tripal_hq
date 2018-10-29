<?php

/**
 * @file used to set chado-specific permissions for a given user.
 *
 */

/**
 * Implements hook_form().
 */
function tripal_hq_permissions_manage_admins_form($form, &$form_state, $uid) {

  $form['instructions'] = [
    '#markup' => '
  <p>This form lets you set Chado specific administrative rights for this user.</p>
  <p>This module supports permissions for child content for organisms and projects, meaning if, for example, an analysis is created linked to an organism, and the user is an admin of the organism, this user will have admin permissions for that analysis as well.</p>',
  ];

  //load each entity as a collapsible field.
  //List organism and project first.


  $form['submit'] = ['#type' => 'submit', '#value' => 'Submit'];

  return $form;

}

function tripal_hq_permissions_manage_admins_form_validate() {

}

function tripal_hq_permissions_manage_admins_form_submit() {

}
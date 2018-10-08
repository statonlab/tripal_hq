<?php


function tripal_hq_admin_settings_form($form, &$form_state) {

  $form['instructions'] = ['#markup' => "Hi mom.  Site-wide settings here.  configure which emails go out."];

  return $form;

}

function tripal_hq_admin_settings_form_validate() {

}

function tripal_hq_admin_settings_form_submit() {

}



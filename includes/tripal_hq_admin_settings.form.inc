<?php


function tripal_hq_admin_settings_form($form, &$form_state) {

  ///Use previous form settings.  If new form, then use saved site variables.
  $prev_email = isset($form_state['values']['email']) ? $form_state['values']['email'] : tripal_hq_get_email_settings();

  $form['instructions'] = ['#markup' => "<p>The below form will let you configure site-wide preferences for this module.</p>"];

  $form['email'] = [
    '#type' => "fieldset",
    '#title' => t('Email Notification Settings'),
    '#tree' => TRUE,
  ];

  $form['email']['submit_request'] = [
    '#type' => "checkboxes",
    '#title' => t('Content Creation Request'),
    '#multiple' => TRUE,
    '#options' => ['User' => t('User'), 'Admin' => t('Admin')],
    '#description' => t('When a user submits a new content creation request, an email can be sent to confirm creation to the user, and to notify the admin an action is required.'),
    '#default_value' => $prev_email['submit_request'] ? $prev_email['submit_request'] : [],
  ];

//TODO: do we want to enable this?

//  $form['email']['edit_request'] = [
//    '#type' => 'checkboxes',
//    '#multiple' => TRUE,
//    '#title' => t('Content Creation Request Edit'),
//    '#options' => ['User' => t('User'), 'Admin' => t('Admin')],
//    '#description' => t('When a user edits an existing content creation request, an email can be sent to confirm creation to the user, and to notify the admin an action is required.'),
//    '#default_value' => $prev_email['edit_request'] ? $prev_email['edit_request'] : [],
//  ];


  $form['email']['approval'] = [
    '#type' => 'checkboxes',
    '#multiple' => TRUE,
    '#title' => t('Content Creation Request Approved'),
    '#options' => ['User' => t('User'), 'Admin' => t('Admin')],
    '#description' => t('When an admin approves a creation request, an email can be sent to notify the user and/or admin.'),
    '#default_value' => $prev_email['approval'] ? $prev_email['approval'] : [],

  ];


  $form['email']['denied'] = [
    '#type' => 'checkboxes',
    '#multiple' => TRUE,
    '#title' => t('Content Creation Request Denied'),
    '#options' => ['User' => t('User'), 'Admin' => t('Admin')],
    '#description' => t('When an admin rejects a creation request, an email can be sent to notify the user and/or admin.'),
    '#default_value' => $prev_email['denied'] ? $prev_email['denied'] : [],
  ];

  $form['submit'] = ['#type' => 'submit', '#value' => 'Submit'];

  return $form;

}

function tripal_hq_admin_settings_form_validate() {

}

/**
 * Implements hook_form_submit().
 *
 * @see tripal_hq_mail for mail keys.
 */
function tripal_hq_admin_settings_form_submit($form, &$form_state) {


  $values = $form_state['values'];
  $email_prefs = $values['email'];

  foreach ($email_prefs as $key => $values) {
    $global_key = 'tripal_hq_' . $key;
    variable_set($global_key, $values);
  }

  tripal_set_message('Tripal HQ module settings changed', TRIPAL_NOTICE);
}


/**
 * Get all set variables for tripal_hq.
 * As of now that is just email settings.
 *
 * @return array
 */
function tripal_hq_get_email_settings() {

  $keys = [
    "submit_request",
    "edit_request",
    "approval",
    "denied",
  ];

  $vals = [];

  foreach ($keys as $key) {

    $search_key = 'tripal_hq_' . $key;

    $value = variable_get($search_key);
    $vals[$key] = $value;
  }
  return $vals;
}

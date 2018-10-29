<?php

/**
 * Bundle list page.
 *
 * @return array
 */
function tripal_hq_list_bundles_page() {
  $page = [];

  // TODO: add description for the page. We might also add a warning
  // TODO: for *admin* users to use the actual tripal content forms
  $page['description'] = [
    '#type' => 'markup',
    '#prefix' => '<p>',
    '#markup' => t('Page description with instructions will go here'),
    '#suffix' => '</p>',
  ];

  // Get bundles sorted by label
  $bundles = db_select('tripal_bundle', 'tb')
    ->fields('tb')
    ->orderBy('label', 'asc')
    ->execute()
    ->fetchAll();

  foreach ($bundles as $bundle) {
    if (!user_access("propose $bundle->name")) {
      continue;
    }

    // Load the bundle to get the description (aka term definition)
    $bundle = tripal_load_bundle_entity(['id' => $bundle->id]);
    if (!$bundle) {
      continue;
    }

    $link = l($bundle->label, "tripal_hq/bio_data/add/$bundle->id");
    $page['bundle_' . $bundle->id] = [
      '#type' => 'item',
      '#markup' => $link,
      '#description' => $bundle->term->definition,
    ];
  }

  return $page;
}

/**
 * Create entity form.
 *
 * @param array $form
 * @param array $form_state
 * @param int $bundle Bundle ID
 * @param int $sid Submission ID
 *
 * @return array
 */
function tripal_hq_user_entity_form($bundle, $sid = NULL) {
  $data = NULL;
  $entity = NULL;

  // Construct an entity if a submission is provided
  if ($sid) {
    $submission = tripal_hq_get_submission_by_id($sid);
    if (empty($submission)) {
      drupal_not_found();
      return [];
    }
    $entity = unserialize($submission->data);
    $bundle = substr($entity->bundle, strlen('bio_data_'));

    if ($submission->status === 'approved') {
      drupal_not_found();
      return [];
    }
  }

  $fullBundle = tripal_load_bundle_entity(['id' => $bundle]);
  drupal_set_title($fullBundle->label);

  // Get the entity form
  $form = drupal_get_form('tripal_entity_form', $fullBundle->id, $entity);

  // Remove the original submit function and replace it with ours
  $index = array_search('tripal_entity_form_submit', $form['#submit']);
  if ($index !== FALSE) {
    unset($form['#submit'][$index]);
  }
  $form['#submit'][] = 'tripal_hq_entity_form_submit';

  // Attach the submission id to the form so that it's available
  // to the submit method
  if ($sid) {
    $form['submission_id'] = [
      '#type' => 'hidden',
      '#value' => $sid,
    ];
  }

  return $form;
}

/**
 * Alter the tripal entity form to add our submit function.
 *
 * @param array $form
 * @param array $form_state
 * @param string $form_id
 *
 * @return array
 */
function tripal_hq_form_tripal_entity_form_alter(&$form, &$form_state) {
  if (strstr($_GET['q'],
      'tripal_hq') !== FALSE || isset($form_state['tripal_hq_set'])) {
    // Remove the original submit function and replace it with ours
    $index = array_search('tripal_entity_form_submit', $form['#submit']);
    if ($index !== FALSE) {
      unset($form['#submit'][$index]);
    }
    $form_state['tripal_hq_set'] = TRUE;
    $form['#submit'][] = 'tripal_hq_entity_form_submit';
    $form['#validate'] = ['tripal_hq_entity_form_validate'];

    $sid = arg(4);
    if ($sid) {
      $form['submission_id'] = [
        '#type' => 'hidden',
        '#value' => $sid,
      ];
    }
  }

  return $form;
}

/**
 * Implements hook_validate() for the tripal_hq_user_entity_form.
 */
function tripal_hq_entity_form_validate($form, &$form_state) {
  // If the user is cancelling or deleting the entity then don't validate.
  if (array_key_exists('clicked_button',
      $form_state) and ($form_state['clicked_button']['#name'] == 'cancel_data' or $form_state['clicked_button']['#name'] == 'delete_data')) {
    return;
  }
  // For adds and updates, perform validation.
  $entity = $form_state['TripalEntity'];
  field_attach_form_validate('TripalEntity', $entity, $form, $form_state);

  $username = $form_state['values']['author_name'];
  $user = user_load_by_name($username);
  if (!$user) {
    form_set_error('author_name', 'Please provide a valid author name.');
  }

  try {
    $create_date = new DateTime($form_state['values']['author_date']);
  } catch (Exception $e) {
    form_set_error('author_date', 'Please provide a valid authored on date.');
  }
}

/**
 * Handle submission.
 *
 * @param $form
 * @param $form_state
 */
function tripal_hq_entity_form_submit($form, &$form_state) {
  // If the user is cancelling or deleting the entity then don't validate.
  if ($form_state['clicked_button']['#name'] == 'cancel_data') {
    $form_state['redirect'] = 'tripal_hq/bio_data';
    return;
  }

  if ($form_state['clicked_button']['#name'] == 'delete_data') {
    if (isset($form_state['values']['submission_id'])) {
      $sid = $form_state['values']['submission_id'];
      $submission = tripal_hq_get_submission_by_id($sid);
      tripal_hq_delete_submission($submission);
      drupal_set_message('Submission deleted successfully.');
      $form_state['redirect'] = 'tripal_hq/bio_data';
    }
    return;
  }

  $entity = $form_state['TripalEntity'];
  $username = isset($form_state['values']['author_name']) ? $form_state['values']['author_name'] : '';
  $user = user_load_by_name($username);
  $entity->uid = $user->uid;

  $entity->created = empty($entity->created) ? time() : $entity->created;

  $published = $form_state['values']['status'];
  $entity->status = $published;

  // Allow the fields to perform actions prior to submit.
  $instances = field_info_instances('TripalEntity', $entity->bundle);
  $langcode = 'und';
  foreach ($instances as $field_name => $instance) {
    $entity_type = $instance['entity_type'];
    if ($entity_type == 'TripalEntity' and array_key_exists($field_name,
        $form)) {
      foreach ($form[$field_name][$langcode] as $delta => $field_form) {
        if (!preg_match('/^\d+$/', $delta)) {
          continue;
        }

        $widget_type = $instance['widget']['type'];
        if (tripal_load_include_field_class($widget_type)) {
          $field = $field_form['#field'];
          $widget = new $widget_type($field, $instance);
          $widget->submit($form, $form_state, $entity_type, $entity, $langcode,
            $delta);
        }
      }
    }
  }

  $controller = entity_ui_controller('TripalEntity');
  $entity = $controller->entityFormSubmitBuildEntity($form, $form_state);
  $sid = isset($form_state['values']['submission_id']) ? $form_state['values']['submission_id'] : NULL;

  $sid = tripal_hq_save_submission($entity, $sid);

  if ($sid) {
    tripal_hq_send_emails($sid, 'submit');

    if (isset($form_state['values']['submission_id'])) {
      drupal_set_message('Submission created successfully. We will review 
                            your submission and get back to you shortly.');
    }
    else {
      drupal_set_message('Submission updated successfully. We will review
                            your submission and get back to you shortly.');
    }

    drupal_goto('tripal_hq/bio_data');
  }
  else {
    drupal_set_message('Unable to save your submission. Please contact us to fix the issue.',
      'error');
  }
}

/**
 * Save the fully constructed entity as a submission.
 *
 * @param $entity
 * @param null $submission_id
 *
 * @return string submission id, or FALSE if error
 */
function tripal_hq_save_submission($entity, $submission_id = NULL) {
  global $user;
  $submission = NULL;
  if ($submission_id) {
    $submission = tripal_hq_get_submission_by_id($submission_id);
  }

  $bundle = tripal_load_bundle_entity(['name' => $entity->bundle]);
  $title = tripal_get_title_format($bundle);
  $title = tripal_replace_entity_tokens($title, $entity, $bundle);
  $entity->title = $title;

  // A hack to remove empty fields since we don't need to delete anything
  // from storage yet.
  $fields = field_info_instances('TripalEntity', $entity->bundle);
  foreach ($fields as $field_name => $value) {
    if (isset($entity->{$field_name})) {
      if (!empty($entity->{$field_name}['und']) && is_array($entity->{$field_name}['und'])) {
        foreach ($entity->{$field_name}['und'] as $key => $value2) {
          if (empty($value2['value']) || $value2['value'] === 'delete_me') {
            unset($entity->{$field_name}['und'][$key]);
          }
        }
      }
    }
  }

  // If the submission is pending, update it
  if ($submission && $submission->status !== 'approved') {
    db_update('tripal_hq_submission')->condition('id', $submission_id)->fields([
      'uid' => $user->uid,
      'title' => $title,
      'status' => 'pending',
      'updated_at' => time(),
      'data' => serialize($entity),
    ])->execute();

    return $submission_id;
  }

  // Create a new submission
  $submission_id = db_insert('tripal_hq_submission')->fields([
    'uid' => $user->uid,
    'title' => $title,
    'status' => 'pending',
    'created_at' => time(),
    'data' => serialize($entity),
    'entity_id' => NULL,
    'bundle_id' => substr($entity->bundle, strlen('bio_data_')),
  ])->execute();

  if ($submission_id) {
    return $submission_id;
  }

  return FALSE;
}

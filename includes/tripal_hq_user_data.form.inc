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

  $bundles = tripal_get_content_types();
  foreach ($bundles as $bundle) {
    if (!user_access("propose $bundle->label content")) {
      continue;
    }

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
 */
function tripal_hq_user_entity_form(
  $form,
  &$form_state,
  $bundle_id,
  $entity_id = NULL
) {
  $form = tripal_entity_form($form, $form_state, $bundle_id);

  // Remove the original submit function and replace it with ours
  $index = array_search('tripal_entity_form_submit', $form['#submit']);
  if ($index !== FALSE) {
    unset($form['#submit'][$index]);
  }
  $form['#submit'][] = 'tripal_hq_entity_form_submit';

  return $form;
}

function tripal_hq_entity_form_submit($form, &$form_state) {
  $entity = $form_state['TripalEntity'];
  $username = $form_state['values']['author_name'];
  $user = user_load_by_name($username);
  $entity->uid = $user->uid;

  $create_date = $form_state['values']['author_date'];
  $entity->created = $create_date;

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

  $entity = entity_ui_controller('TripalEntity')->entityFormSubmitBuildEntity($form,
    $form_state);
  /** @var \TripalEntityController $controller */
  $bundle = tripal_load_bundle_entity(['name' => $entity->bundle]);
  $title = tripal_get_title_format($bundle);
  $title = tripal_replace_entity_tokens($title, $entity, $bundle);
  $entity->title = $title;
  $serialized = serialize($entity);

  global $user;
  db_insert('tripal_hq_submission')->fields([
    'uid' => $user->uid,
    'title' => $title,
    'status' => 'pending',
    'timestamp' => time(),
    'data' => $serialized,
    'entity_id' => null
  ])->execute();

  drupal_set_message('Submission created successfully. We will review your submission and get back to you shortly.');

  drupal_goto('tripal_hq/submission_dash');
}

<?php

/**
 * Bundle list page.
 *
 * @return array
 */
function tripal_hq_list_bundles_page() {
  $page = [];

  // TODO: add description for the page
  $page['description'] = [
    '#type' => 'markup',
    '#prefix' => '<p>',
    '#markup' => t('Page description with instructions will go here'),
    '#suffix' => '</p>',
  ];

  $bundles = tripal_get_content_types();
  foreach ($bundles as $bundle) {
    if (!user_access("propose $bundle->name")) {
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
function tripal_hq_user_create_entity_form($form, &$form_state) {
  //$form = tripal_add_form
}

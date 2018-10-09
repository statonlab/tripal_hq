<?php

class TripalHQEntityController extends TripalEntityController{

  public function save($entity, $cache = []) {
    global $user;
    $pkeys = [];

    if (!isset($cache['clear_cached_fields'])) {
      $cache['clear_cached_fields'] = TRUE;
    }

    $changed_date = time();
    $create_date = $changed_date;
    if (property_exists($entity, 'created')) {
      if (!is_numeric($entity->created)) {
        $temp = new DateTime($entity->created);
        $create_date = $temp->getTimestamp();
      }
    }

    $status = 1;
    if (property_exists($entity, 'status')) {
      if ($entity->status === 0 or $entity->status === 1) {
        $status = $entity->status;
      }
    }

    $transaction = db_transaction();
    try {
      // If our entity has no id, then we need to give it a
      // time of creation.
      if (empty($entity->id)) {
        $entity->created = $create_date;
        $invocation = 'entity_insert';
      }
      else {
        $invocation = 'entity_update';
        $pkeys = ['id'];
      }
      if (property_exists($entity, 'publish') and $entity->publish == TRUE) {
        $invocation = 'entity_publish';
      }

      // Invoke hook_entity_presave().
      module_invoke_all('entity_presave', $entity, $entity->type);

      // Write out the entity record.
      $record = [
        'term_id' => $entity->term_id,
        'type' => $entity->type,
        'bundle' => $entity->bundle,
        'title' => $entity->title,
        'uid' => $entity->uid,
        'created' => $create_date,
        'changed' => $changed_date,
        'status' => $status,
      ];
      if (property_exists($entity, 'nid') and $entity->nid) {
        $record['nid'] = $entity->nid;
      }
      if ($invocation == 'entity_update') {
        $record['id'] = $entity->id;
      }

      // Now we need to either insert or update the fields which are
      // attached to this entity. We use the same primary_keys logic
      // to determine whether to update or insert, and which hook we
      // need to invoke.  We do not attach fields when publishing an entity.
      // This is because a field may have default values and if so, those fields
      // will be attached and the storage backend may then try to insert
      // fields which should not be inserted because they already exist.
      if ($invocation == 'entity_insert') {
        field_attach_insert('TripalEntity', $entity);
      }
      if ($invocation == 'entity_update') {
        field_attach_update('TripalEntity', $entity);
      }

      // Set the title for this entity.
      $this->setTitle($entity, NULL, $cache);

      // Set the path/url alias for this entity.
      $this->setAlias($entity, NULL, $cache);

      // Clear any cache entries for this entity so it can be reloaded using
      // the values that were just saved.
      // Also, we don't need to clear cached fields when publishing because we
      // didn't attach any (see above).
      if ($cache['clear_cached_fields'] AND ($invocation != 'entity_publish')) {
        $cid = 'field:TripalEntity:' . $entity->id;
        cache_clear_all($cid, 'cache_field', TRUE);
      }

      return $entity;
    } catch (Exception $e) {
      $transaction->rollback();
      watchdog_exception('tripal', $e);
      drupal_set_message("Could not save the TripalEntity: " . $e->getMessage(),
        "error");
      return FALSE;
    }
  }
}

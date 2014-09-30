<?php

/**
 * Implements hook_drush_load().
 *
 * In a drush contrib check if the frontend part (hosting_hook variant) is enabled.
 */
function local_sync_tasks_drush_load() {
  $features = drush_get_option('hosting_features', array());

  return array_key_exists('local_sync_tasks', $features) && $features['local_sync_tasks'];
}

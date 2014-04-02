<?php

/**
 * Implements hook_drush_load().
 *
 * In a drush contrib check if the frontend part (hosting_hook variant) is enabled.
 */
function remote_files_drush_load() {
  $features = drush_get_option('hosting_features', array());

  return array_key_exists('remote_files', $features) && $features['remote_files'];
}

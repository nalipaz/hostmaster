<?php
/**
 * Implements hook_drush_load().
 *
 * In a drush contrib check if the frontend part (hosting_hook variant) is enabled.
 */
function symlink_directories_drush_load() {
  $features = drush_get_option('hosting_features', array());

  return array_key_exists('symlink_directories', $features) && $features['symlink_directories'];
}

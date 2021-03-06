<?php
/**
 * @file
 *   Expose the hosting sync feature to hostmaster.
 */

/**
 * Implementation of hook_hosting_feature().
 */
function hosting_sync_hosting_feature() {
  $features['sync'] = array(
    'title' => t('Sync task'),
    'description' => t("Tasks that synchronizing sites."),
    'status' => HOSTING_FEATURE_ENABLED,
    'module' => 'hosting_sync',
    );
  return $features;
}


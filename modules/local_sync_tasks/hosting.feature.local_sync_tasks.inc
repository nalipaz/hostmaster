<?php
/**
 * @file
 * Make this module a Hosting feature in Aegir.
 * 
 * @author Nicholas Alipaz - http://nicholas.alipaz.net/ (email through contact form)
 */

/**
 * Implements hook_hosting_feature().
 */
function hosting_local_sync_tasks_hosting_feature() {
  $features['local_sync_tasks'] = array(
    'title' => t('Local sync tasks'),
    'description' => t('Local sync stuff.'),
    'status' => HOSTING_FEATURE_DISABLED,
    'module' => 'hosting_local_sync_tasks',
    'group' => 'experimental'
  );

  return $features;
}

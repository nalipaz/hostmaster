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
function hosting_symlink_directories_hosting_feature() {
  $features['symlink_directories'] = array(
    'title' => t('Symlink Directories'),
    'description' => t('Symlink stuff.'),
    'status' => HOSTING_FEATURE_DISABLED,
    'module' => 'hosting_symlink_directories',
    'group' => 'experimental'
  );

  return $features;
}

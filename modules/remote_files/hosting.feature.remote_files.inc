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
function hosting_remote_files_hosting_feature() {
  $features['remote_files'] = array(
    'title' => t('Remote Files'),
    'description' => t('Allows setting a site to use a remote set of files for the files directory.'),
    'status' => HOSTING_FEATURE_DISABLED,
    'module' => 'hosting_remote_files',
    'group' => 'experimental'
  );

  return $features;
}

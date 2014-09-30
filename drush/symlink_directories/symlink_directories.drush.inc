<?php
/**
 * @file
 * 
 * @author Nicholas Alipaz - http://nicholas.alipaz.net/ (email through contact form)
 */

/**
 * Implements hook_drush_init().
 */
function symlink_directories_drush_init() {
  symlink_directories_provision_register_autoload();
}

/**
 * Register our directory as a place to find provision classes.
 */
function symlink_directories_provision_register_autoload() {
  static $loaded = FALSE;
  if (!$loaded) {
    $loaded = TRUE;
    provision_autoload_register_prefix('Provision_', dirname(__FILE__));
  }
}

/**
 * Expose the service type this extension defines to provision.
 *
 * @return
 *   An array with the service type the key, and the default implementation the value.
 */
function symlink_directories_provision_services() {
  symlink_directories_provision_register_autoload();

  return array('symlink_directories' => NULL);
}

function symlink_directories_create_symlinks($data, $path, $symlink_directory_base, $symlink_subdirectories) {
  $symlink_subdirectories = explode(',', $symlink_subdirectories);

  foreach ($symlink_subdirectories as $subdir) {
    $target = $symlink_directory_base . '/' . $subdir;
    $link = $path . '/' . $subdir;
    $cmd = sprintf('rm -rf %s',
      escapeshellarg($link)
    );
    if ($data['server']->shell_exec($cmd)) {
      drush_log(dt("Removed any existing links or directories at %link", array(
        '%link' => $link,
      )));
    }
    $cmd = sprintf('ln -sf %s %s',
      escapeshellarg($target),
      escapeshellarg($link)
    );
    if ($data['server']->shell_exec($cmd)) {
      drush_log(dt("Created symlink for %link at %target", array(
        '%link' => $link,
        '%target' => $target,
      )));
    }
  }
}

/**
 * Implementation of drush_hook_pre_provision_install()
 */
function drush_symlink_directories_pre_provision_install($url = NULL) {
  drush_set_option('symlink_directory_base', drush_get_option('symlink_directory_base', NULL));
  drush_set_option('symlink_subdirectories', drush_get_option('symlink_subdirectories', NULL));
}

function symlink_directories_provision_apache_dir_config($data) {
  $symlink_directory_base = drush_get_option('symlink_directory_base', NULL);
  $symlink_subdirectories = drush_get_option('symlink_subdirectories', NULL);
  
  if ($symlink_directory_base && $symlink_subdirectories) {
    $platform_path = drush_get_context('DRUSH_DRUPAL_ROOT') . '/sites/all';
    symlink_directories_create_symlinks($data, $platform_path, $symlink_directory_base, $symlink_subdirectories);
  }
}

/**
 * Implementation of hook_provision_apache_vhost_config()
 */ 
function symlink_directories_provision_apache_vhost_config($uri, $data) {
  $symlink_directory_base = drush_get_option('symlink_directory_base', NULL);
  $symlink_subdirectories = drush_get_option('symlink_subdirectories', NULL);
  
  if ($symlink_directory_base && $symlink_subdirectories) {
    $site_path = drush_get_option('site_path', NULL, 'alias');
    symlink_directories_create_symlinks($data, $site_path, $symlink_directory_base, $symlink_subdirectories);
  }
}

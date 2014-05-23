<?php
/**
 * @file
 * Drush file to create remote file to local synchronization.
 * 
 * To better deal with needing to use assets, rather than having to regularly
 *   grab a tarball and replace content locally I use sshfs fuse mounts to 
 *   mount directory locally.  However, we also have to consider that some 
 *   directories should always load from the local machine, css, js, etc.  This
 *   drush file will deal with all this when aegir runs a site verification, 
 *   adding in Apache rules and needed directories.
 * @author Nicholas Alipaz - http://nicholas.alipaz.net/ (email through contact form)
 */

/**
 * Implements hook_drush_init().
 */
function remote_files_drush_init() {
  remote_files_provision_register_autoload();
}

/**
 * Register our directory as a place to find provision classes.
 */
function remote_files_provision_register_autoload() {
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
function remote_files_provision_services() {
  remote_files_provision_register_autoload();
  return array('remote_files' => NULL);
}

/*
 * Implements hook_provision_apache_vhost_config()
 */
function remote_files_provision_apache_vhost_config($uri, $data) {
  $lines = array();
  $status = d()->remote_files_status;
  $remote_uri = trim(d()->remote_files_uri, '/') . '/';

  if ($status === '1') {
    // Add the needed lines into the vhost.
    $lines[] = '<IfModule mod_rewrite.c>';
    $lines[] = '  RewriteEngine on';
    $lines[] = '  RewriteRule ^/sites/' . $uri . '/files/((?!css/|ctools/|imagecache/|js/|tmp/).*)$ ' . $remote_uri . '$1 [R=301,NC,L]';
    $lines[] = '  RewriteRule ^/sites/default/files/((?!css/|ctools/|imagecache/|js/|tmp/).*)$ ' . $remote_uri . '$1 [R=301,NC,L]';
    $lines[] = '</IfModule>';
    $lines[] = '';
  }

  return implode("\n", $lines);
}

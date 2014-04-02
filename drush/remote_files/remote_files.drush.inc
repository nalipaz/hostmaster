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

function _remote_files_create_dir($uri, $data) {
	$site_path = d()->site_path;
	$remote_dir = $site_path . '/remote-files';

	provision_file()->create_dir($remote_dir, dt('Remote Files'), 02775);

	return $remote_dir;
}

/*
 * Implements hook_provision_apache_vhost_config()
 */
function remote_files_provision_apache_vhost_config($uri, $data) {
	$site_path = d()->site_path;
	$path = _remote_files_create_dir($uri, $data);
	$lines = array();
	$status = d()->remote_files_status;
	$username = d()->remote_files_username;
	$host = d()->remote_files_host;
	$remote_dir = rtrim(d()->remote_files_remote_dir, '/');
	$local_dir = $site_path . '/remote-files';
	$command = 'sshfs ' . $username . '@' . $host . ':' . $remote_dir . ' ' . $local_dir . ' -o allow_other';

  if ($status === '1') {
		// Add the needed lines into the vhost.
		$lines[] = '<IfModule mod_rewrite.c>';
		$lines[] = '  RewriteEngine on';
		$lines[] = '  RewriteRule ^/sites/' . $uri . '/files/((?!css/|ctools/|imagecache/|js/|tmp/).*)$ /sites/$uri/remote-files/$1 [L]';
		$lines[] = '</IfModule>';
		$lines[] = '';
	}

	drush_shell_cd_and_exec($site_path, $command);

  return implode("\n", $lines);
}

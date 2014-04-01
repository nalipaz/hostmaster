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
 * Implements hook_drush_command().
 * 
 * @see http://drupalcontrib.org/api/drupal/contributions!drush!docs!drush.api.php/function/hook_drush_command/7
 */
function remote_files_drush_command() {
	$items['provision-remote_files-add_dir'] = array(
    'description' => 'Create the directory for the remote files to be mounted on.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
		'aliases' => array(
			'rfdc',
		),
  );
}

/**
 * Callback for the `provision-remote_files-add_dir` drush command.
 */
function drush_remote_files_provision_remote_files_add_dir() {
}

function _remote_files_create_dir($uri, $data) {
	$site_path = d()->site_path;
	$remote_files_dir = $site_path . '/remote-files';

	provision_file()->create_dir($remote_files_dir, dt('Remote Files'), 0775);

	return $remote_files_dir;
}

/*
 * Implements hook_provision_apache_vhost_config()
 */
function http_basic_auth_provision_apache_vhost_config($uri, $data) {
	$path = _remote_files_create_dir($uri, $data);

	// Add the needed lines into the vhost.
	$lines = <<<EOF
<IfModule mod_rewrite.c>
  RewriteEngine on
  RewriteRule ^/sites/$uri/files/((?!css/|ctools/|imagecache/|js/|tmp/).*)$ /sites/$uri/remote-files/$1 [L]
</IfModule>
EOF;

  return $lines;
}

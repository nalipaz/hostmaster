<?php
/**
 * @file
 * 
 * @author Nicholas Alipaz - http://nicholas.alipaz.net/ (email through contact form)
 */

/**
 * Implements hook_drush_init().
 */
function local_sync_tasks_drush_init() {
  local_sync_tasks_provision_register_autoload();
}

/**
 * Register our directory as a place to find provision classes.
 */
function local_sync_tasks_provision_register_autoload() {
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
function local_sync_tasks_provision_services() {
  local_sync_tasks_provision_register_autoload();

  return array('local_sync_tasks' => NULL);
}

/**
 * Implements hook_drush_help_alter().
 */
function local_sync_tasks_drush_help_alter(&$command) {
  if ($command['command'] == 'sql-sync') {
    $command['options']['registry-rebuild'] = "If set runs registry rebuild command (must have registry-rebuild in your drush source).";
//    $command['options']['registry-rebuild'] = "If set runs registry rebuild command (must have registry-rebuild in your drush source).";
    $command['options']['modules'] = "Enable/disable the specified modules in the target database after the sync operation has been completed. Should be specified in alias file as a keyed array of modules to enable and disable.\n```\n\$aliases['sitealias'] = array(\n  'target-command-specific' => array(\n    'sql-sync' => array(\n      'modules' => array(\n        'enable' => array('stage_file_proxy', 'syslog'),\n        'disable' => array('cdn', 'help', 'update', 'simpletest', 'dblog', 'memcache', 'memcache_admin'),\n      ),\n    ),\n  ),\n);\n```";
    $command['options']['variables'] = "Alter the specified variables in the target database after the sync operation has been completed. Should be specified in alias file as a keyed array of variables to set and del.\n```\n\$aliases['sitealias'] = array(\n  'target-command-specific' => array(\n    'sql-sync' => array(\n      'variables' => array(\n        'set' => array(\n          'stage_file_proxy_origin' => 'http://example.com'\n        ),\n        'del' => array(\n          'admin_menu_cache_client',\n        ),\n      ),\n    ),\n  ),\n);\n```";
  }
}

/**
 * Implements hook_post_sql_sync().
 */
function drush_local_sync_tasks_post_sql_sync($source = NULL, $destination = NULL) {
  // Registry rebuild task.
  $registry_rebuild = drush_get_option('registry-rebuild');

  if ($registry_rebuild === TRUE) {
    drush_log(dt('Running post-sql-sync registry rebuild'), 'ok');
    drush_invoke_process($destination, 'registry-rebuild', array(), array('yes' => TRUE));
  }

  // Module Tasks.
  $module_tasks = drush_get_option('modules');

  if (!empty($module_tasks)) {
    // Make disable always run first.
    ksort($module_tasks);

    foreach ($module_tasks as $task => $module_list) {
      switch ($task) {
        case 'enable':
        case 'disable':
          $function = 'pm-' . $task;
          $module_string = (count($module_list) > 1) ? dt('modules') : dt('module');
          $args = array('!title' => ucfirst($task), '!modules' => implode(', ', $module_list), '!module_string' => $module_string);
          $message = dt('!title !modules !module_string post-sql-sync', $args);

          drush_log($message, 'ok');
          drush_invoke_process($destination, $function, $module_list, array('yes' => TRUE));
          break;
      }
    }
  }

  // Variable Changes.
  $variable_tasks = drush_get_option('variables');

  if (!empty($variable_tasks)) {
    // Make deletions always run first.
    ksort($variable_tasks);

    foreach ($variable_tasks as $task => $variables) {
      $function = 'v' . $task;

      switch ($task) {
        case 'set':
          foreach ($variables as $variable => $value) {
            drush_log(dt("Setting variable '!variable' to '!value'", array('!variable' => $variable, '!value' => $value)), 'ok'); 
            drush_invoke_process($destination, $function, array($variable, $value), array('yes' => TRUE));
          }
          break;

        case 'del':
          foreach ($variables as $key => $variable) {
            // Make sure there is a valid variable name.
            $variable = (empty($variable)) ? $key : $variable;
            drush_log(dt("Deleting variable '!variable'", array('!variable' => $variable)), 'ok'); 
            drush_invoke_process($destination, $function, array($variable), array('yes' => TRUE));
          }
      }
    }
  }

//  // Features Revert All task.
//  $features_revert = drush_get_option('features-revert-all');
//
//  if ($features_revert_all === TRUE) {
//    drush_log(dt('Running post-sql-sync features revert all'), 'ok');
//    drush_invoke_process($destination, 'features-revert-all', array(), array('yes' => TRUE));
//  }
}
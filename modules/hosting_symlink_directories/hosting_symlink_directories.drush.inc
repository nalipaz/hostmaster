<?php
/**
 * Implementation of drush_hook_pre_hosting_task()
 * Send the site's php_version attribute to the backend for processing.
 */

/**
 * Implementation of hook_hosting_site_context_options().
 */
function drush_hosting_symlink_directories_pre_hosting_task() {
  $task =& drush_get_context('HOSTING_TASK');

  $task->options['symlink_directory_base'] = (!empty($task->ref->symlink_directory_base)) ? $task->ref->symlink_directory_base : 'null';
  $task->options['symlink_subdirectories'] = (!empty($task->ref->symlink_subdirectories)) ? $task->ref->symlink_subdirectories : 'null';
}

/**
 * Implementation of hook_drush_context_import().
 */
//function hosting_symlink_directories_drush_context_import($context, &$node) {
//  if ($context->type === 'site') {
//    $node->symlink_directory_base = (!empty($context->symlink_directory_base)) ? $context->symlink_directory_base : NULL;
//    $node->symlink_subdirectories = (!empty($context->symlink_subdirectories)) ? $context->symlink_subdirectories : null;
//  }
//}

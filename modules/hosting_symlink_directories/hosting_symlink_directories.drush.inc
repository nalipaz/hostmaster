<?php
/**
 * Implementation of drush_hook_pre_hosting_task()
 * Send the site's php_version attribute to the backend for processing.
 */
//function drush_hosting_symlink_directories_pre_hosting_task() {
//  $task =& drush_get_context('HOSTING_TASK');
//  if (($task->ref->type == 'site' || $task->ref->type == 'platform') && ($task->task_type == 'install' || $task->task_type == 'verify')) {
//    $task->options['symlink_directory_base'] = $task->ref->symlink_directory_base;
//    $task->options['symlink_subdirectories'] = $task->ref->symlink_subdirectories;
//  }
//}


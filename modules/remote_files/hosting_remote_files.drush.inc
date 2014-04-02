<?php
/**
 * @file
 * Main drush file to allow passing some variables to backend from Aegir frontend.
 * 
 * @author Nicholas Alipaz - http://nicholas.alipaz.net/ (email through contact form)
 */

/**
 * Implementation of hook_hosting_site_context_options().
 */
function hosting_remote_files_hosting_site_context_options(&$task) {
  if (!empty($task->ref->remote_files_status)) {
    $task->context_options['remote_files_status'] = $task->ref->remote_files_status;
    $task->context_options['remote_files_username'] = $task->ref->remote_files_username;
    $task->context_options['remote_files_host'] = $task->ref->remote_files_host;
    $task->context_options['remote_files_remote_dir'] = $task->ref->remote_files_remote_dir;
  }
  else {
    $task->context_options['remote_files_status'] = 'null';
    $task->context_options['remote_files_username'] = 'null';
    $task->context_options['remote_files_host'] = 'null';
    $task->context_options['remote_files_remote_dir'] = 'null';
  }
}

/**
 * Implementation of hook_drush_context_import().
 */
function hosting_remote_files_drush_context_import($context, &$node) {
  if ($context->type === 'site') {
    if (!empty($context->remote_files_status)) {
      $node->remote_files_status = $context->remote_files_status;
      $node->remote_files_username = $context->remote_files_username;
      $node->remote_files_host = $context->remote_files_host;
      $node->remote_files_remote_dir = $context->remote_files_remote_dir;
    }
  }
}

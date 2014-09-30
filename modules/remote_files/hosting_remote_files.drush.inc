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
    $task->context_options['remote_files_uri'] = $task->ref->remote_files_uri;
  }
  else {
    $task->context_options['remote_files_status'] = 'null';
    $task->context_options['remote_files_uri'] = 'null';
  }
}

/**
 * Implementation of hook_drush_context_import().
 */
function hosting_remote_files_drush_context_import($context, &$node) {
  if ($context->type === 'site') {
    if (!empty($context->remote_files_status)) {
      $node->remote_files_status = $context->remote_files_status;
      $node->remote_files_uri = $context->remote_files_uri;
    }
  }
}

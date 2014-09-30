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
function hosting_local_sync_tasks_hosting_site_context_options(&$task) {
  if (!empty($task->ref->local_sync_tasks_array)) {
    $task->context_options['target-command-specific'] = $task->ref->local_sync_tasks_array;
  }
  else {
    $task->context_options['target-command-specific'] = 'null';
  }
}

/**
 * Implementation of hook_drush_context_import().
 */
function hosting_local_sync_tasks_drush_context_import($context, &$node) {
  if ($context->type === 'site') {
    if (!empty($context->local_sync_tasks_array)) {
      $node->local_sync_tasks_array = $context->local_sync_tasks_array;
    }
  }
}

<?php

/**
 * The local_sync_tasks service base class.
 */
class Provision_Service_local_sync_tasks extends Provision_Service {
  public $service = 'local_sync_tasks';

  /**
   * Add the needed properties to the site context.
   */
  static function subscribe_site($context) {
    $context->setProperty('target-command-specific');
  }
}

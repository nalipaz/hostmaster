<?php

/**
 * The local_sync_tasks service base class.
 */
class Provision_Service_symlink_directories extends Provision_Service {
  public $service = 'symlink_directories';

  /**
   * Add the needed properties to the site context.
   */
  static function subscribe_site($context) {
    $context->setProperty('target-command-specific');
  }
}

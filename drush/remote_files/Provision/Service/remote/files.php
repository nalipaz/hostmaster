<?php

/**
 * The remote_files service base class.
 */
class Provision_Service_remote_files extends Provision_Service {
  public $service = 'remote_files';

  /**
   * Add the needed properties to the site context.
   */
  static function subscribe_site($context) {
    $context->setProperty('remote_files_status');
    $context->setProperty('remote_uri');
  }
}


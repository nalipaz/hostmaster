<?php

/**
 * @file hosting_sync.tasks.inc
 *
 * Implements a new Aegir task.
 */

/**
 * Implementation of hook_hosting_tasks()
 */
function hosting_sync_hosting_tasks() {
  $tasks = array();
  $tasks['site']['sync'] = array(
    'title' => t('Sync data'),
    'description' => t('Synchronize database and files from another site and (optionally) run update.php, clear cache, and revert features. <strong>Data on the current site will be DESTROYED!</strong>'),
    'access callback' => 'hosting_sync_hosting_task_menu_access',
    'dialog' => TRUE,
  );
  return $tasks;
}

/**
 * Access Callback for Aegir Tasks
 *
 * This function defines access to the various aegir tasks.
 *
 * @arg $node object
 *   The node object is running the task. (Site, Platform, Server)
 *
 * @arg $task string
 *   The type of the task that is running.
 *
 * @see hosting_task_menu_access()
 *
 */
function hosting_sync_hosting_task_menu_access($node, $task) {
  // If we are passed the nid by mistake
  if (!isset($node->nid)){
    $node = node_load($node);
  }

  if ($task == 'sync' && user_access('create sync task') && $node->type == 'site') {
    // If we are deleted or in the process of deleting, we can't sync!
    if (hosting_task_outstanding($node->nid, 'delete') || ($node->site_status == HOSTING_SITE_DELETED)) {
      return FALSE;
    }

    // Otherwise, only show if the site is enabled
    return $node->site_status == HOSTING_SITE_ENABLED;
  }

  return FALSE;
}

/**
 * Helper to determine if a particular site has a module installed.
 */
function _hosting_sync_site_has_module($node, $module) {
  $package = hosting_package_instance_load(array('rid' => $node->nid, 'p.short_name' => $module));
  return $package->status;
}

/**
 * Helper to get the list of sites.
 *
 * Similar to hosting_get_sites_by_status() but it calls db_rewrite_sql() so that the
 * node access rules are applied (adding support for the 'hosting_client' module).
 */
function _hosting_sync_get_sites($platform = NULL, $profile = NULL, $exclude_nid = NULL, $status = HOSTING_SITE_ENABLED) {
  $sql = "SELECT n.nid FROM {node} n LEFT JOIN {hosting_site} s ON n.vid = s.vid WHERE n.type = 'site' AND s.status = %d";
  $args = array($status);

  if (!is_null($platform)) {
    $sql .= " AND s.platform = %d";
    $args[] = $platform;
  }

  if (!is_null($profile)) {
    $sql .= " AND s.profile = %d";
    $args[] = $profile;
  }

  if (!is_null($exclude_nid)) {
    $sql .= " AND n.nid != %d";
    $args[] = $exclude_nid;
  }

  $result = db_query(db_rewrite_sql($sql), $args);

  $nodes = array();
  while ($nid = db_fetch_object($result)) {
    $nodes[$nid->nid] = node_load($nid->nid);
  }
  return $nodes;
}

/**
 * Helper to get a list of platforms that are compatible with the given site.
 *
 *  - $node is the site in question
 *  - $type is whether the $node is the 'source' or 'destination'
 */
function hosting_sync_get_compatible_platforms($node, $type = 'destination') {
  $profile_platform_instances = hosting_package_instances_load(
    array('r.type' => 'platform', 'n.nid' => $node->profile, 'h.status' => HOSTING_PLATFORM_ENABLED));

  $result = array();

  foreach ($profile_platform_instances as $profile_instance) {
    if ($type === 'source') {
      $status = hosting_package_comparison($node->nid, $profile_instance->iid);
    }
    else {
      $status = hosting_package_comparison($profile_instance->iid, $node->nid);
    }

    // If there are no errors, this one is compatible!
    if ($status['error'] == 0) {
      $result[] = array(
        'platform' => $profile_instance->rid,
        'profile' => $profile_instance->nid,
        'profile_instance' => $profile_instance->iid,
        'status' => $status,
      );
    }
  }

  drupal_alter('hosting_sync_compatible_platforms', $result, $node, $type);

  return $result;
}

/**
 * Helper to get the sites for a particular destination.
 */
function hosting_sync_get_compatible_sites($node, $type = 'destination') {
  $result = hosting_sync_get_compatible_platforms($node, $type);

  foreach ($result as $idx => &$info) {
    $sites = _hosting_sync_get_sites($info['platform'], $info['profile'], $node->nid);
    if (count($sites) > 0) {
      $info['sites'] = $sites;
    }
    else {
      unset($result[$idx]);
    }
  }

  drupal_alter('hosting_sync_compatible_sites', $result, $node, $type);

  return $result;
}

/**
 * Helper to create a set of radio buttons out of possible sources/destinations.
 */
function _hosting_sync_sites_elements($node, $result, $parents) {
  $elements = array();
  foreach ($result as $info) {
    if ($info['platform'] != $node->platform) {
      $status = $info['status'];
      $description = t("Upgrades: !upgrades, Warnings: !missing, Errors: !errors | <a href='!url' class='hosting-package-comparison-link'>Compare platforms</a>",
        array(
          '!upgrades' => $status['upgrade'],
          '!missing' => $status['missing'] + $status['downgrade'],
          '!errors' => $status['error'],
          '!url' => url('hosting/sync/compare/' . $node->nid . '/' . $info['profile_instance']))
        );
    }
    else {
      $description = t('Current platform');
    }

    $platform_node = node_load($info['platform']);
    $elements[$info['platform']] = array(
      '#type' => 'fieldset',
      '#title' => $platform_node->title,
      '#description' => $description,
      '#attributes' => array('class' => 'hosting-sync-platform-group'),
    );

    foreach ($info['sites'] as $site) {
      $elements[$info['platform']][$site->hosting_name] = array(
        '#type' => 'radio',
        '#title' => $site->title,
        '#parents' => $parents,
        '#return_value' => '@' . $site->hosting_name,
      );
    }
  }

  return $elements;
}

/**
 * Implementation of hook_hosting_task_TASK_TYPE_form().
 *
 * For "Sync Content" task.
 */
function hosting_task_sync_form($node) {
  $form = array();

  drupal_add_js(drupal_get_path('module', 'hosting_sync') . '/hosting_sync.js');

  // Get a list of compatible sources for this site as the destination
  $sites = hosting_sync_get_compatible_sites($node, 'destination');

  if (count($sites) == 0) {
    $form['source'] = array(
      '#type' => 'item',
      '#title' => t('Source'),
      '#value' => t('No sources are currently available.'),
    );
    return $form;
  }

  $form['source'] = array_merge(array(
    '#type' => 'radios',
    '#title' => t('Source'),
    '#required' => TRUE,
    '#description' => t('Choose the site to copy data and/or files from.')
  ), _hosting_sync_sites_elements($node, $sites, array('parameters', 'source')));

  $form['destination'] = array(
    '#type' => 'value',
    '#value' => '@' . $node->hosting_name,
  );

  // What to sync?
  $form['database'] = array(
    '#title' => t('Copy database from source.'),
    '#type' => 'checkbox',
    '#default_value' => 1,
    '#prefix' => '<fieldset><legend>' . t('What to sync?') . '</legend>',
  );
  $form['files'] = array(
    '#title' => t('Copy files from source.'),
    '#type' => 'checkbox',
    '#default_value' => 1,
  );
  $form['modules'] = array(
    '#title' => t('Copy modules from source.'),
    '#type' => 'checkbox',
    '#default_value' => 0,
  );
  $form['themes'] = array(
    '#title' => t('Copy themes from source.'),
    '#type' => 'checkbox',
    '#default_value' => 0,
  );
  $form['libraries'] = array(
    '#title' => t('Copy libraries from source.'),
    '#type' => 'checkbox',
    '#default_value' => 0,
    '#suffix' => '</fieldset>',
  );

  // Options
  $form['registry-rebuild'] = array(
    '#title' => t('Rebuild registry'),
    '#type' => 'checkbox',
    '#default_value' => 1,
  );
  $form['updatedb'] = array(
    '#title' => t('Run update.php'),
    '#type' => 'checkbox',
    '#default_value' => 1,
  );
  if (_hosting_sync_site_has_module($node, 'features')){
    $form['features_revert_all'] = array(
      '#title' => t('Revert all features'),
      '#type' => 'checkbox',
      '#default_value' => $has_features,
      '#access' => $has_features,
    );
  } else{
    // I think its better UI just to hide it? If they need features it will be enabled!
    //$form['actions']['revert'] = array(
    //  '#title' => t('<em>Revert all features on Destination after content sync?</em>'),
    //  '#type' => 'checkbox',
    //  '#description' => t('This site doesn\'t have features.module enabled. Please enable and then "Verify" the site.'),
    //  '#default_value' => FALSE,
    //  '#disabled' => TRUE,
    //  );
  }
  $form['cache_clear'] = array(
    '#title' => t('Clear cache'),
    '#type' => 'checkbox',
    '#default_value' => 1,
  );
  $form['backup'] = array(
    '#title' => t('Create backup'),
    '#type' => 'checkbox',
    '#description' => t('If this option is enabled, a backup of the destination will be created before any changes are made. <em>This is will allow you to restore later in case this was a mistake!</em>'),
    '#default_value' => 1,
  );

  $form['#validate'][] = 'hosting_task_sync_form_validate';
  return $form;
}

/**
 * Validation for hosting_task_hosting_sync_form()
 */
function hosting_task_sync_form_validate(&$form, &$form_state){
  // Can't sync to self
  if ($form_state['values']['parameters']['source'] == $form_state['values']['parameters']['destination']){
    form_set_error('destination', t('The source cannot be the same as the destination.'));
  }
}


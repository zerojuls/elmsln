<?php

/**
 * @file
 * Drush integration for APDQC module.
 */

/**
 * Implements hook_drush_command().
 */
function apdqc_drush_command() {
  $items['apdqc'] = array(
    'description' => dt('Apply the enhacements described on the project page'),
    'examples' => array(
      'drush apdqc' => dt('Takes site offline, runs the functions to process changes in the database, brings it back online.'),
    ),
  );
  return $items;
}

/**
 * Implements hook_drush_help().
 */
function apdqc_drush_help($section) {
  switch ($section) {
    case 'drush:apdqc':
      return dt('Run this to automatically apply the database performance optimizations.');
  }
}

/**
 * Drush command to all all the apdqc functions
 */
function drush_apdqc() {
  // take site offline
  variable_set('maintenance_mode', TRUE);
  // ensure we have the admin form in order to submit it
  module_load_include('inc', 'apdqc', 'apdqc.admin');
  // test for what version to run and if to apply semaphore modifications
  $results = db_query("SELECT VERSION()")->fetchAssoc();
  $version = reset($results);
  $results = db_query("SHOW TABLE STATUS WHERE Name = '{semaphore}'")->fetchAssoc();
  if (version_compare($version, '5.6.0', '<=')) {
    if (strcasecmp($results['Engine'], 'MEMORY') != 0) {
      $form_state = array();
      $form_state['values']['op'] = t('Convert semaphore table to MEMORY');
      drupal_form_submit('apdqc_admin_operations_form', $form_state);
    }
  }
  elseif (strcasecmp($results['Engine'], 'InnoDB') != 0) {
    $form_state = array();
    $form_state['values']['op'] = t('Convert semaphore table to InnoDB');
    drupal_form_submit('apdqc_admin_operations_form', $form_state);
  }

  // collation of tables
  $form_state = array();
  $form_state['values']['op'] = t('Convert cache tables collations');
  drupal_form_submit('apdqc_admin_operations_form', $form_state);

  // convert cache table indexes
  $form_state = array();
  $form_state['values']['op'] = t('Convert cache tables indexes');
  drupal_form_submit('apdqc_admin_operations_form', $form_state);

  // bring site back online
  variable_del('maintenance_mode');
}
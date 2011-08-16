<?php

/**
 * @file
 * Provides form processing code for module settings.
 */

/**
 * Builds the admin form for Intellitext settings.
 *
 * @ingroup forms
 */
function intellitxt_admin_settings_form($form, &$form_state) {

  $form['description'] = array(
    '#markup' => '<div>' . t('Please enter the variables from the Javascript snippet supplied by Vibrant') . '</div>',
  );

  // Because we have many fields with the same values, we have to set
  // #tree to be able to access them.
  $form['#tree'] = TRUE;

  $form['intellitxt_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => 'Enable Intellitxt',
    '#default_value' => variable_get('intellitxt_configured', FALSE),
  );

  $form['domain'] = array(
    '#type' => 'textfield',
    '#title' => t('Custom DNS'),
    '#default_value' => variable_get('intellitxt_domain', ''),
    '#required' => TRUE,
    '#description' => t('Domain string supplied from Vibrant. eg: mydrupalsite.uk.intellitxt.com'),
    '#maxlength' => 80,
  );

  // Multiple codes for each section.
  $form['sections'] = array(
    '#type' => 'fieldset',
    '#title' => t('Section settings'),
    '#prefix' => '<div id="sections-fieldset-wrapper">',
    '#suffix' => '</div>',
  );
  // Build the fieldset with the proper number of sections. We'll use
  // $form_state['num_sections'] to determine the number of textfields to build.
  $saved_sections = variable_get('intellitxt_sections', array());
  if (empty($form_state['num_sections'])) {
    // init
    $form_state['num_sections'] = (!empty($saved_sections)) ? count($saved_sections) : 1;
  }

  // Build the current number of text fields.
  for ($i = 0; $i < $form_state['num_sections']; $i++) {
    $default_section = (empty($saved_sections[$i])) ? '' : key($saved_sections[$i]);
    $form['sections']['path_code']['section_' . $i]['sec_' . $i] = array(
      '#prefix' => '<div class="section-path-code"><div class="path">',
      '#suffix' => '</div>',
      '#type' => 'textfield',
      '#title' => t('Path'),
      '#description' => t('Enter the path after @host/ do not include leading or trailing slash', array('@host' => $_SERVER['HTTP_HOST'])),
      '#default_value' => $default_section,
    );

    $default_ipid = (empty($saved_sections[$i])) ? '' : current($saved_sections[$i]);
    $form['sections']['path_code']['section_' . $i]['ipid_' . $i] = array(
      '#type' => 'textfield',
      '#title' => t('IPID'),
      '#prefix' => '<div class="ipid">',
      '#suffix' => '</div></div>',
      '#default_value' => $default_ipid,
    );
  }

  // Add more button.
  $form['sections']['add_section'] = array(
    '#type' => 'submit',
    '#value' => t('Add one more'),
    '#submit' => array('intellitxt_admin_settings_form_add_one'),
    // See the examples in ajax_example.module for more details on the
    // properties of #ajax.
    '#ajax' => array(
      'callback' => 'intellitxt_admin_settings_form_callback',
      'wrapper' => 'sections-fieldset-wrapper',
    ),
  );

  if ($form_state['num_sections'] > 1) {
    $form['sections']['remove_section'] = array(
      '#type' => 'submit',
      '#value' => t('Remove one'),
      '#submit' => array('intellitxt_admin_settings_form_remove_one'),
      '#ajax' => array(
        'callback' => 'intellitxt_admin_settings_form_callback',
        'wrapper' => 'sections-fieldset-wrapper',
      ),
    );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );

  $form['#attached']['css'][] = drupal_get_path('module', 'intellitxt') . '/intellitxt.css';

  return $form;
}

/**
 * Callback for both ajax-enabled buttons.
 *
 * This simply selects and returns the fieldset with the sections in it.
 */
function intellitxt_admin_settings_form_callback($form, $form_state) {
  return $form['sections'];
}

/**
 * Submit handler for the "add-one-more" button.
 *
 * It just increments the max counter and causes a rebuild.
 */
function intellitxt_admin_settings_form_add_one($form, &$form_state) {
  $form_state['num_sections']++;
  $form_state['rebuild'] = TRUE;
}

/**
 * Submit handler for the "remove one" button.
 *
 * Decrements the max counter and causes a form rebuild.
 */
function intellitxt_admin_settings_form_remove_one($form, &$form_state) {
  if ($form_state['num_sections'] > 1) {
    $form_state['num_sections']--;
  }
  $form_state['rebuild'] = TRUE;
}

/**
 * Validates the admin form for Intellitext settings.
 */
function intellitxt_admin_settings_form_validate($form, &$form_state) {

  if (preg_match('/\s/', $form_state['values']['domain'])) {
    form_set_error('domain', 'Custom DNS must not contain any whitespace');
  }

  if ($form_state['values']['intellitxt_enabled']) {
    // Check we have at least one path and ipid pair
    $sections_state = $form_state['values']['sections']['path_code'];
    for ($i = 0; $i < count($sections_state); $i++) {

      $path = $sections_state['section_' . $i]['sec_' . $i];
      $ipid = $sections_state['section_' . $i]['ipid_' . $i];

      // Check for missing values.
      if ($path && !$ipid) {
        form_set_error('sections][path_code][section_' . $i, 'Path is missing IPID code');
      }
      elseif (!$path && $ipid) {
        form_set_error('sections][path_code][section_' . $i, 'Please add a Path for IPID code');
      }

      // Validate values.
      if (preg_match('/\s/', $path)) {
        form_set_error('sections][path_code][section_' . $i . '][sec_' . $i, 'Path must not contain any whitespace');
      }

      if (preg_match('/\s/', $ipid) || ($ipid && !ctype_digit($ipid))) {
        form_set_error('sections][path_code][section_' . $i . '][ipid_' . $i, 'IPID must only contain numbers and no whitespace');
      }
    }
  }
}

/**
 * Saves Intellitext settings into variables.
 */
function intellitxt_admin_settings_form_submit($form, &$form_state) {
  $intellitxt_enabled = ($form_state['values']['intellitxt_enabled']) ? TRUE : FALSE;
  variable_set('intellitxt_configured', $intellitxt_enabled);
  variable_set('intellitxt_domain', $form_state['values']['domain']);
  $sections = array();
  $sections_state = $form_state['values']['sections']['path_code'];

  for ($i = 0; $i < count($sections_state); $i++) {
    $section_path = trim($sections_state['section_' . $i]['sec_' . $i], '/');
    $section_ipid = $sections_state['section_' . $i]['ipid_' . $i];
    if (!empty($section_path) && !empty($section_ipid)) {
      $sections[] = array($section_path => $section_ipid);
    }
  }

  if (!empty($sections)) {
    variable_set('intellitxt_sections', $sections);
  }
  else {
    variable_del('intellitxt_sections');
  }
  drupal_set_message(t('Configuration Saved'));
}

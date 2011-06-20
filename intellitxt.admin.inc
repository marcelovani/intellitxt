<?php

/**
 * @file
 * 
 */

/**
 * Builds the admin form for Intellitext settings
 * @ingroup forms
 */
function intellitxt_admin_settings_form($form, &$form_state) {

  $form['description'] = array(
    '#markup' => '<div>' . t('Please enter the variables from the Javascript snippet supplied by Vibrant') . '</div>',
  );	
	
  // Because we have many fields with the same values, we have to set
  // #tree to be able to access them.
  $form['#tree'] = TRUE;  
  
  $form['domain'] = array(
    '#type' => 'textfield',
    '#title' => t('Custom DNS'),
    '#default_value' => variable_get('intellitxt_domain', ''),
    '#required' => TRUE,
    '#description' => t('Domain string supplied from Vibrant. eg: mydrupalsite.uk.intellitxt.com'),
    '#maxlength' => 80,  
  );  
  
  $form['intellitext'] = array(
    '#type' => 'fieldset',
    '#title' => t('Site-wide settings (single code)'),
  );
 
  $form['intellitext']['ipid'] = array(
    '#type' => 'textfield',
    '#title' => t('IPID'),
    '#default_value' => variable_get('intellitxt_ipid', ''),
    //'#required' => TRUE,
    '#description' => t('Domain string supplied from Vibrant. This is found at the end of the script URL and consists only of numbers eg: 14078'),
    '#maxlength' => 8,  
  ); 
  
  //multiple codes for each section
  $form['sections'] = array(
    '#type' => 'fieldset',
    '#title' => t('Section settings (multiple codes)'),
    '#prefix' => '<div id="sections-fieldset-wrapper">',
    '#suffix' => '</div>',  
  );  
  // Build the fieldset with the proper number of sections. We'll use
  // $form_state['num_sections'] to determine the number of textfields to build.  
  $saved_sections = variable_get('intellitxt_sections', array());
  if (empty($form_state['num_sections'])) {
    //init
  	$form_state['num_sections'] = count($saved_sections);
  } 
  
  //build the current number of text fields
  for ($i = 0; $i < $form_state['num_sections']; $i++) {
  	$form['sections']['section']['sec_'.$i] = array(
      '#prefix' => '<div class="section-path-code"><div class="path">',
  	  '#suffix' => '</div>',
      '#type' => 'textfield',
      '#title' => t('Path'),
  	  '#description' => 'Enter the path after ' . $_SERVER['HTTP_HOST'] . '/ do not include leading or trailing slash',
  	  '#default_value' => key($saved_sections[$i]),
    );
    $form['sections']['section']['ipid_'.$i] = array(
      '#type' => 'textfield',
      '#title' => t('IPID'),
  	  '#prefix' => '<div class="ipid">',
      '#suffix' => '</div></div>',
  	  '#default_value' => current($saved_sections[$i]),
    );    
  }
  
  //add more button
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
 * Validates the admin form for Intellitext settings
 */
function intellitxt_admin_settings_form_validate($form, &$form_state) {
	
	if (preg_match('/\s/', $form_state['values']['domain'])) {
		form_set_error('domain','Custom DNS must not contain any whitespace');
	}
	
	if (preg_match('/\s/', $form_state['values']['intellitext']['ipid']) || !ctype_digit($form_state['values']['intellitext']['ipid'])) {
		form_set_error('ipid','IPID must only contain numbers and no whitespace');
	}

}

/**
 * Saves Intellitext settings into variables
 */
function intellitxt_admin_settings_form_submit($form, &$form_state) {
	
	variable_set('intellitxt_configured', true);
	variable_set('intellitxt_domain', $form_state['values']['domain']);
	if (isset($form_state['values']['intellitext']['ipid'])) {
		variable_set('intellitxt_ipid', $form_state['values']['intellitext']['ipid']);
	}
	$sections = array();
	$sections_state = $form_state['values']['sections']['section'];

	for ($i=0; $i < count($sections_state); $i++) {
	  if (!empty($sections_state['sec_' . $i]) && !empty($sections_state['ipid_' . $i])) {
	    $sections[$i] = array($sections_state['sec_' . $i] => $sections_state['ipid_' . $i]);
	  }
	}
	if (!empty($sections)) {
	  variable_set('intellitxt_sections', $sections);
	}
	drupal_set_message('Configuration Saved');
}



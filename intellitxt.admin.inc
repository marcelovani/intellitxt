<?php

/**
 * @file
 * 
 */

/**
 * Builds the admin form for Intellitext settings
 * @ingroup forms
 */
function intellitxt_admin_settings_form() {

  $form['intellitext'] = array(
    '#type' => 'fieldset',
    '#title' => t('Itellitxt Settings'),
  );
  
  $form['intellitext']['domain'] = array(
    '#type' => 'textfield',
    '#title' => t('Custom DNS'),
    '#default_value' => variable_get('intellitxt_domain', ''),
    '#required' => TRUE,
    '#description' => t('Domain string supplied from Vibrant. eg: mydrupalsite.uk.intellitxt.com'),
    '#maxlength' => 80,  
  );  
  
  $form['intellitext']['ipid'] = array(
    '#type' => 'textfield',
    '#title' => t('IPID'),
    '#default_value' => variable_get('intellitxt_ipid', ''),
    '#required' => TRUE,
    '#description' => t('Domain string supplied from Vibrant. This is found at the end of the script URL and consists only of numbers eg: 14078'),
    '#maxlength' => 8,  
  ); 
  
  $form['#submit'][] = 'intellitxt_admin_settings_form_submit';
  
  return system_settings_form($form);
}

/**
 * Validates the admin form for Intellitext settings
 */
function intellitxt_admin_settings_form_validate($form, &$form_state) {
	if (preg_match('/\s/', $form_state['values']['domain'])) {
		form_set_error('domain','Custom DNS must not contain any whitespace');
	}
	if (preg_match('/\s/', $form_state['values']['ipid']) || !ctype_digit($form_state['values']['ipid'])) {
		form_set_error('ipid','IPID must only contain numbers and no whitespace');
	}

}

/**
 * Saves Intellitext settings into variables
 */
function intellitxt_admin_settings_form_submit($form, &$form_state) {
	variable_set('intellitxt_configured', true);
	variable_set('intellitxt_domain', $form_state['values']['domain']);
	variable_set('intellitxt_ipid', $form_state['values']['ipid']);

}




<?php

class intellitxt_context_reaction_tag extends context_reaction {

  function options_form($context) {
    $options = array();
    $tags = variable_get('intellitxt_tags', array());
    foreach ($tags as $tag) {
      $options[key($tag)] = key($tag) . ' (' . current($tag) . ')';
    }
    asort($options);

    return array(
      '#title' => $this->title,
      '#description' => $this->description,
      '#options' => $options,
      '#type' => 'select',
      '#default_value' => $this->fetch_from_context($context),
    );
  }

  function execute() {
    $contexts = $this->get_contexts();
    foreach ($contexts as $context) {
      $machine_name = $context->reactions[$this->plugin];
      if ($machine_name) {
        intellitxt_add_tag($machine_name);
      }
    }
  }
}
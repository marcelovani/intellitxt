<?php

class intellitxt_context_reaction_tag extends context_reaction {

  function options_form($context) {
    $options = array();
    $tags = variable_get('intellitxt_tags', array());
    foreach ($tags as $tag) {
      $options[(int)current($tag)] = key($tag) . ' (' . current($tag) . ')';
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
      $ipid = $context->reactions[$this->plugin];
      if ($ipid) {
        intellitxt_add_tag($ipid);
      }
    }
  }
}
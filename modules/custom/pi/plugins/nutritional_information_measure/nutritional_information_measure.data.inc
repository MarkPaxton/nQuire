<?php

function _nutritional_information_measure_food_data() {

  return array(
      1 => array('title' => "Potato", 'portion' => "portion", 'value' => array(30, 3, 0, 2, 141, 12, 9, 1, 0, 1, 10, 0)),
      2 => array('title' => "Pasta", 'portion' => "portion", 'value' => array(45, 9, 2, 3, 86, 22, 52, 1, 0, 0, 0, 0)),
      3 => array('title' => "Rice", 'portion' => "portion", 'value' => array(56, 5, 2, 0, 122, 2, 32, 0, 0, 0, 0, 0)),
      4 => array('title' => "Bread - White", 'portion' => "slice", 'value' => array(13, 2, 1, 0, 10, 140, 30, 0, 0, 0, 0, 0)),
      5 => array('title' => "Bread - Brown", 'portion' => "slice", 'value' => array(13, 2, 1, 1, 12, 133, 56, 1, 0, 0, 0, 0)),
      6 => array('title' => "Meat - Red", 'portion' => "portion", 'value' => array(0, 12, 42, 0, 27, 28, 5, 1, 0, 2, 0, 0)),
      7 => array('title' => "Meat - White", 'portion' => "portion", 'value' => array(0, 22, 6, 0, 52, 64, 9, 1, 0, 0, 0, 0)),
      8 => array('title' => "Fish", 'portion' => "portion", 'value' => array(0, 26, 1, 0, 92, 408, 13, 0, 0, 3, 0, 0)),
      9 => array('title' => "Pulses (peas, beans, lentils)", 'portion' => "portion", 'value' => array(7, 3, 0, 1, 29, 5, 6, 1, 0, 0, 0, 0)),
      10 => array('title' => "Egg", 'portion' => "portion", 'value' => array(0, 7, 6, 0, 44, 83, 34, 1, 0, 1, 0, 1)),
      11 => array('title' => "Cheese", 'portion' => "portion", 'value' => array(15, 4, 10, 0, 20, 284, 60, 1, 0, 0, 0, 1)),
      12 => array('title' => "Yogurt", 'portion' => "portion", 'value' => array(17, 2, 1, 0, 0, 24, 49, 0, 0, 0, 44, 0)),
      13 => array('title' => "Milk", 'portion' => "glass", 'value' => array(14, 10, 5, 0, 269, 129, 360, 0, 0, 2, 3, 0)),
      14 => array('title' => "Chocolate", 'portion' => "bar", 'value' => array(34, 3, 15, 1, 0, 3, 18, 1, 0, 0, 0, 0)),
      15 => array('title' => "Biscuits", 'portion' => "portion", 'value' => array(42, 4, 15, 1, 2, 228, 54, 1, 0, 0, 0, 0)),
      16 => array('title' => "Cake", 'portion' => "slice", 'value' => array(33, 4, 16, 0, 10, 299, 64, 1, 0, 0, 0, 1)),
      17 => array('title' => "Crisps", 'portion' => "packet", 'value' => array(21, 2, 14, 2, 1, 320, 12, 1, 0, 0, 14, 0)),
      18 => array('title' => "Vegetables", 'portion' => "portion", 'value' => array(1, 1, 0, 1, 76, 5, 15, 0, 0, 0, 29, 0)),
      19 => array('title' => "Fruit", 'portion' => "portion", 'value' => array(12, 0, 0, 2, 85, 3, 4, 0, 0, 0, 6, 0)),
      20 => array('title' => "Cereals", 'portion' => "bowl", 'value' => array(45, 4, 1, 2, 1, 500, 2, 4, 0, 1, 0, 0)),
      21 => array('title' => "Ice Cream", 'portion' => "bowl", 'value' => array(24, 4, 12, 0, 75, 72, 120, 0, 109, 1, 1, 0)),
      22 => array('title' => "General Pudding", 'portion' => "bowl", 'value' => array(53, 3, 10, 2, 78, 99, 42, 1, 0, 0, 7, 0)),
  );
}

function _nutritional_information_measure_data_keys() {
  return array("Carb.", "Protein", "Fats", "Fibre", "Water", "Sodium", "Calcium", "Iron", "Vit. A", "Vit. B", "Vit. C", "Vit. D");
}

function _nutritional_information_measure_data_units() {
  return array("g", "g", "g", "g", "g", "mg", "mg", "mg", "mg", "mg", "mg", "mg");
}

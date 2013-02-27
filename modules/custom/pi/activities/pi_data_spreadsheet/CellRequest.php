<?php

class PiDataSpreadSheetCellRequest {

  private $cells;

  function __construct() {
    $this->cells = array();
  }

  function addCell($x, $y, $content) {
    $this->cells["{$x}_{$y}"] = array('x' => $x, 'y' => $y, 'c' => $content);
  }

  function callData() {
    $minX = $minY = PHP_INT_MAX;
    $maxX = $maxY = 0;

    foreach ($this->cells as $cell) {
      $minX = min($minX, $cell['x']);
      $minY = min($minY, $cell['y']);
      $maxX = max($maxX, $cell['x']);
      $maxY = max($maxY, $cell['y']);
    }

    $range = null;
    $size = 0;
    if ($minX > $maxX || $minY > $maxY) {
      return FALSE;
    } else if ($minX === $maxX && $minY === $maxY) {
      $range = $this->columnFormat($minX) . $this->rowFormat($minY);
      $width = 1;
      $size = 1;
    } else {
      $range = $this->columnFormat($minX) . $this->rowFormat($minY) . ':' . $this->columnFormat($maxX) . $this->rowFormat($maxY);
      $width = $maxX - $minX + 1;
      $size = $width * ($maxY - $minY + 1);
    }

    $xml_values = array();
    foreach ($this->cells as $cell) {
      $pos = ($cell['y'] - $minY) * $width + ($cell['x'] - $minX);
      $xml_values[$pos] = "<cell><input>{$cell['c']}</input></cell>";
    }

    $xml = '<list>';
    for ($i = 0; $i < $size; $i++) {
      $xml .= isset($xml_values[$i]) ? $xml_values[$i] : '<cell><input></input></cell>';
    }
    $xml .= '</list>';
    
    return array('range' => $range, 'cells' => $xml);
  }


  private function columnFormat($column) {
    $letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    $base = strlen($letters);
    $value = '';

    while (true) {
      $value = $letters[$column % $base] . $value;
      $column /= $base;
      //if ($column === 0) {
      break;
      //}
    }

    return $value;
  }

  private function rowFormat($row) {
    return $row + 1;
  }

}


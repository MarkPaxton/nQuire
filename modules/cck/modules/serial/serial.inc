<?php
// $Id: serial.inc,v 1.1.2.5 2009/10/24 16:43:45 kirsh Exp $
 
/**
 * @file
 * Internal functions for the Serial module.
 *
 * Note: This module uses php in SQL to support dynamic table names.
 * (required because each serial field needs a dedicated dynamic table).
 * However, all table names are safe (passed through db_escape_table).
 *
 * It seems that this is better than using table names as arguments, e.g.
 *   $query = 'INSERT INTO %s (nid) VALUES(%d)';
 *   db_query($query, db_prefix_tables('{'. $table .'}'), $nid);
 */
 
/**
 * Creates an assistant serial table for a new created field.
 *
 * @param $field a new serial field
 */
function _serial_create_table($field) {
  $table = _serial_get_table_name($field);
  $schema = _serial_get_table_schema();
  db_create_table($ret, $table, $schema);
}

/**
 * Drops an assistant serial table for a deleted field.
 *
 * @param $field a deleted serial field
 */
function _serial_drop_table($field) {
  $table = _serial_get_table_name($field);
  db_drop_table($ret, $table);
}

/**
 * Gets the name of the assistant table for a specific field.
 *
 * @param $field
 *   a serial field instance
 * @return
 *   the name of the assistant table of the specified field instance.
 */
function _serial_get_table_name($field) {
  return db_escape_table( // be on the safe side
    'serial_'. $field['type_name'] .'_'. $field['field_name']);
}

/**
 * Gets the schema of the assistant tables for generating serial values.
 *
 * @return
 *   the assistant table schema.
 */
function _serial_get_table_schema() {
  return array(
    'fields' => array(
      'sid' => array(
        'type' => 'serial',
        'unsigned' => TRUE,
        'not null' => TRUE,
        'description' => 'The atomic serial field.',
      ),
      'nid' => array(
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
        'description' => 'Id of the owner node.',
      ),
    ),
    'primary key' => array('sid'),
    'unique keys' => array(
      'nid' => array('nid'),
    ),
  );
}

/**
 * Generates a unique serial value (unique per node type).
 *
 * @param $nid
 *   id of the node for which to generate a serial value
 * @param $field
 *   the serial field
 *
 * @return
 *   the unique serial value number.
 */
function _serial_generate_value($nid, $field, $delete = TRUE) {
  // Insert a temporary record to get a new unique serial value:
  $table = _serial_get_table_name($field);
  $query = 'INSERT INTO {'. $table .'} (nid) VALUES(%d)';
  db_query($query, $nid);

  // Retrieve the new unique serial value:
  $query = 'SELECT sid FROM {'. $table .'} WHERE nid = %d';
  $result = db_result(db_query($query, $nid));
  $sid = $result ? $result : 0;

  // Delete old temporary records:
  if ($delete && ($sid % 10) == 0) {
    $query = 'DELETE FROM {'. $table .'} WHERE nid < %d';
    db_query($query, $nid);
  }

  // Return the new unique serial value:
  return $sid;
}

/**
 * Initializes the value of a new serial field in existing nodes.
 *
 * @return the number of existing nodes that have been initialized.
 */
function _serial_init_old_nodes($field) {
  // Retrieve all the node ids of that type:
  $query = "SELECT DISTINCT nid FROM {node} WHERE type = '%s' ORDER BY nid";
  $result = db_query($query, $field['type_name']);

  // Allocate a serial number for every old node:
  $count = 0;
  $field_name = $field['field_name'];
  while ($nid = db_result($result)) {
    $node = node_load($nid);
    $node->{$field_name}[0]['value'] = _serial_generate_value($nid, $field, FALSE);
    node_save($node);
    $last_nid = $nid;
    $count++;
  }

  // Delete temporary records (except the last):
  if (isset($last_nid)) {
    $serial_table = _serial_get_table_name($field);
    $query = "DELETE FROM {$serial_table} WHERE nid < %d";
    db_query($query, $last_nid);
  }

  // Return the number of existing nodes that have been initialized:
  return $count;
}
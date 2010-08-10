<?php

define( 'DEFAULT_FORMAT', 'csv' );
define( 'IMPORT_FILE_PATH', 'pi_import_export' );

function _pi_user_import_get_formats()
{
	$formats = array();
	
	$files = file_scan_directory( dirname( __FILE__ ), '.*', array( '.', '..', 'formats.inc'), 0, TRUE );
	
	if(is_array($files))
	{
		foreach($files as $file)
		{
			$formats[$file->name] = strtoupper($file->name);
		}
	}
	
	return $formats;
}

function _pi_user_import_get_files()
{
	$import_files = array();
	
	$import_path = file_directory_path() . '/' . IMPORT_FILE_PATH;
	
	$files = file_scan_directory( $import_path, '.*', array( '.', '..', 'formats.inc'), 0, TRUE );
	
	if(is_array($files))
	{
		foreach($files as $file)
		{
			$import_files[$file->basename] = $file->basename;
			
			//print_r($file);
		}
	}
	
	return $import_files;
}
	
?>
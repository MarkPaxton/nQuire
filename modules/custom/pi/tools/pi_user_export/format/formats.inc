<?php

define( 'DEFAULT_FORMAT', 'csv' );
define( 'EXPORT_FILE_PATH', 'pi_import_export' );

function _pi_user_export_get_formats()
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
	
?>
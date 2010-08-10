<?php

function import_users_from_file($filename)
{
	$users = array();
	$is_md5_password_import = FALSE;
	
	$filepath_users = file_directory_path() . '/' . IMPORT_FILE_PATH . '/' . $filename;

	if( file_exists($filepath_users) === FALSE )
	{
		$filepath_users = $_SERVER['DOCUMENT_ROOT'] . base_path() . $filepath_users;
	}
	
	$f_handle = fopen($filepath_users, 'r');

	if($f_handle !== FALSE)
	{
		while(($data = fgetcsv($f_handle, 1024, ",")) !== FALSE)
		{
			$obj = new stdClass;
			
			$obj->uid				= $data[0];
    		$obj->name				= $data[1];
    		$obj->pass				= $data[2];
    		$obj->mail				= $data[3];
    		$obj->mode				= $data[4];
    		$obj->sort				= $data[5];
    		$obj->threshold			= $data[6];
    		$obj->theme				= $data[7];
    		$obj->signature			= $data[8];
    		$obj->created			= $data[9];
    		$obj->access			= $data[10];
    		$obj->login				= $data[11];
    		$obj->status			= $data[12];
    		$obj->timezone			= $data[13];
    		$obj->language			= $data[14];
    		$obj->picture			= $data[15];
    		$obj->init				= $data[16];
    		$obj->data				= $data[17];
    		$obj->signature_format	= $data[18];
			
    		if( is_numeric( $obj->uid ) )
    		{
    			$users[] = $obj;
    		}
    		else if( $obj->uid === 'uid' )
    		{
    			if( substr_count($obj->pass, '(md5)') > 0 )
    			{
    				$is_md5_password_import = TRUE;
    			}
    		}
    		
			//print_r( $obj );
		}
		fclose($f_handle);
	}

	return array( 'users' => $users, 'is_md5' => $is_md5_password_import );
}

function import_og_from_file($filename)
{
	$groups = array();
	
	$filepath_users = file_directory_path() . '/' . IMPORT_FILE_PATH . '/' . $filename;

	if( file_exists($filepath_users) === FALSE )
	{
		$filepath_users = $_SERVER['DOCUMENT_ROOT'] . base_path() . $filepath_users;
	}
	
	$f_handle = fopen($filepath_users, 'r');

	if($f_handle !== FALSE)
	{
		while(($data = fgetcsv($f_handle, 1024, ",")) !== FALSE)
		{
			$obj = new stdClass;
			
			$obj->nid				= $data[0];
    		$obj->title				= $data[1];
    		$obj->status			= $data[2];
    		$obj->promote			= $data[3];
    		$obj->sticky			= $data[4];
    		
    		$obj->og_selective		= $data[5];
    		$obj->og_description	= $data[6];
    		$obj->og_theme			= $data[7];
    		$obj->og_register		= $data[8];
    		$obj->og_directory		= $data[9];
    		$obj->og_language		= $data[10];
    		$obj->og_private		= $data[11];
    		$obj->og_role			= $data[12];
    		$obj->is_active			= $data[13];
    		$obj->is_admin			= $data[14];
    		$obj->uid				= $data[15];
    					
    		if( is_numeric( $obj->nid ) )
    		{
    			$groups[] = $obj;
    		}    		
    		
			//print_r( $obj );
		}
		fclose($f_handle);
	}

	return $groups;
}

?>
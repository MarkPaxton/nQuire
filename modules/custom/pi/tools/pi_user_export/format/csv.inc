<?php

function export_users_to_file($filename, $users)
{
	$filepath_users = file_create_filename( $filename . '_user.csv', file_directory_temp());
	$filepath_groups = file_create_filename( $filename . '_groups.csv', file_directory_temp());
	
	$f_handle = fopen($filepath_users, 'w');

	if($f_handle)
	{
		$user_info = array( 'uid', 'user name', 'password:(md5)', 'email', 'mode', 'sort', 'threshold', 'theme', 'signature', 'created', 'access', 'login', 'status', 'timezone', 'language', 'picture', 'init', 'data', 'signature_format');
		fputcsv($f_handle, $user_info);
		
		$og[0] = array( 'og' => array( 'nid',  'title', 'status', 'promote', 'sticky', 'og_selective', 'og_description', 'og_theme', 'og_register', 'og_directory', 'og_language', 'og_private'), 
						'og_uid' => array( 'og_role', 'is_active', 'is_admin', 'uid' ) );
		export_groups_to_file($filepath_groups, $og);
		
		foreach($users as $user)
		{
			$user_info 	= $user['user'];
			$og			= $user['og'];
			
			if(is_array($og))
			{
				export_groups_to_file($filepath_groups, $og);
			}
			
			//print_r($og);
			
			fputcsv($f_handle, $user_info);
		}
		
		fclose($f_handle);
		
		$export_path = file_directory_path() . '/' . EXPORT_FILE_PATH;
		
		file_check_directory($export_path, TRUE);
		file_move( $filepath_users, $export_path, FILE_EXISTS_RENAME );
		file_move( $filepath_groups, $export_path, FILE_EXISTS_RENAME );
	}

}

function export_groups_to_file($filename, $groups)
{
	$f_handle = fopen($filename, 'a');

	if($f_handle)
	{
		foreach($groups as $group)
		{
			$group_info = array_merge( $group['og'], $group['og_uid']);
			fputcsv($f_handle, $group_info);
		}
		
		fclose($f_handle);
	}
}

?>
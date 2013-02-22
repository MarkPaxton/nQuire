<?php 

function get_required_pi_modules()
{
	$pi_modules[] = 'ahah_helper';
	$pi_modules[] = 'views';
	$pi_modules[] = 'og';
    $pi_modules[] = 'og_access';
	$pi_modules[] = 'og_actions';
	$pi_modules[] = 'og_views';
	$pi_modules[] = 'pi';
	$pi_modules[] = 'pi_activity';
	$pi_modules[] = 'pi_activity_status';
	$pi_modules[] = 'pi_authoring';
	$pi_modules[] = 'pi_available_content_types';
	$pi_modules[] = 'pi_available_measures';
	$pi_modules[] = 'pi_breadcrumb';
	$pi_modules[] = 'pi_chart';
	$pi_modules[] = 'pi_data';
	$pi_modules[] = 'pi_header';
	$pi_modules[] = 'pi_hypothesis';
	$pi_modules[] = 'pi_hypothesis_conclusion';
	$pi_modules[] = 'pi_information';
	$pi_modules[] = 'pi_inquiry';
	$pi_modules[] = 'pi_inquiry_info';
	$pi_modules[] = 'pi_install_inquiry';
	$pi_modules[] = 'pi_key_answers';
	$pi_modules[] = 'pi_key_questions';
	$pi_modules[] = 'pi_manage_focus';
	$pi_modules[] = 'pi_phase';
	$pi_modules[] = 'pi_previous_next';
	$pi_modules[] = 'pi_result_presentations';
	$pi_modules[] = 'pi_selected_photographs';
	$pi_modules[] = 'pi_set_status';
	$pi_modules[] = 'pi_sort_data';
	$pi_modules[] = 'pi_sort_key_answers';
	$pi_modules[] = 'pi_sort_key_questions';
	$pi_modules[] = 'pi_sort_measures';
	$pi_modules[] = 'pi_sort_result_presentations';
	$pi_modules[] = 'pi_stage';
	$pi_modules[] = 'pi_stage_selector';
	$pi_modules[] = 'pi_summary';
	$pi_modules[] = 'pi_teacher_tools';
	$pi_modules[] = 'pi_user_import';
	$pi_modules[] = 'pi_wiki_notes';

	$pi_modules[] = 'comment';
	$pi_modules[] = 'forum';
	
	return $pi_modules;
}

function get_missing_modules_list() {
	
	$required = get_required_pi_modules();
	
	$modules = get_system_modules();
	
	$enabled = $modules[ 'enabled' ];
	$available = $modules[ 'available' ];
	
	foreach( $required as $name ) {
		
		if( !in_array( $name, $enabled ) ) {
			
			if( in_array( $name, $available ) ) {
				$required_disabled[ $name ] = $name;
			}
			else {
				$required_missing[ $name ] = $name;
			}
		} 
		
	}
	
	$modules_list[ 'required_disabled' ] = $required_disabled;
	$modules_list[ 'required_missing' ] = $required_missing;
	
	return $modules_list;
}

function get_system_modules() {
	
	$modules = module_rebuild_cache();
	
	foreach( $modules as $name => $module ) {
		
		if( $module->status  ) {
			$enabled[] = $name; 
		}
		else {
			$available[] = $name;
		}
		
	}
	
	$module_list[ 'enabled' ] = $enabled;
	$module_list[ 'available' ] = $available;
	
	return $module_list;
}
?>
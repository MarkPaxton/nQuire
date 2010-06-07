<?php 
/** @file This file contains all the functions that display the inquiry level view.
 * This is the page where the phases will be listed, e.g. with the octagon layout 
 */



function pi_inquiry_inquiry_view($node) {
	global $user;
	$uid = $user->uid;
    $image_path = drupal_get_path( 'module', 'pi' ) . '/images/';
	
	$details = get_inquiry_details_from_current_path();
	
	/*
	 * We are now assuming a 1-1 mapping of both activities and phases to inquiries
	 * This means we no longer need session ids
	 * to find the inquiry when on the phase or activity level
	*/
	
	
	//set current inquiry
	//$sid = get_current_session_id();
	//set_current_inquiry_for_session_and_user_id($details->inquiry_id, $sid, $uid);
	

	$loaded_stage = node_load($details->stage_id);
	drupal_set_title($loaded_stage->title);
	$output = "<p>" . $loaded_stage->body . "</p>";
	$phases = get_phases_of_inquiry_and_user($details->inquiry_id, $user->uid);

	// Header
	$header = array(
	t('Name'),
	t('Description'),
	);

	$rows = array();

	//current phase check
	$current_stage = get_current_stage_of_inquiry_and_user($details->inquiry_id, $user->uid);
	if (isset($current_stage->stage_id))
	{
		$current_stage_id = $current_stage->stage_id;
		$stage = node_load($current_stage_id);
		$phases_of_current_stage = $stage->phases;
	}
	else
	{
		$current_stage_id = '';
		$phases_of_current_stage = array();
	}

	foreach($phases as $phase_nid)
	{
		$phase = node_load($phase_nid);
		$row = array();
		if (in_array($phase->nid, $phases_of_current_stage)) {
			$row[] = l(t($phase->title . " <img width='12' src='" . $image_path . "/folder_empty_current.png'/>"), t("phase/" . $phase->nid), array('html' => TRUE));
		}
		else {
			$row[] = l($phase->title, t("phase/" . $phase->nid));
		}
		$row[] = $phase->body;
		$rows[] = array('data' => $row);
		$activity_count[] = count(get_activities_of_stage_phase_and_user($phase->inquiry_id, NULL, $phase->nid, $user->uid));
		$new_activity_count[] = new_activity_list_count($phase->inquiry_id, $phase->nid, $user->uid);
	}
	//get_phase_view_preference
	$output .= display_phases( $rows, $activity_count, $new_activity_count, $node->layout);
	//$output .= theme('table', $header, $rows);
	return $output;
}

/**
 * This function generates the HTML code for the "phase view". Dependant on the parameter layout, 
 * it generates either a list, a pentagon or an octagon view.
 * 
 * @param array $phases An array with the data for every phase.
 * @param array $activity_count An array of integer, indicating the amount of available activities in each phase.
 * @param array $new_activity_count An array of integers, indicating who many new activities exists for each phase.
 * @param string $layout The layout type to use (list, pentagon, octagon).
 * @return string $output The HTML code for the "phase view".
 */
function display_phases($phases, $activity_count, $new_activity_count, $layout) {
	
	$show_phase_icons = FALSE;
	$show_activities = FALSE;

	if( strpos($layout, 'icon') !== FALSE )
	{
		$show_phase_icons = TRUE;
	}

	if( strpos($layout, 'activity') !== FALSE )
        {
                $show_activities = TRUE;
        }

	if( strpos($layout, 'octagon') !== FALSE )
	{
		$layout = 'octagon';
	}

	
	if( is_array( $phases ) )
	{
		//reindex the array to make sure it counts from 0,1,2,3....
		//and does not contain missing indexes
		$phases = array_values( $phases );
	}
	
	drupal_add_css(drupal_get_path('module', 'pi') .'/pi_phase.css');
	
	$output = "";
	$image_path = drupal_get_path( 'module', 'pi' ) . '/images/';
	
	if ($layout == 'list') {
		$background_arrows_image = '<div style="position: absolute; top: 5px; left: 150px;">';
		$background_arrows_image .= '<img width="150" height="625" src="' . $image_path . 'arrows_loop.png" />';
		$left_positions = array('200px', '200px', '200px', '200px', '200px', '200px', '200px', '200px');
		$top_positions = array('35px', '110px', '185px', '260px', '335px', '410px', '485px', '560px');
		$phase_icons = array('topic.jpg', 'question.jpg', 'method.jpg', 'data_collection.jpg', 'analysis.jpg', 'conclusion.jpg', 'evaluation.jpg', 'presentation.jpg');
	}
	elseif ($layout == 'pentagon') {
		$background_arrows_image = '<div style="position: absolute; top: 23px; left: 151px;">';
		$background_arrows_image .= '<img width="300" height="172" src="' . $image_path . 'arrows_pentagon.png" />';
		$left_positions = array('180px', '345px', '320px', '40px', '15px');
		$top_positions = array('5px', '85px', '165px', '165px', '85px');
		$phase_icons = array('question.jpg', 'method.jpg', 'data_collection.jpg', 'analysis.jpg', 'conclusion.jpg');
	}
	elseif ($layout === 'octagon') {		
		$background_arrows_image = '<div style="position: absolute; top: 23px; left: 135px;">';
		$background_arrows_image .= '<img width="426" height="278" src="' . $image_path . 'arrows_octagon.png" />';
		$left_positions = array('380px', '450px', '450px', '380px', '80px', '10px', '10px', '80px');
		$top_positions = array('10px', '95px', '180px', '270px', '270px', '180px', '95px', '10px');
		$phase_icons = array('topic.jpg', 'question.jpg', 'method.jpg', 'data_collection.jpg', 'analysis.jpg', 'conclusion.jpg', 'evaluation.jpg', 'presentation.jpg');
	};
	
	$output .= '<div style="position: relative; width: 100%; height: 500px;">';
	$output .= $background_arrows_image;
	$output .= '</div>';
	
	if( is_array( $phases ) )
	{
		foreach( $phases as $number => $phases ) 
		{
			if( isset( $phases['data'] ) ) 
			{
				$data = $phases['data'];
				
				if( $phase_icons[ $number ] )
				{
					$icon_url = $image_path . $phase_icons[ $number ];
				}
				else
				{
					$icon_url =  null;
				}
				
				$title = $data[ 0 ];
				$activitys = $activity_count[ $number ];
				$new_activitys = $new_activity_count[ $number ];
					    						
				$output .= '<div style="position: absolute; top: ' . $top_positions[ $number ] . '; left: ' . $left_positions[ $number ] . ';" class="phasebox">';
				$output .= display_phases_box( $icon_url, $title, $activitys, $new_activitys, $show_phase_icons , $show_activities );					
				$output .= '</div></div>';
			}
		}
	}
	$output .= '</div>';
	return $output;
}

/**
 * This function generates the HTML div for one phase. Depending on the parameter show_icon and
 * show_activities, a different CSS style is used.
 * 
 * @param string $icon_url The URL to the icon image file. 
 * @param string $title The title of the phase. 
 * @param int $activity_count The amount of activities available in this phase.
 * @param int $new_activity_count The amount of new available activities in this phase.
 * @param boolean $show_icon Whether or not th show the icon image.
 * @param boolean $show_activities Whether or not to show the available and new activities.
 * @return string The HTML code for the phase division.
 */
function display_phases_box( $icon_url, $title, $activity_count, $new_activity_count, $show_icon = false, $show_activities = false )
{
	$output = '';
	
	if($show_icon === true)
	{
		$output .= display_phases_icon_box( $icon_url, $show_icon );
	}
	
	if( $show_icon !== true && $show_activities !== true )
	{
		$output .= '<div class="textcontnoicon">
						<div class="titleboxnoiconnoact">' . $title . '</div>';
	}
	else if( $show_icon !== true && $show_activities === true )
	{
		$output .= '<div class="textcontnoicon">
						<div class="titleboxnoicon"><span style="padding: 0px 0px 0px 5px">' . $title . '</span></div>';
	}
	else if( $show_icon === true && $show_activities !== true )
	{
		$output .= '<div class="textcont">
						<div class="titleboxnoact"><span style="padding: 0px 0px 0px 5px">' . $title . '</span></div>';
	}
	else
	{
		$output .= '<div class="textcont">
						<div class="titlebox"><span style="padding: 0px 0px 0px 5px">' . $title . '</span></div>';
	}
	
	if($show_activities === true)
	{
		$output .= display_phases_activity_box( $activity_count, $new_activity_count, $show_activities );
	}
	
	return $output;
}

/**
 * This function generates the HTML div with the icon image. The return value is the HTML code for the div.
 *  
 * @param string $icon_url A string representing the URL to the icon image.
 * @return string $output The HTML code for the icon div.
 */
function display_phases_icon_box($icon_url)
{
	$output = '';
	
	$output .= '<div class="imgbox">';
	
	if( !empty( $icon_url ) )
	{
		$output .= '<img height="40px" width="40px" src="' . $icon_url . '" />';
	}
		
	$output .= '</div>';
	
	return $output;
}

/**
 * This function generates the HTML div for activities and new activities. The return value is the HTML code for the div.
 * 
 * @param int $activity_count Amount of available activities in this phase.
 * @param int $new_activity_count Amount of new activities in this phase.
 * @return string $output HTML code for the activities div.
 */
function display_phases_activity_box($activity_count, $new_activity_count)
{
	$output = '';
	
	if(!is_numeric($activity_count))
	{
		$activity_count = '0';
	}
	
	$output = '<div class="activitybox">
						<span style="padding: 0px 0px 0px 5px">' . $activity_count . ' Activities</span>
				</div>
				<div class="newactbox">';								
	if($new_activity_count > 0) 
	{
			$output .= '<span style="padding: 0px 0px 0px 5px"> ' . $new_activity_count . ' new!</span>';
	}			
		
	$output .= '</div>';
	
	return $output;
}

?>

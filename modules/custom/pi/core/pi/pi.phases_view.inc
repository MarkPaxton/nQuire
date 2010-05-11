<?php 

function display_phases($headings, $rows, $activity_count, $new_activity_count, $layout ) {
	
	$show_phase_icons = true;
	$show_activities = true;
	
	if( is_array( $rows ) )
	{
		//reindex the array to make sure it counts from 0,1,2,3....
		//and does not contain missing indexes
		$rows = array_values( $rows );
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
		$phase_icons = array('topic.jpg', 'question.jpg', 'method.jpg', 'data_collection.jpg', '', 'conclusion.jpg', 'evaluation.jpg', 'presentation.jpg');
	};
	
	$output .= '<div style="position: relative; width: 100%; height: 500px;">';
	$output .= $background_arrows_image;
	$output .= '</div>';
	
	//$count = 0;
	//if( count($rows) )
	if( is_array( $rows ) )
	{
		foreach( $rows as $number => $row ) 
		{
			if( isset( $row['data'] ) ) 
			{
				$data = $row['data'];
				
				if( $phase_icons[ $number ] )
				{
					$icon_url = $image_path . $phase_icons[ $number ];
				}
				else
				{
					$icon_url =  null;
				}
				
				$title = $data[ 0 ];
				$activity_count = $activity_count[ $number ];
				$new_activity_count = $new_activity_count[ $number ];
					    						
				$output .= '<div style="position: absolute; top: ' . $top_positions[ $number ] . '; left: ' . $left_positions[ $number ] . ';" class="phasebox">';
				$output .= display_phases_box( $icon_url, $title, $activity_count, $new_activity_count, $show_phase_icons , $show_activities );					
				$output .= '</div></div>';
								
				//$count = $count + 1;
			}
		}
	}

	$output .= '</div>';
	return $output;

}

function display_phases_box( $icon_url, $title, $activity_count, $new_activity_count, $show_icon = false, $show_activities = false )
{
	$output = '';
	
	$output .= display_phases_icon_box( $icon_url, $show_icon );
	
	if( $show_icon === false && $show_activities === false )
	{
		
		$output .= '<div class="textcontnoicon">
						<div class="titleboxnoiconnoact"><span style="position: absolute; top: 50%; height: 20px; margin: -10px 0px -10px 0px; width: 100%; text-align: center">' . $title . '</span></div>';
	}
	else if( $show_icon === false && $show_activities !== false )
	{
		$output .= '<div class="textcontnoicon">
						<div class="titleboxnoicon"><span style="padding: 0px 0px 0px 5px">' . $title . '</span></div>';
	}
	else if( $show_icon !== false && $show_activities === false )
	{
		$output .= '<div class="textcont">
						<div class="titleboxnoact"><span style="padding: 0px 0px 0px 5px">' . $title . '</span></div>';
	}
	else
	{
		$output .= '<div class="textcont">
						<div class="titlebox"><span style="padding: 0px 0px 0px 5px">' . $title . '</span></div>';
	}
	
	$output .= display_phases_activity_box( $activity_count, $new_activity_count, $show_activities );
	
	return $output;
}

function display_phases_icon_box( $icon_url, $show_icon = false )
{
	$output = '';
	
	if( $show_icon !== false )
	{
		$output .= '<div class="imgbox">';
	
		if( !empty( $icon_url ) )
		{
			$output .= '<img height="40px" width="40px" src="' . $icon_url . '" />';
		}
		
		$output .= '</div>';
	}
	
	return $output;
}

function display_phases_activity_box( $activity_count, $new_activity_count, $show_activities = false )
{
	$output = '';
	
	if( !is_numeric( $activity_count ) )
	{
		$activity_count = '0';
	}
	
	if( $show_activities !== false )
	{
		$output = '<div class="activitybox">
						<span style="padding: 0px 0px 0px 5px">' . $activity_count . ' Activities</span>
					</div>
					<div class="newactbox">';								
		if ($new_activity_count > 0) 
		{
			$output .= '<span style="padding: 0px 0px 0px 5px"> ' . $new_activity_count[$count] . ' new!</span>';
		}			
		
		$output .= '</div>';
	}
	
	return $output;
}


?>
<?php

define( 'TAB_CHARACTER_SIZE',	8 );
define( 'MAX_TABS',	5 );

function print_stacktrace( $error_log = FALSE, $stop = FALSE ) {
	
	$stacktrace_info;
	
	$stacktrace = debug_backtrace();
	
	if( is_array( $stacktrace ) ) {
		
		foreach( $stacktrace as $call ) {
			
			if( $call['function'] !== 'print_stacktrace' ) {
				$stacktrace_info .= format_log_output( $call['line'], 1 ) . "" . 
									format_log_output( $call['function'] ) . "" . $call['file'] . "\n";
			} 
			
		}
		
	}
	
	
	if( $error_log === TRUE ) {
		error_log( $stacktrace_info );
	}
	else {
		$stacktrace_info = "<pre>" . $stacktrace_info . "</pre>";
		print( $stacktrace_info );
	}
	
	if( $stop === TRUE ) {
		die();
	}
	
}

function format_log_output( $message, $max_tabs = MAX_TABS ) {
	
	$counter = 0;
	$info = $message;
	
	if( strlen( $message ) > 0 ) {
		$tabs = $max_tabs - floor( strlen( $message ) / TAB_CHARACTER_SIZE );
	}
	else {
		$tabs = $max_tabs;
	}
	
	while( $counter < $tabs ) {	
		$info .=  "\t";
		$counter++;
	}
	
	return $info;
}
	
?>
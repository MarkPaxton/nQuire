<?php

define( 'TAB_CHARACTER_SIZE',	8 );
define( 'MAX_TABS',	5 );


/**
* This is a debug function that prints a given variable to drupal message area
* it uses output buffers (ob_start, ob_get_clean) and var_dump
* @param $var_to_dump any variable
*/
function pi_debug_message($var_to_dump)
{
	ob_start();
	if(is_string($var_to_dump))
	{
		echo($var_to_dump);
	}
	else
	{
		var_dump($var_to_dump);
	}
	drupal_set_message(t('<pre>@data</pre>', array('@data' => ob_get_clean()), 'status'));
}


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
	

/*
function process_error_backtrace($errno, $errstr, $errfile, $errline, $errcontext) {
if(!(error_reporting() & $errno))
return;
switch($errno) {
case E_WARNING      :
case E_USER_WARNING :
case E_STRICT       :
case E_NOTICE       :
case E_USER_NOTICE  :
$type = 'warning';
$fatal = false;
break;
default             :
$type = 'fatal error';
$fatal = true;
break;
}
$trace = array_reverse(debug_backtrace());
array_pop($trace);
if(php_sapi_name() == 'cli') {
echo 'Backtrace from ' . $type . ' \'' . $errstr . '\' at ' . $errfile . ' ' . $errline . ':' . "\n";
foreach($trace as $item)
echo '  ' . (isset($item['file']) ? $item['file'] : '<unknown file>') . ' ' . (isset($item['line']) ? $item['line'] : '<unknown line>') . ' calling ' . $item['function'] . '()' . "\n";
} else {
echo '<p class="error_backtrace">' . "\n";
echo '  Backtrace from ' . $type . ' \'' . $errstr . '\' at ' . $errfile . ' ' . $errline . ':' . "\n";
echo '  <ol>' . "\n";
foreach($trace as $item)
echo '    <li>' . (isset($item['file']) ? $item['file'] : '<unknown file>') . ' ' . (isset($item['line']) ? $item['line'] : '<unknown line>') . ' calling ' . $item['function'] . '()</li>' . "\n";
echo '  </ol>' . "\n";
echo '</p>' . "\n";
}
if(ini_get('log_errors')) {
$items = array();
foreach($trace as $item)
$items[] = (isset($item['file']) ? $item['file'] : '<unknown file>') . ' ' . (isset($item['line']) ? $item['line'] : '<unknown line>') . ' calling ' . $item['function'] . '()';
$message = 'Backtrace from ' . $type . ' \'' . $errstr . '\' at ' . $errfile . ' ' . $errline . ': ' . join(' | ', $items);
error_log($message);
}
if($fatal)
{
exit(1);
}
}

set_error_handler('process_error_backtrace');

*/


?>
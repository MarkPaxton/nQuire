<?php

$class = new stdClass();

if($class instanceof stdClass )
{
	echo "Class...";
}
else 
{
	echo "not this type";
}

print_r($class);

?>
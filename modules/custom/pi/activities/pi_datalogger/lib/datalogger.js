function saveData(data)
{
	//var myObject = eval('(' + data + ')');
	var myObject = null;
	try {
		myObject = jQuery.parseJSON(data);
		alert( "Saving data..." );
	}
	catch(error)
	{ 
		alert("Error receiving data:\n" + error.message);
	}
	return myObject;
}
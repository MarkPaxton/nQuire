function saveData(data)
{
	//var myObject = eval('(' + data + ')');
	var dataToSave = null;
	try {
		dataToSave = eval('(' + data + ')');
		for(item in dataToSave)
		{
			alert(item[0].datafield + " = " + item[1].value); 
		}
	}
	catch(error)
	{ 
		alert("Error receiving data:\n" + error.message);
	}
	return dataToSave;
}
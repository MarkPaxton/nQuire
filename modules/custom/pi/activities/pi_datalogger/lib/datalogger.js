function saveData(data)
{
	try
	{
		$('#edit-datalogger-data-datalogger-values').val(data);
		$('#edit-submit-button').trigger('click');
	}
	catch(error)
	{
		alert("Error receiving data:\n" + error.message);
	}
}
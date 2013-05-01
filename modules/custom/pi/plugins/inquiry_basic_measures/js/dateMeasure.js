$(function() {
	nQuireJsSupport.register('InquiryBasicMeasuresDate', {
		init: function() {
			$('div[measure_type="measure_widget__date"]').each(function() {
				var inputElementId = $(this).attr('input_element_id');
				$('input[name="' + inputElementId + '"]').datepicker({
					dateFormat: "yy-mm-dd"
				});
			});
		}
	});
});



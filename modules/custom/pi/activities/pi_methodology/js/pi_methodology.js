

$(function() {
	nQuireJsSupport.register('PiMethodologyWidget', {
		init: function() {
			var stop = function() {
				$('.measure_container').each(function() {
					var region = $(this).attr('region_id');
					var weight = 0;
					$(this).children('.measure_item').each(function() {
						var measure = $(this).attr('measure_id');
						$('input[name="measures_edit[measure_' + measure + '][region]"]').attr('value', region);
						$('input[name="measures_edit[measure_' + measure + '][weight]"]').attr('value', weight);
						++weight;
					});
				});
			};

			var start = null;

			//$('.measure_item').prepend($('<div>').addClass('nquire-item-handle'));

			$('.measure_container').each(function() {
				$(this).sortable({
					items: '.measure_item',
					connectWith: '.measure_container',
					revert: 100,
					start: start,
					stop: stop
				});
			});
		}
	});
});

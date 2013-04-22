




(function($) {
	var methods = {
		init: function(options) {
			this.data('options', options);

			var self = this;

			this.find('.virtual_microscope_measure_start_button').customMouseInput('click', function() {
				$(this).vmUserInteractionMeasure('_buttonClick', options.startCallback);
			});

			this.find('.virtual_microscope_measure_cancel_button').customMouseInput('click', function() {
				$(this).vmUserInteractionMeasure('_buttonClick', options.cancelCallback);
			});

			this.find('.virtual_microscope_measure_clear_button').customMouseInput('click', function() {
				$(this).vmUserInteractionMeasure('_buttonClick', options.clearCallback);
			});

			this.find('.virtual_microscope_measure_save_button').customMouseInput('click', function() {
				$(this).vmUserInteractionMeasure('_buttonClick', options.saveCallback);
			});

			return this;
		},
		_buttonClick: function(callback) {
			if (callback && !this.hasClass('virtual_microscope_measure_button_disabled') && !this.hasClass('virtual_microscope_measure_button_hidden')) {
				callback();
			}
		},
		setActiveMode: function(active) {
			if (active) {
				this.find('.virtual_microscope_measure_start_button, .virtual_microscope_measure_clear_button').addClass('virtual_microscope_measure_button_hidden');
				this.find('.virtual_microscope_measure_cancel_button, .virtual_microscope_measure_save_button').removeClass('virtual_microscope_measure_button_hidden');
			} else {
				this.find('.virtual_microscope_measure_cancel_button, .virtual_microscope_measure_save_button').addClass('virtual_microscope_measure_button_hidden');
				this.find('.virtual_microscope_measure_start_button, .virtual_microscope_measure_clear_button').removeClass('virtual_microscope_measure_button_hidden');
				var options = this.data('options');
				if (options && options.emptyValueCallback && options.emptyValueCallback()) {
					this.find('.virtual_microscope_measure_clear_button').addClass('virtual_microscope_measure_button_disabled');
				} else {
					this.find('.virtual_microscope_measure_clear_button').removeClass('virtual_microscope_measure_button_disabled');
				}
			}
		}
	};

	$.fn.vmUserInteractionMeasure = function(method) {
		if (methods[method]) {
			return methods[method].apply(this, Array.prototype.slice.call(arguments, 1));
		} else if (typeof method === 'object' || !method) {
			return methods.init.apply(this, arguments);
		} else {
			console.log('Method ' + method + ' does not exist on jQuery.vmUserInteractionMeasure');
			return false;
		}
	};
})(jQuery);



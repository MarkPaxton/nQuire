/*global jquery, JSON*/

$(function() {
	var ColorMeasureManager = function(element, dataService, measureId, colors) {
		this._element = $(element);
		this._box = this._element.find('.virtual_microscope_color_measure_box');
		this._measureId = measureId;
		this._colors = colors;

		this._value = null;

		var self = this;

		this._element.vmUserInteractionMeasure({
			startCallback: function() {
				self._startInput();
			},
			cancelCallback: function() {
				self._cancelInput();
			},
			clearCallback: function() {
				self._clearValue();
			},
			emptyValueCallback: function() {
				return !self._value;
			},
			dataService: dataService
		});

		$('body').rockColorPicker({
			selectionCallback: function(value) {
				self._saveAndStop(value);
			},
			closeCallback: function() {
				self._stopInput();
			}
		});

	};

	ColorMeasureManager.prototype.setServiceDelegate = function(delegate) {
		this._serviceDelegate = delegate;
	};

	ColorMeasureManager.prototype.initMeasureValue = function(value) {
		this._value = value && value.length > 0 ? value : null;
		$('body').rockColorPicker('select', this._value);
		this._element.vmUserInteractionMeasure('setActiveMode', false);
		this._updateDisplayValue();
	};

	ColorMeasureManager.prototype._clearValue = function() {
		this._value = null;
		$('body').rockColorPicker('select', this._value);
		this._element.vmUserInteractionMeasure('setActiveMode', false);
		this._serviceDelegate.saveData('', true);
		this._updateDisplayValue();
	};
	
	ColorMeasureManager.prototype.clearValue = function() {
		this._value = null;
		$('body').rockColorPicker('select', this._value);
		this._element.vmUserInteractionMeasure('setActiveMode', false);
		this._serviceDelegate.saveData('');
		this._updateDisplayValue();
	};


	ColorMeasureManager.prototype.stopUserDelayProcess = function() {
		this._cancelInput();
	};

	ColorMeasureManager.prototype._saveAndStop = function(value) {
		this._value = value ? value : null;
		this._serviceDelegate.saveData(this._value, true);
		this._stopInput();
	};

	ColorMeasureManager.prototype._cancelInput = function() {
		$('body').rockColorPicker('close');
		this._stopInput();
	};

	ColorMeasureManager.prototype._stopInput = function() {
		this._element.vmUserInteractionMeasure('setActiveMode', false);
		this._updateDisplayValue();
		this._serviceDelegate.userDelayProcessStopped();
	};

	ColorMeasureManager.prototype._startInput = function() {
		var self = this;
		var callback = function() {
			$('body').rockColorPicker('open');
			self._element.vmUserInteractionMeasure('setActiveMode', true);
		};

		this._serviceDelegate.requestUserDelayProcessStart(callback);
	};

	ColorMeasureManager.prototype._updateDisplayValue = function() {
		var color = this._value ? this._colors[parseInt(this._value)] : false;
		if (color) {
			this._box.html(color.name);
			this._box.css('background', color.color);
			if (!color.textColor) {
				var r = parseInt(color.color.substr(1, 2), 16);
				var g = parseInt(color.color.substr(3, 2), 16);
				var b = parseInt(color.color.substr(5, 2), 16);
				var l = r * .299 + g * .587 + b * .114;
				color.textColor = l < 128 ? 'white' : 'black';
			}
			this._box.css('color', color.textColor);
		} else {
			this._box.html('');
			this._box.css('background', 'white');
		}
	};


	nQuireJsSupport.register('VirtualMicroscopeColorMeasure', {
		init: function(dependencies) {
			$('div[measure_type="measure_widget_vm_color"]').each(function() {
				var inputElementId = $(this).attr('input_element_id');
				var manager = new ColorMeasureManager(this, dependencies.AjaxDataService, inputElementId, dependencies.VirtualMicroscopeColorMeasureData.colors);
				dependencies.DynamicMeasureService.registerMeasure(inputElementId, manager);
			});
		}
	}, ['AjaxDataService', 'DynamicMeasureService', 'VirtualMicroscopeColorMeasureData']);
});

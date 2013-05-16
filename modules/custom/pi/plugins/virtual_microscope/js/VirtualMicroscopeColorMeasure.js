/*global jquery, JSON*/

$(function() {
	var ColorMeasureManager = function(element, dataService, measureId) {
		this._element = $(element);
		this._box = this._element.find('.virtual_microscope_color_measure_box');
		this._measureId = measureId;

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

			},
			saveCallback: function() {
				self._saveAndStop();
			},
			emptyValueCallback: function() {
				return !self._value;
			},
			dataService: dataService
		});
	};

	ColorMeasureManager.prototype.setServiceDelegate = function(delegate) {
		this._serviceDelegate = delegate;
	};

	ColorMeasureManager.prototype.initMeasureValue = function(value) {
		this._value = value && value.length > 0 ? value : null;
		this._element.vmUserInteractionMeasure('setActiveMode', false);
		this._updateDisplayValue();
	};

	ColorMeasureManager.prototype.clearValue = function() {
		this._value = null;
		this._element.vmUserInteractionMeasure('setActiveMode', false);
		this._serviceDelegate.saveData('');
		this._updateDisplayValue();
	};


	ColorMeasureManager.prototype.stopUserDelayProcess = function() {
		/* since this is not cancel, accept temporal data and close */
		this._saveAndStop();
	};

	ColorMeasureManager.prototype._saveAndStop = function() {
		this._value = this._editingInfo;
		this._serviceDelegate.saveData(JSON.stringify(this._value), true);
		this._stopInput();
	};

	ColorMeasureManager.prototype._cancelInput = function() {
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
			self._element.vmUserInteractionMeasure('setActiveMode', true);
		};

		this._serviceDelegate.requestUserDelayProcessStart(callback);
	};

	ColorMeasureManager.prototype._updateDisplayValue = function() {
		this._box.html(this._value ? this._value : '');
	};


	nQuireJsSupport.register('VirtualMicroscopeNumberMeasure', {
		init: function(dependencies) {
			$('div[measure_type="measure_widget_vm_color"]').each(function() {
				var inputElementId = $(this).attr('input_element_id');
				var manager = new ColorMeasureManager(this, dependencies.AjaxDataService, inputElementId);
				dependencies.DynamicMeasureService.registerMeasure(inputElementId, manager);
			});
		}
	}, ['AjaxDataService', 'DynamicMeasureService']);
});

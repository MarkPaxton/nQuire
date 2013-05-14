/*global jquery, JSON*/

$(function() {

	var LengthPainter = function(measureId, dataBrowser) {
		this._measureId = measureId;
		this._dataBrowser = dataBrowser;
		this._inputActive = false;
	};

	LengthPainter.prototype.getMeasureType = function() {
		return 'length';
	};

	LengthPainter.prototype.setInputActive = function(inputActive) {
		if (this._inputActive !== inputActive) {
			this._inputActive = inputActive;
			this._updatePaint();
		}
	};

	LengthPainter.prototype._updatePaint = function() {
		this._dataBrowser.updateCurrentDataFeaturePaint(this._measureId);
	};

	LengthPainter.prototype.createPaintShape = function(data, options) {
		
		var value = data ? data[this._measureId] : false;
		console.log(value, options, this._inputActive);
		return null;
		var shapes = (options.mode === 'selected' && this._editingInfo) ? this._editingInfo.shapes : this._parseShapes(data[this._measureId]);

		var output = {
			pos: {x: 0, y: 0},
			shapes: []
		};

		for (var i in shapes) {
			output.shapes.push({
				type: 'polyline',
				points: shapes[i].points,
				settings: $.extend({}, this._baseStyle, {stroke: this._baseStyleColors[shapes[i].color]})
			});
		}


		return output;
	};

	nQuireJsSupport.register('VirtualMicroscopeLengthMeasure', {
		init: function(dependencies) {
			$('div[measure_type="measure_widget_vm_length"]').each(function() {
				var inputElementId = $(this).attr('input_element_id');
				var inputElementTitle = $(this).attr('input_element_title');
				var lengthManager = new LengthPainter(inputElementId, dependencies.VirtualMicroscopeDataBrowser);

				var manager = dependencies.VirtualMicroscopeNumberMeasure.createManager(this, inputElementId, lengthManager);
				dependencies.DynamicMeasureService.registerMeasure(inputElementId, manager);
				dependencies.VirtualMicroscopeDataBrowser.registerPaintFeature(inputElementId, inputElementTitle, lengthManager);
			});
		}
	}, ['VirtualMicroscopeNumberMeasure', 'VirtualMicroscopeDataBrowser', 'DynamicMeasureService']);
});



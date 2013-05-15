/*global jquery, JSON*/

$(function() {

	var LengthPainter = function(measureId, dataBrowser, color, actionCallbacks) {
		this._measureId = measureId;
		this._dataBrowser = dataBrowser;
		this._actionCallbacks = actionCallbacks;

		this._activeInput = true;
		this._activeDataValue = null;

		this._baseStyle = {
			fill: 'none',
			'stroke-width': 20,
			'stroke-linecap': 'butt',
			'stroke-linejoin': 'round',
			'vector-effect': 'non-scaling-stroke'
		};
		
		var darkColor = color.replace(/f/g, 'a');
		
		this._normalStyle = {stroke: darkColor, strokeWidth: 4};
		this._selectedStyle = {stroke: color, strokeWidth: 4};
		this._hoverStyle = {stroke: color, strokeWidth: 6};
		this._editingStyle = {stroke: color, strokeWidth: 10, strokeOpacity: .5};
	};

	LengthPainter.prototype.getMeasureType = function() {
		return 'length';
	};

	LengthPainter.prototype.setPaintValueForSave = function(activeInput, value) {
		this._activeInput = activeInput;
		this._activeDataValue = value;
	};

	LengthPainter.prototype.createPaintShape = function(data, options) {
		var selected = options.mode === 'selected';
		var hover = options.mode === 'hover';

		try {
			var value = selected && this._activeDataValue ? this._activeDataValue : JSON.parse(data[this._measureId]);
		} catch (e) {
			value = null;
		}

		if (value) {

			var style = selected && this._activeInput ? this._editingStyle : (hover ? this._hoverStyle : (selected ? this._selectedStyle : this._normalStyle));

			var settings = $.extend({}, this._baseStyle, style);

			var yl = value.y11 - value.y12;
			var xl = value.x11 - value.x12;
			var angle = Math.atan2(yl, xl) + .5 * Math.PI;
			var length = .1 * Math.sqrt(xl * xl + yl * yl);
			var dx = length * Math.cos(angle);
			var dy = length * Math.sin(angle);


			var output = {
				pos: {x: 0, y: 0},
				shapes: [{
						type: 'line',
						points: {x1: value.x11 - dx, y1: value.y11 - dy, x2: value.x11 + dx, y2: value.y11 + dy},
						settings: settings,
						callbacks: this._actionCallbacks
					}, {
						type: 'line',
						points: {x1: value.x11, y1: value.y11, x2: value.x12, y2: value.y12},
						settings: settings,
						callbacks: this._actionCallbacks
					}, {
						type: 'line',
						points: {x1: value.x12 - dx, y1: value.y12 - dy, x2: value.x12 + dx, y2: value.y12 + dy},
						settings: settings,
						callbacks: this._actionCallbacks
					}]
			};
			return output;
		}

		return null;
	};

	nQuireJsSupport.register('VirtualMicroscopeLengthMeasure', {
		init: function(dependencies) {
			$('div[measure_type="measure_widget_vm_length"]').each(function() {
				var inputElementId = $(this).attr('input_element_id');
				var inputElementTitle = $(this).attr('input_element_title');
				var measureColor = $(this).attr('measure_color');
				
				var actionCallbacks = {
					click: function(featureName) {
						var id = "" + parseInt(featureName);
						dependencies.AjaxDataService.setData(id);
					}
				};


				var lengthManager = new LengthPainter(inputElementId, dependencies.VirtualMicroscopeDataBrowser, measureColor, actionCallbacks);

				var manager = dependencies.VirtualMicroscopeNumberMeasure.createManager(this, inputElementId, lengthManager);
				dependencies.DynamicMeasureService.registerMeasure(inputElementId, manager);
				dependencies.VirtualMicroscopeDataBrowser.registerPaintFeature(inputElementId, inputElementTitle, lengthManager, true);
			});
		}
	}, ['VirtualMicroscopeNumberMeasure', 'VirtualMicroscopeDataBrowser', 'DynamicMeasureService', 'AjaxDataService']);
});

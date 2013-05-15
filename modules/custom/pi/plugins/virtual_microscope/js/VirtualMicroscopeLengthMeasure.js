/*global jquery, JSON*/

$(function() {

	var LenghtPainterColors = {
		_n: 0,
		_colors: [
			['#aa0000', '#ff0000'],
			['#00aa00', '#00ff00'],
			['#0000aa', '#0000ff'],
			['#aaaa00', '#ffff00'],
			['#aa00aa', '#ff00ff'],
			['#00aaaa', '#00ffff']
		],
		nextColor: function() {
			var c = this._colors[this._n % this._colors.length];
			this._n++;
			return c;
		}
	};

	var LengthPainter = function(measureId, dataBrowser) {
		this._measureId = measureId;
		this._dataBrowser = dataBrowser;

		this._activeInput = true;
		this._activeDataValue = null;

		var color = LenghtPainterColors.nextColor();

		this._baseStyle = {
			fill: 'none',
			'stroke-width': 20,
			'stroke-linecap': 'butt',
			'stroke-linejoin': 'round',
			'vector-effect': 'non-scaling-stroke'
		};
		this._normalStyle = {stroke: color[0], strokeWidth: 4};
		this._hoverStyle = {stroke: color[1], strokeWidth: 6};
		this._editingStyle = {stroke: color[1], strokeWidth: 10, strokeOpacity: .5};
	};

	LengthPainter.prototype.getMeasureType = function() {
		return 'length';
	};

	LengthPainter.prototype.setPaintValueForSave = function(activeInput, value) {
		this._activeInput = activeInput;
		this._activeDataValue = value;
	};

	LengthPainter.prototype.createPaintShape = function(data, options) {
		try {
			var value = options.mode === 'selected' && this._activeDataValue ? this._activeDataValue : JSON.parse(data[this._measureId]);
		} catch (e) {
			value = null;
		}

		if (value) {

			var style = options.mode === 'selected' && this._activeInput ? this._editingStyle : (options.mode === 'hover' ? this._hoverStyle : this._normalStyle);

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
						settings: settings
					}, {
						type: 'line',
						points: {x1: value.x11, y1: value.y11, x2: value.x12, y2: value.y12},
						settings: settings
					}, {
						type: 'line',
						points: {x1: value.x12 - dx, y1: value.y12 - dy, x2: value.x12 + dx, y2: value.y12 + dy},
						settings: settings
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
				var lengthManager = new LengthPainter(inputElementId, dependencies.VirtualMicroscopeDataBrowser);

				var manager = dependencies.VirtualMicroscopeNumberMeasure.createManager(this, inputElementId, lengthManager);
				dependencies.DynamicMeasureService.registerMeasure(inputElementId, manager);
				dependencies.VirtualMicroscopeDataBrowser.registerPaintFeature(inputElementId, inputElementTitle, lengthManager);
			});
		}
	}, ['VirtualMicroscopeNumberMeasure', 'VirtualMicroscopeDataBrowser', 'DynamicMeasureService']);
});

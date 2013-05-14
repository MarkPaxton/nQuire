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
		this._inputActive = false;
		this._value = null;

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

	LengthPainter.prototype.setPaintValue = function(value) {
		this._value = value;
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
		if (this._value) {

			var style = this._inputActive ? this._editingStyle : (options.mode === 'hover' ? this._hoverStyle : this._normalStyle);
			var settings = $.extend({}, this._baseStyle, style);

			var yl = this._value.y11 - this._value.y12;
			var xl = this._value.x11 - this._value.x12;
			var angle = Math.atan2(yl, xl) + .5 * Math.PI;
			var length = .1 * Math.sqrt(xl * xl + yl * yl);
			var dx = length * Math.cos(angle);
			var dy = length * Math.sin(angle);

			var output = {
				pos: {x: 0, y: 0},
				shapes: [{
						type: 'line',
						points: {x1: this._value.x11 - dx, y1: this._value.y11 - dy, x2: this._value.x11 + dx, y2: this._value.y11 + dy},
						settings: settings
					}, {
						type: 'line',
						points: {x1: this._value.x11, y1: this._value.y11, x2: this._value.x12, y2: this._value.y12},
						settings: settings
					}, {
						type: 'line',
						points: {x1: this._value.x12 - dx, y1: this._value.y12 - dy, x2: this._value.x12 + dx, y2: this._value.y12 + dy},
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

/*global jquery, JSON*/

$(function() {
	var NumberMeasureManager = function(element, eventManager, dataBrowser, measureId, measureManager) {
		this._element = $(element);
		this._box = this._element.find('.virtual_microscope_number_measure_box');
		this._startButton = this._element.find('button');
		this._measureId = measureId;
		this._eventManager = eventManager;
		this._dataBrowser = dataBrowser;
		this._measureManager = measureManager;

		this._baseStyleColors = {blue: '#0000ff', red: '#ff0000', yellow: '#ffff00', green: '#00ff00'};
		this._baseStyle = {fill: 'none', 'stroke-width': 20, 'stroke-linecap': 'round', 'stroke-linejoin': 'round'};

		this._value = null;
		this._capturing = false;
		this._editingInfo = null;

		//this._dataBrowser.registerPaintFeature(measureId, 'Number', this);
		var self = this;

		this._element.vmUserInteractionMeasure({
			startCallback: function() {
				self._startInput();
			},
			cancelCallback: function() {
				self._stopInput();
			},
			clearCallback: function() {

			},
			saveCallback: function() {
				self._saveAndStop();
			}
		});
	};

	NumberMeasureManager.prototype.setServiceDelegate = function(delegate) {
		this._serviceDelegate = delegate;
	};

	NumberMeasureManager.prototype.initMeasureValue = function(value) {
		this._value = value && value.length > 0 ? JSON.parse(value) : null;
		this._element.vmUserInteractionMeasure('setActiveMode', false);
		this._updateDisplayValue();
	};

	NumberMeasureManager.prototype.clearValue = function() {
		this._value = null;
		this._serviceDelegate.saveData('');
		this._updateDisplayValue();
	};

	NumberMeasureManager.prototype._updatePaint = function() {
		this._dataBrowser.updateCurrentDataFeaturePaint(this._measureId);
	};

	NumberMeasureManager.prototype.createPaintShape = function(data, options) {

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


	NumberMeasureManager.prototype.stopUserDelayProcess = function() {
		/* since this is not cancel, accept temporal data and close */
		this._saveAndStop();
	};

	NumberMeasureManager.prototype._saveAndStop = function() {
		this._value = this._editingInfo;
		this._serviceDelegate.saveData(JSON.stringify(this._value), true);
		this._stopInput();
	};

	NumberMeasureManager.prototype._stopInput = function() {
		this._editingInfo = null;
		this._capturing = false;
		this._eventManager.stopListening();
		this._element.vmUserInteractionMeasure('setActiveMode', false);
		this._serviceDelegate.userDelayProcessStopped();

		this._updateDisplayValue();
		
		this._measureManager.setInputActive(false);
	};

	NumberMeasureManager.prototype._startInput = function() {
		this._editingInfo = this._value;
		this._capturing = true;
		this._serviceDelegate.userDelayProcessStarted();
		this._element.vmUserInteractionMeasure('setActiveMode', true);
		this._eventManager.startListening(this);
		
		this._measureManager.setInputActive(true);
	};

	NumberMeasureManager.prototype.getMeasureType = function() {
		return this._measureManager.getMeasureType();
	};
	NumberMeasureManager.prototype.getMeasureValue = function() {
		return this._value;
	};
	NumberMeasureManager.prototype.newMeasureValue = function(value) {
		if (this._capturing) {
			if (JSON.stringify(value) !== JSON.stringify(this._editingInfo)) {
				this._editingInfo = value;
				this._updateDisplayValue();
				this._serviceDelegate.dataChanged();
			}
		}
	};

	NumberMeasureManager.prototype._updateDisplayValue = function() {
		var value = this._capturing ? this._editingInfo : this._value;
		this._box.html(value ? value.value : '');
	};


	nQuireJsSupport.register('VirtualMicroscopeNumberMeasure', {
		init: function(dependencies) {
			this._dependencies = dependencies;
			this._measureListener = null;
			var self = this;
			dependencies.VirtualMicroscopeManager.addMeasureListener(function(measure) {
				self.measureMessage(measure);
			});
		},
		createManager: function(element, measureId, measureType) {
			return new NumberMeasureManager(element, this, this._dependencies.VirtualMicroscopeDataBrowser, measureId, measureType);
		},
		stopListening: function(measureManager) {
			if (this._measureListener === measureManager) {
				this._measureListener = null;
			}
			this._dependencies.VirtualMicroscopeManager.stopMeasureTool();
		},
		startListening: function(measureManager) {
			this._measureListener = null;
			var value = measureManager.getMeasureValue();

			if (!value) {
				value = {
					type: measureManager.getMeasureType()
				};
			}

			var initialValue = value.type === 'angle' ? {
				state: 'angle',
				setInPixels: true,
				x1: value.x11,
				x2: value.x12,
				y1: value.y11,
				y2: value.y12,
				altx1: value.x21,
				altx2: value.x22,
				alty1: value.y21,
				alty2: value.y22
			} : {
				state: 'measure',
				setInPixels: true,
				x1: value.x11,
				x2: value.x12,
				y1: value.y11,
				y2: value.y12
			};


			this._dependencies.VirtualMicroscopeManager.setMeasureValue(initialValue);
			this._measureListener = measureManager;
		},
		measureMessage: function(value) {
			if (this._measureListener) {
				var type = typeof(value.altx1) === 'undefined' ? 'length' : 'angle';
				if (type === this._measureListener.getMeasureType()) {
					var measure = {
						type: type,
						x11: value.x1pixels,
						y11: value.y1pixels,
						x12: value.x2pixels,
						y12: value.y2pixels
					};

					if (measure.type === 'angle') {
						measure.x21 = value.altx1pixels;
						measure.y21 = value.alty1pixels;
						measure.x22 = value.altx2pixels;
						measure.y22 = value.alty2pixels;
						measure.value = value.distance;
					} else {
						measure.value = value.distance;
					}

					this._measureListener.newMeasureValue(measure);
				}
			}
		}
	}, ['VirtualMicroscopeManager', 'VirtualMicroscopeDataBrowser']);
});

/*global jquery, JSON*/

$(function() {
	var AnnotationMeasureManager = function(element, menuManager, dataBrowser, whiteboard, measureId) {
		this._element = $(element);
		this._startButton = this._element.find('button');
		this._measureId = measureId;
		this._menuManager = menuManager;
		this._dataBrowser = dataBrowser;
		this._whiteboard = whiteboard;

		this._baseStyleColors = {blue: '#0000ff', red: '#ff0000', yellow: '#ffff00', green: '#00ff00'};
		this._baseStyle = {fill: 'none', 'stroke-width': 20, 'stroke-linecap': 'round', 'stroke-linejoin': 'round'};

		this._shapes = [];
		this._editingInfo = null;

		this._dataBrowser.registerPaintFeature(measureId, 'Annotation', this);

		var self = this;
		this._startButton.click(function() {
			self._startAnnotation();
			return false;
		});
	};

	AnnotationMeasureManager.prototype.setServiceDelegate = function(delegate) {
		this._serviceDelegate = delegate;
	};

	AnnotationMeasureManager.prototype.initMeasureValue = function(value) {
		this._shapes = this._parseShapes(value);
	};

	AnnotationMeasureManager.prototype._parseShapes = function(value) {
		try {
			return JSON.parse(value);
		} catch (e) {
			return [];
		}
	};


	AnnotationMeasureManager.prototype.clearValue = function() {
		this._serviceDelegate.saveData('');
		this._shapes = [];
	};

	AnnotationMeasureManager.prototype._updatePaint = function() {
		this._dataBrowser.updateCurrentDataFeaturePaint(this._measureId);
	};

	AnnotationMeasureManager.prototype.createPaintShape = function(data, options) {

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


	AnnotationMeasureManager.prototype.stopUserDelayProcess = function() {
		/* since this is not cancel, accept temporal data and close */
		this._saveAndStop();
	};

	AnnotationMeasureManager.prototype._saveAndStop = function() {
		this._shapes = this._editingInfo.shapes;
		this._serviceDelegate.saveData(JSON.stringify(this._shapes), true);
		this._stopInput();
	};

	AnnotationMeasureManager.prototype._stopInput = function() {
		this._editingInfo = null;
		this._whiteboard.stopPathCapture();
		this._menuManager.closeMenu();
		this._serviceDelegate.userDelayProcessStopped();
		this._startButton.removeAttr('disabled');
	};

	AnnotationMeasureManager.prototype._startAnnotation = function() {
		this._editingInfo = {
			shapes: jQuery.extend(true, [], this._shapes),
			currentShape: -1,
			mode: null
		};
		this._tempShapesCurrentShape = -1;

		this._startButton.attr('disabled', 'disabled');
		var self = this;
		
		var callback = function() {
			self._menuManager.openMenu(function(event, data) {
				self._processMenuEvent(event, data);
			});
			self._whiteboard.captureUserInputPath(function(event, point) {
				self._processPaintUserInput(event, point);
			});
		};
		
		this._serviceDelegate.requestUserDelayProcessStart(callback);
	};

	AnnotationMeasureManager.prototype._processMenuEvent = function(event, data) {
		switch (event) {
			case 'mode':
				this._editingInfo.mode = data;
				break;
			case 'done':
				this._saveAndStop();
				break;
			case 'cancel':
				this._stopInput();
				this._updatePaint();
				break;
		}
	};



	AnnotationMeasureManager.prototype._processPaintUserInput = function(action, point) {
		switch (action) {
			case 'dragstart':
				this._editingInfo.initialPoint = [point.x, point.y];
				this._editingInfo.currentShape = -1;
				break;
			case 'enddrag':
				this._editingInfo.initialPoint = null;
				this._editingInfo.currentShape = -1;
				break;
			case 'drag':
				var modified = false;
				if (this._editingInfo.mode === 'erase') {
					var ep0 = this._editingInfo.initialPoint;
					var ep1 = [point.x, point.y];
					for (var i = this._editingInfo.shapes.length - 1; i >= 0; i--) {
						var points = this._editingInfo.shapes[i].points;
						for (var j = 1; j < points.length; j++) {
							if (this._intersects(ep0, ep1, points[j - 1], points[j])) {
								this._editingInfo.shapes.splice(i, 1);
								modified = true;
								break;
							}
						}
					}
					this._editingInfo.initialPoint = ep1;
				} else {
					if (this._editingInfo.currentShape < 0 && this._editingInfo.initialPoint) {
						this._editingInfo.currentShape = this._editingInfo.shapes.length;
						this._editingInfo.shapes.push({points: [this._editingInfo.initialPoint], color: this._editingInfo.mode});
					}

					if (this._editingInfo.currentShape >= 0) {
						this._editingInfo.shapes[this._editingInfo.currentShape].points.push([point.x, point.y]);
						modified = true;
					}
				}

				if (modified) {
					this._updatePaint();
					this._serviceDelegate.dataChanged();
				}
				break;
		}
	};

	AnnotationMeasureManager.prototype._intersects = function(a, b, c, d) {
		return this._lineSegmentIntersect(a, b, c, d) &&
						this._lineSegmentIntersect(c, d, a, b);
	};
	AnnotationMeasureManager.prototype._lineSegmentIntersect = function(a, b, c, d) {
		var AB = this._segment(a, b);
		var AC = this._segment(a, c);
		var AD = this._segment(a, d);
		return this._cross(AB, AC) * this._cross(AB, AD) <= 0;
	};
	AnnotationMeasureManager.prototype._segment = function(a, b) {
		return [b[0] - a[0], b[1] - a[1]];
	};
	AnnotationMeasureManager.prototype._cross = function(A, B) {
		return A[0] * B[1] - A[1] * B[0];
	};


	nQuireJsSupport.register('VirtualMicroscopeAnnotationMeasure', {
		init: function(dependencies) {
			this._createPaintMenu();
			this._listener = null;

			var self = this;

			$('div[measure_type="measure_widget_vm_annotation"]').each(function() {
				var inputElementId = $(this).attr('input_element_id');
				var manager = new AnnotationMeasureManager(this, self, dependencies.VirtualMicroscopeDataBrowser, dependencies.VirtualMicroscopeWhiteboard, inputElementId);
				dependencies.DynamicMeasureService.registerMeasure(inputElementId, manager);
			});
		},
		openMenu: function(listener) {
			this._listener = listener;
			this._menuContainer.removeClass('virtual-microscope-annotation-menu-disabled');
			if (this._listener) {
				this._listener('mode', this._mode);
			}
		},
		closeMenu: function() {
			this._menuContainer.addClass('virtual-microscope-annotation-menu-disabled');
			this._listener = null;
		},
		_createPaintMenu: function() {
			this._menuContainer = $('<div>').addClass('virtual-microscope-annotation-menu virtual-microscope-annotation-menu-disabled').appendTo('#virtual_microscope_container');
			var colors = ['blue', 'green', 'yellow', 'red'];
			for (var i in colors) {
				$('<div>').addClass('virtual-microscope-annotation-mode-selector virtual-microscope-annotation-color virtual-microscope-annotation-color-' + colors[i])
								.attr('mode', colors[i])
								.appendTo(this._menuContainer);
			}

			$('<div>').addClass('virtual-microscope-annotation-mode-selector virtual-microscope-annotation-erase')
							.attr('mode', 'erase')
							.appendTo(this._menuContainer);

			var self = this;
			this._menuContainer.find('.virtual-microscope-annotation-mode-selector').click(function() {
				self._setMode($(this).attr('mode'));
			});

			var buttonContainer = $('<div>').addClass('virtual-microscope-annotation-buttons-container').appendTo(this._menuContainer);

			$('<button>').html('Cancel').appendTo(buttonContainer).click(function() {
				if (self._listener) {
					self._listener('cancel');
				}
				return false;
			});

			this._setMode('blue');
		},
		_setMode: function(mode) {
			if (this._mode !== mode) {
				this._mode = mode;
				this._menuContainer.find('.virtual-microscope-annotation-mode-selector').each(function() {
					var element = $(this);
					if (element.attr('mode') === mode) {
						element.addClass('virtual-microscope-annotation-mode-selected');
					} else {
						element.removeClass('virtual-microscope-annotation-mode-selected');
					}
				});
				if (this._listener) {
					this._listener('mode', mode);
				}
			}
		}
	}, ['DynamicMeasureService', 'VirtualMicroscopeDataBrowser', 'VirtualMicroscopeWhiteboard']);
});



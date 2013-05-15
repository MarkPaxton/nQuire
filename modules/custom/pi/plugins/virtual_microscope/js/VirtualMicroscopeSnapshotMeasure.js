

$(function() {
	var SnapshotMeasureManager = function(element, vmManager, dataBrowser, measureId) {
		this._element = $(element);
		this._measureId = measureId;
		this._vmManager = vmManager;
		this._dataBrowser = dataBrowser;

		this._vmIsAdjusting = false;
		this._currentPos = null;

		this._snapshotCanvas = null;

		var self = this;
		vmManager.addPositionListener(function(position) {
			self._currentPos = position;
			if (!self._vmIsAdjusting && self._serviceDelegate) {
				self._serviceDelegate.dataChanged();
			}
		});

		this._dataBrowser.initVirtualMicroscopeViewMeasureHandler(this);
	};

	SnapshotMeasureManager.prototype.setServiceDelegate = function(delegate) {
		this._serviceDelegate = delegate;
	};

	SnapshotMeasureManager.prototype.initMeasureValue = function(value) {
		var self = this;
		this._vmIsAdjusting = true;
		clearTimeout(this._vmAdjustTimer);
		this._vmAdjustTimer = setTimeout(function() {
			self._vmIsAdjusting = false;
		}, 500);
	};

	SnapshotMeasureManager.prototype._getSnaptshotCanvas = function() {
		if (!this._snapshotCanvas) {
			var element = $('<canvas>').css({'display': 'none'}).appendTo($('body'));
			this._snapshotCanvas = element[0];
		}
		return this._snapshotCanvas;
	};

	SnapshotMeasureManager.prototype.prepareToSave = function() {
		if (this._currentPos) {
			this._serviceDelegate.randomDelayProcessStarted();
			var self = this;

			var s4end = function() {
				self._serviceDelegate.saveData(JSON.stringify(self._currentPos));
				self._serviceDelegate.randomDelayProcessStopped();
			};

			var s3drawData = function(imageSize) {
				var svg = self._dataBrowser.getCurrentDataSvg();

				svg.css({'width': imageSize.width, 'height': imageSize.height});
				var mainGroup = svg.children();
				mainGroup.attr('transform', 'scale(' + (imageSize.width / imageSize.originalWidth) + ')' + mainGroup.attr('transform'));

				var code = 'data:image/svg+xml;charset=utf-8,' + $('<div>').append(svg).html();
				self._currentPos.dataImage = code;

				s4end();
			};

			var s2getSnapshot = function() {
				self._vmManager.getSnapshot(s2processSnapshot);
			};
			var s2processSnapshot = function(snapshot) {
				var image = new Image();
				image.onload = function() {
					var imageSize = {};
					imageSize.originalWidth = parseFloat(this.width);
					imageSize.width = 200.0;
					imageSize.height = this.height * (imageSize.width / imageSize.originalWidth);
					var resizeCanvas = self._getSnaptshotCanvas();
					resizeCanvas.width = imageSize.width;
					resizeCanvas.height = imageSize.height;
					resizeCanvas.getContext("2d").drawImage(this, 0, 0, imageSize.width, imageSize.height);

					self._currentPos.vmImage = resizeCanvas.toDataURL();

					s3drawData(imageSize);
				};

				image.src = snapshot;
			};

			var s1getUrlView = function() {
				self._vmManager.getUrlView(s1processUrlView);
			};
			var s1processUrlView = function(viewUrl) {
				var k = viewUrl.lastIndexOf('?');
				self._currentPos.viewQuery = viewUrl.substr(k + 1);

				s2getSnapshot();
			};

			s1getUrlView();
		}
	};



	SnapshotMeasureManager.prototype.parseSampleFromData = function(data) {
		var view = this.parseViewFromData(data);
		return view ? view.sample : null;
	};

	SnapshotMeasureManager.prototype.parsePositionFromData = function(data) {
		var view = this.parseViewFromData(data);
		return view ? view.position : null;
	};

	SnapshotMeasureManager.prototype.parseViewFromData = function(data) {
		try {
			var value = data[this._measureId];
			var obj = JSON.parse(value);
			return obj;
		} catch (e) {
			return null;
		}
	};

	SnapshotMeasureManager.prototype.clearValue = function() {
		this._serviceDelegate.saveData('');
	};



	nQuireJsSupport.register('VirtualMicroscopeSnapshotMeasure', {
		init: function(dependencies) {
			$('div[measure_type="measure_widget_vm_snapshot"]').each(function() {
				var inputElementId = $(this).attr('input_element_id');
				var manager = new SnapshotMeasureManager(this, dependencies.VirtualMicroscopeManager, dependencies.VirtualMicroscopeDataBrowser, inputElementId);
				dependencies.DynamicMeasureService.registerMeasure(inputElementId, manager);
			});
		}
	}, ['DynamicMeasureService', 'VirtualMicroscopeManager', 'VirtualMicroscopeDataBrowser']);
});



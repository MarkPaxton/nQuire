

$(function() {
  var SnapshotMeasureManager = function(element, vmManager, dataBrowser, measureId) {
    this._element = $(element);
    this._measureId = measureId;
    this._vmManager = vmManager;

    this._vmIsAdjusting = false;
    this._currentPos = null;

    var self = this;
    vmManager.addPositionListener(function(position) {
      self._currentPos = position;
      if (!self._vmIsAdjusting && self._serviceDelegate) {
        self._serviceDelegate.dataChanged();
      }
    });
    dataBrowser.initVirtualMicroscopeViewMeasureHandler(this);
  };

  SnapshotMeasureManager.prototype.setServiceDelegate = function(delegate) {
    this._serviceDelegate = delegate;
  };

  SnapshotMeasureManager.prototype.initMeasureValue = function(value) {
    var self = this;
    this._currentPos = null;
    this._vmIsAdjusting = true;
    clearTimeout(this._vmAdjustTimer);
    this._vmAdjustTimer = setTimeout(function() {
      self._vmIsAdjusting = false;
    }, 500);
  };

  SnapshotMeasureManager.prototype.prepareToSave = function() {
    if (this._currentPos) {
      var self = this;

      this._serviceDelegate.randomDelayProcessStarted();

      var prepare = function(viewUrl) {
        var k = viewUrl.indexOf('?');
        self._currentPos.viewQuery = viewUrl.substr(k + 1);
        self._serviceDelegate.saveData(JSON.stringify(self._currentPos));
        self._serviceDelegate.randomDelayProcessStopped();
      };

      this._vmManager.getUrlView(prepare);
    }
  };

  SnapshotMeasureManager.prototype.parseViewFromData = function(data) {
    try {
      value = data[this._measureId];
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



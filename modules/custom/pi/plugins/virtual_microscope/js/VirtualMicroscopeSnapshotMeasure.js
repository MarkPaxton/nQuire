

$(function() {
  var SnapshotMeasureManager = function(element) {
    this._element = $(element);
  };

  SnapshotMeasureManager.prototype.setServiceDelegate = function(delegate) {
    this._serviceDelegate = delegate;
  };

  SnapshotMeasureManager.prototype.initMeasureValue = function(value) {

  };

  SnapshotMeasureManager.prototype.parseSampleFromValue = function(value) {
    if (typeof value === 'undefined') {
      value = this._serviceDelegate.getData();
    }

    try {
      var obj = JSON.parse(value);
      return obj.sample;
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
        var manager = new SnapshotMeasureManager(this);
        var inputElementId = $(this).attr('input_element_id');
        dependencies.DynamicMeasureService.registerMeasure(inputElementId, manager);
      });
    }
  }, ['DynamicMeasureService']);
});



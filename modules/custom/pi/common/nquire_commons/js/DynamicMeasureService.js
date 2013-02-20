
/*global nQuireJsSupport*/

$(function() {
  var DynamicMeasureServiceDelegate = function(service, elementId) {
    this._service = service;
    this._elementId = elementId;
  };

  DynamicMeasureServiceDelegate.prototype.saveData = function(data) {
    this._service._saveData(this._elementId, data);
  };

  DynamicMeasureServiceDelegate.prototype.getData = function() {
    return this._service._getData(this._elementId);
  };

  nQuireJsSupport.register('DynamicMeasureService', {
    _measureHandlers: {},
    init: function() {
    },
    registerMeasure: function(elementId, handler) {
      this._measureHandlers[elementId] = handler;
      handler.setServiceDelegate(new DynamicMeasureServiceDelegate(this, elementId));
      handler.initMeasureValue(this._getData(elementId));
    },
    _saveData: function(elementId, data) {

    },
    _getData: function(elementId) {
      return $('input#' + elementId).val();
    }
  });
});


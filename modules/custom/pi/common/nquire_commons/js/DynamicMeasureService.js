
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
  
  DynamicMeasureServiceDelegate.prototype.startRandomDelayProcess = function() {
  };

  DynamicMeasureServiceDelegate.prototype.stopRandomDelayProcess = function() {
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
      $('input[name="' + elementId + '"]').val(data);
    },
    _getData: function(elementId) {
      return $('input[name="' + elementId + '"]').val();
    }
  });
});


(function($) {
  var methods = {
    init: function(options) {
      this.addClass('dynamic_measure_link');
      if (options.disabled) {
        this.addClass('dynamic_measure_link_disabled');
      }

      this.click(function() {
        if (!$(this).hasClass('dynamic_measure_link_disabled')) {
          options.callback();
        }
        return false;
      });
      return this;
    },
    enable: function() {
      this.removeClass('dynamic_measure_link_disabled');
    },
    disable: function() {
      this.addClass('dynamic_measure_link_disabled');
    }
  };

  $.fn.nQuireDynamicMeasureLink = function(method) {
    if (methods[method]) {
      return methods[method].apply(this, Array.prototype.slice.call(arguments, 1));
    } else if (typeof method === 'object' || !method) {
      return methods.init.apply(this, arguments);
    } else {
      console.log('Method ' + method + ' does not exist on jQuery.nQuireDynamicMeasureLink');
      return false;
    }
  };
})(jQuery);



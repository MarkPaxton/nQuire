




(function($) {

  var methods = {
    init: function(options) {
      if (options) {
        if (options.defaultValue && this.nQuireWidget('getDataValue').length === 0) {
          this.nQuireWidget('setDataValue', options.defaultValue);
        }
      }
    },
    _getDataContainerName: function() {
      return this.attr('data-element-name');
    },
    _getDataContainer: function() {
      return $('input[name="' + this.nQuireWidget('_getDataContainerName') + '"]');
    },
    getDataValue: function(type) {
      var value = this.nQuireWidget('_getDataContainer').val();
      if (type === 'object') {
        return JSON.parse(value);
      } else {
        return value;
      }
    },
    setDataValue: function(value) {
      if (typeof value === 'array' || typeof value === 'object') {
          value = JSON.stringify(value);
      }
      this.nQuireWidget('_getDataContainer').val(value);
      return this;
    }
  };

  $.fn.nQuireWidget = function(method) {
    if (methods[method]) {
      return methods[method].apply(this, Array.prototype.slice.call(arguments, 1));
    } else if (typeof method === 'object' || !method) {
      return methods.init.apply(this, arguments);
    } else {
      console.log('Method ' + method + ' does not exist on jQuery.nQuireWidget');
      return false;
    }
  };
})(jQuery);

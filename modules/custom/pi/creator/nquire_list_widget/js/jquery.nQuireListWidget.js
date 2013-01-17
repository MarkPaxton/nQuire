
(function($) {

  var methods = {
    init: function() {
      this.nQuireWidget({
        defaultValue: '[]'
      });

      this.addClass('inquiry-structure-container');


      var self = this;

      this.find('a').click(function(event) {
        self.nQuireListWidget('_createItem');
        self.nQuireListWidget('_dataModified');
        event.preventDefault();
        event.stopPropagation();
      });

      this.nQuireListWidget('_initData');
    },
    _initData: function() {
      var originalData = this.nQuireWidget('getDataValue', 'object');

      if (originalData.length > 0) {
        for (var i in originalData) {
          this.nQuireListWidget('_createItem', originalData[i]);
        }
      } else {
        this.nQuireListWidget('_createItem');
      }
    },
    _createItem: function(value) {
      var self = this;
      $('<input>').attr('type', 'text').appendTo($('<li>').appendTo(this.find('ul')))
              .keypressed(function() {
        self.nQuireListWidget('_dataModified');
      });
      return this;
    },
    _dataModified: function() {
      var data = [];
      this.find('li').each(function() {
        var li = $(this);
        var value = li.find('input').val();
        if (value.length > 0) {
          data.push(value);
        }
      });

      this.nQuireWidget('setDataValue', data);
      return this;
    }
  };

  $.fn.nQuireListWidget = function(method) {
    if (methods[method]) {
      return methods[method].apply(this, Array.prototype.slice.call(arguments, 1));
    } else if (typeof method === 'object' || !method) {
      return methods.init.apply(this, arguments);
    } else {
      console.log('Method ' + method + ' does not exist on jQuery.nQuireListWidget');
      return false;
    }
  };
})(jQuery);

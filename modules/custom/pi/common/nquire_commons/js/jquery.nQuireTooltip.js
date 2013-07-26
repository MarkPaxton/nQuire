


(function($) {
  var methods = {
    init: function(options) {
      var tooltip = this.nQuireTooltip('_getTooltip');

      var content = tooltip.find('.nquire-tooltip-content');
      content.html('');

      tooltip.removeClass('nquire-tooltip-hidden');

      if (options && options.creationCallback) {
        options.creationCallback(content);
      }

      this.nQuireTooltip('_position', tooltip);

      return this;
    },
    close: function() {
      var tooltip = this.nQuireTooltip('_getTooltip');
      tooltip.addClass('nquire-tooltip-hidden');
      return this;
    },
    _getTooltip: function() {
      var tooltip = $('#nquire-tooltip');
      if (tooltip.length === 0) {
        tooltip = $('<div>').attr('id', 'nquire-tooltip').addClass('nquire-tooltip nquire-tooltip-hidden').appendTo($('body'));
        $('html').click(function() {
          tooltip.addClass('nquire-tooltip-hidden');
        });
        tooltip.click(function(event) {
          event.stopPropagation();
          event.preventDefault();
        });

        tooltip.append($('<div>').addClass('nquire-tooltip-content'));
      }
      return tooltip;
    },
    reposition: function() {
      this.nQuireTooltip('_position', $('#nquire-tooltip'));
    },
    _position: function(tooltip) {
      var pos = this.offset();
      var params = [['top', 'height', true, true], ['left', 'width', false, false]];
      var margin = 5;

      for (var i in params) {
        var posParam = params[i][0];
        var sizeParam = params[i][1];
        var before = params[i][2];
        var fromOposite = params[i][3];

        var ePos = this.offset()[posParam];
        var eSize = this[sizeParam]();
        var tSize = tooltip[sizeParam]();

        var tPos = ePos;
        if (before) {
          tPos -= tSize;
          if (fromOposite) {
            tPos += eSize;
          } else {
            tPos -= margin;
          }
        } else if (!fromOposite) {
          tPos += eSize + margin;
        }

        tooltip.css(posParam, tPos + 'px');
      }
      return this;
    }
  };

  $.fn.nQuireTooltip = function(method) {
    if (methods[method]) {
      return methods[method].apply(this, Array.prototype.slice.call(arguments, 1));
    } else if (typeof method === 'object' || !method) {
      return methods.init.apply(this, arguments);
    } else {
      console.log('Method ' + method + ' does not exist on jQuery.nQuireTooltip');
      return false;
    }
  };
})(jQuery);



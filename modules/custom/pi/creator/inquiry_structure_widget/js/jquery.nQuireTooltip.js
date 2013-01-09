


(function($) {
  var methods = {
    init: function(options) {
      var tooltip = this.nQuireTooltip('_getTooltip');

      var content = tooltip.find('.nquire-tooltip-content');
      content.html('');
      if (options && options.creationCallback) {
        options.creationCallback(content);
      }

      tooltip.removeClass('nquire-tooltip-hidden');
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
    close: function() {
      var tooltip = this.nQuireTooltip('_getTooltip');
      tooltip.addClass('nquire-tooltip-hidden');
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



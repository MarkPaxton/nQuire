
(function($) {

	var methods = {
		init: function(options) {
			var _options = $.extend({
				selectionCallback: false,
				closeCallback: false,
				defaultValue: null
			}, options);

			var self = this;
			var picker = $('#rock-color-picker-container');


			self.data('rock-color-picker-user-selection-callbacks', []);
			self.data('rock-color-picker-selection-callback', _options.selectionCallback);
			self.data('rock-color-picker-close-callback', _options.closeCallback);

			$('#rock-color-picker-container').each(function() {
				$(this).remove().appendTo(self);
			});

			picker.css({
				top: .5 * ($(window).height() - picker.height()),
				left: .5 * ($(window).width() - picker.width())
			});

			picker.customMouseInput('sizing', function(k, x, y, dx, dy) {
				var w = picker.width();
				var neww = Math.max(200, Math.min(2000, k * w));
				var appliedk = neww / w;
				var newh = appliedk * picker.height();
				var pos = picker.position();

				var newstate = {
					left: x - appliedk * (x - pos.left) - $(window).scrollLeft() + dx,
					top: y - appliedk * (y - pos.top) - $(window).scrollTop() + dy,
					width: neww,
					height: newh
				};

				picker.css(newstate);
			});

			picker.customMouseInput('move', function(deltaX, deltaY) {
				var margin = 100;
				var w = picker.width();
				var h = picker.height();
				var ww = $(window).width();
				var wh = $(window).height();
				var pos = picker.offset();
				pos.left = Math.max(margin - w, Math.min(ww - margin, pos.left + deltaX - $(window).scrollLeft()));
				pos.top = Math.max(margin - h, Math.min(wh - margin, pos.top + deltaY - $(window).scrollTop()));

				picker.css(pos);
			});

			$('.rock-color-picker-chip').customMouseInput('click', function(element) {
				self.rockColorPicker('_setValue', $(element).attr('color-id'), true);
				self.rockColorPicker('close');
			});

			$('#rock-color-picker-close').customMouseInput('click', function() {
				self.rockColorPicker('close');
				if (self.data('rock-color-picker-close-callback')) {
					self.data('rock-color-picker-close-callback')();
				}
			});

			this.rockColorPicker('_setValue', _options.defaultValue, false);
		},
		open: function() {
			$('#rock-color-picker-container').fadeIn();
			$('#rock-color-picker-container').removeClass('hidden');
		},
		close: function() {
			$('#rock-color-picker-container').fadeOut();
		},
		toggle: function() {
			if ($('#rock-color-picker-container').is(':visible')) {
				this.rockColorPicker('close');
			} else {
				this.rockColorPicker('open');
			}
			return this;
		},
		_setValue: function(value, userAction) {
			this.data('rock-color-picker-selected', value);
			$('.rock-color-picker-selection-border').hide();
			
			if (value) {
				$('.rock-color-picker-chip[color-id="' + value + '"] > .rock-color-picker-selection-border').show();
			} 

			if (userAction) {
				(this.data('rock-color-picker-selection-callback'))(value);
			}
		},
		clearSelection: function(userAction) {
			this.rockColorPicker('_setValue', null, userAction);
			this.rockColorPicker('close');
		},
		select: function(value) {
			this.rockColorPicker('_setValue', value, false);
		}
	};

	$.fn.rockColorPicker = function(method) {
		if (methods[method]) {
			return methods[method].apply(this, Array.prototype.slice.call(arguments, 1));
		} else if (typeof method === 'object' || !method) {
			return methods.init.apply(this, arguments);
		} else {
			console.log('Method ' + method + ' does not exist on jQuery.rockColorPicker');
			return false;
		}
	};
})(jQuery);
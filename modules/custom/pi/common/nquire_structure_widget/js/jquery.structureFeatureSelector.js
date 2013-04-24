/* 
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

(function($) {

	var methods = {
		init: function(options) {
			var _options = $.extend({
				multiple: false,
				columnValues: [],
				defaultValueForItem: false,
				validValuesForItem: false,
				defaultValue: '0',
				forItemType: 'all'
			}, options);

			var defaultValue = options.columnValues.length > 0 ?
							options.columnValues[0].id : null;

			this.nQuireWidget({
				defaultValue: defaultValue
			});

			this.data('options', _options);
			
			var value = this.nQuireWidget('getDataValue', 'object');
			for (var i in options.columnValues) {
				this.structureFeatureSelector('_buildColumn', options.columnValues[i], options.columnValues[i].id == value);
			}

			this.structureFeatureSelector('_enableSaveButton', false);
		},
		_buildColumn: function(column, checked) {
			var options = this.data('options');

			var button = $('<input type="radio" name="column" value="' + column.id + '"/>');
			if (checked) {
				button.attr('checked', 'checked');
			}
			
			var self = this;
			button.change(function() {
				self.structureFeatureSelector('_dataModified', $(this).attr('value'));
			});
			
			var headCell = $('<td>').addClass('feature-cell').attr('column-id', column.id);
			var cellContainer = $('<div>').addClass('feature-title').appendTo(headCell);
			cellContainer.append(button).append(column.title);

			this.find('tr.structure-feature-list').append(headCell);


			this.find('tr').not('.structure-feature-list').each(function() {
				var row = $(this);

				var valueCell = $('<td>').addClass('value-cell').attr('column-id', column.id);

				if (options.forItemType === 'all' || options.forItemType === row.attr('type')) {
					var itemId = row.attr('item-id');
					valueCell.append(options.labels[column.values[itemId]]);
				}

				row.append(valueCell);
			});

			this.structureFeatureSelector('_updateOddEvenClasses');
		},
		_dataModified: function(data) {
			this.nQuireWidget('setDataValue', data);
			this.structureFeatureSelector('_enableSaveButton', true);
		},
		_enableSaveButton: function(enabled) {
			var button = $('#edit-submit');
			if (enabled) {
				button.removeAttr('disabled');
			} else {
				button.attr('disabled', 'disabled');
			}
			return this;
		},
		_updateOddEvenClasses: function() {
			var iOdd = false;

			this.find('.structure-feature-list').each(function() {
				var cOdd = false;
				$(this).find('.feature-cell').each(function() {
					$(this).removeClass().addClass('feature-cell')
									.addClass(cOdd ? 'column-odd' : 'column-even');
					cOdd = !cOdd;
				});
			});

			this.find('.structure-tablerow').each(function() {
				var row = $(this);
				var cOdd = false;
				var active = row.find('.structure-tablecell-active-item').length > 0;
				var itemClass;

				if (active) {
					itemClass = iOdd ? 'item-odd' : 'item-even';
					iOdd = !iOdd;
				}

				$(this).find('.value-cell').each(function() {
					var cell = $(this);

					cell.removeClass().addClass('value-cell')
									.addClass(cOdd ? 'column-odd' : 'column-even');

					if (active) {
						cell.addClass(itemClass);
					}
					cOdd = !cOdd;
				});
			});
		}
	};

	$.fn.structureFeatureSelector = function(method) {
		if (methods[method]) {
			return methods[method].apply(this, Array.prototype.slice.call(arguments, 1));
		} else if (typeof method === 'object' || !method) {
			return methods.init.apply(this, arguments);
		} else {
			console.log('Method ' + method + ' does not exist on jQuery.structureFeatureWidget');
			return false;
		}
	};
})(jQuery);

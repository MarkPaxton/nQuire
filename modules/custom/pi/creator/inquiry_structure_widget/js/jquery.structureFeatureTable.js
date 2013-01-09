/* 
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

(function($) {

  var methods = {
    init: function(options) {
      var _options = $.extend({
        allowCreate: true,
        newButtonLabel: 'Add',
        newDialogTitle: 'Add new column',
        renameDialogTitle: 'Rename column',
        deleteDialogTitle: 'Delete column',
        freeTextColumn: true,
        allowEmpty: true,
        defaultColumn: 'new',
        values: {0: '0'},
        defaultValue: '0',
        forItemType: 'all'
      }, options);

      var self = this;

      var defaultValue = [];
      if (!_options.allowEmpty) {
        defaultValue.push({id: 'new0', title: _options.defaultColumn, values: {}});
      }

      this.nQuireWidget({
        defaultValue: defaultValue
      });

      if (_options.allowCreate) {
        var button = this.structureFeatureTable('_createLink', 'new', _options.newButtonLabel, false);
        var row = this.find('tr.structure-feature-list');
        $('<td class="new-feature-cell">').appendTo(row).append(button);
        button.click(function(event) {
          self.structureFeatureTable('_openAddNewColumnDialog');
          event.stopPropagation();
          event.preventDefault();
        });

        this.find('tr').not('.structure-feature-list').each(function() {
          $(this).append($('<td>'));
        });
      }
      this.data('nextNewId', 1);
      this.data('options', _options);


      var value = this.nQuireWidget('getDataValue', 'object');
      for (i in value) {
        this.structureFeatureTable('_buildColumn', value[i]);
      }

    },
    _createLink: function(op, title, inline) {
      return $('<a>').attr('op', op).html(title).attr('href', '#').addClass(inline ? 'link-button' : 'normal-link');
    },
    _buildColumn: function(column) {
      var options = this.data('options');
      var self = this;

      var headCell = $('<td>').addClass('feature-cell').attr('column-id', column.id).html(column.title);

      this.find('tr.structure-feature-list').children(':last').before(headCell);

      this.find('tr').not('.structure-feature-list').each(function() {
        var row = $(this);
        var valueCell = $('<td>');

        if (options.forItemType === 'all' || options.forItemType === row.attr('type')) {
          var itemId = row.attr('item-id');
          var value = typeof column.values[itemId] !== 'undefined' ?
                  column.values[itemId] : options.defaultValue;

          valueCell.addClass('value-cell')
                  .attr('column-id', column.id).attr('item-id', itemId).attr('value', value);
          self.structureFeatureTable('_createLink', 'value', options.values[value], false).click(function(event) {
            var cell = $(this).parent();
            self.structureFeatureTable('_openValueDialog', cell);
            event.preventDefault();
            event.stopPropagation();
          }).appendTo(valueCell);
        }

        row.children(':last').before(valueCell);
      });
    },
    _openAddNewColumnDialog: function() {
      var self = this;
      var options = this.data('options');

      this.structureFeatureTable('_openColumnDialog', null, options.newDialogTitle, true, '_addNewColumnDialog');
    },
    _addNewColumnDialog: function(id, title) {
      if (title.length > 0) {
        var n = this.data('nextNewId');
        this.data('nextNewId', n + 1);
        this.structureFeatureTable('_buildColumn', {id: 'new' + n, title: title, values: {}});
      }
    },
    _openColumnDialog: function(title, columnId, useInput, callback) {
      var self = this;

      this.find('.new-feature-cell > a').nQuireTooltip({
        creationCallback: function(content) {
          content.append($('<p>').html(title));

          if (useInput) {
            var input = $('<input>').attr('id', 'new-column-input').appendTo(content);
            input.keypress(function(event) {
              if (event.which === 13) {
                event.preventDefault();
                event.stopPropagation();
                self.structureFeatureTable(callback, columnId, $(this).val());
                self.nQuireTooltip('close');
              }
            });
          }

          var buttons = $('<div>').appendTo(content);
          var create = $('<button>').html('Ok').appendTo(buttons);
          var cancel = $('<button>').html('Cancel').appendTo(buttons);

          create.click(function() {
            self.structureFeatureTable(callback, columnId, input ? input.val() : null);
            self.nQuireTooltip('close');
          });

          cancel.click(function() {
            self.nQuireTooltip('close');
          });
        }
      });
    },
    _openValueDialog: function(cell) {
      var self = this;
      var options = this.data('options');

      cell.nQuireTooltip({
        creationCallback: function(content) {
          var ul = $('<ul>').appendTo(content);
          for (var key in options.values) {
            var link = self.structureFeatureTable('_createLink', 'select', options.values[key], false)
                    .attr('value', key)
                    .click(function() {
              self.structureFeatureTable('_changeValue', cell.attr('column-id'), cell.attr('item-id'), $(this).attr('value'));
              self.nQuireTooltip('close');
            });
            $('<li>').appendTo(ul).append(link);
          }
        }
      });
    },
    _changeValue: function(columnId, itemId, value) {
      var options = this.data('options');
      var cell = $('td[column-id="' + columnId + '"][item-id="' + itemId + '"]');
      cell.attr('value', value);
      cell.find('a').html(options.values[value]);
    }

  };

  $.fn.structureFeatureTable = function(method) {
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

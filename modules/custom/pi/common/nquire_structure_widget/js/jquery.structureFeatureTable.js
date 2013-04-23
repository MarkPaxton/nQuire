/* 
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

(function($) {

  var methods = {
    init: function(options) {
      var _options = $.extend({
        allowCreate: true,
        canRename: true,
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
        var button = this.structureFeatureTable('_createLink', _options.newButtonLabel, false);
        var row = this.find('tr.structure-feature-list');
        $('<td class="new-feature-cell">').appendTo(row).append(button);
        button.click(function(event) {
          self.structureFeatureTable('_openColumnDialog', button, _options.newDialogTitle, null, '_addNewColumn', true);
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

      this.structureFeatureTable('_enableSaveButton', false);
    },
    _createLink: function(title, inline) {
      return $('<a>').html(title).attr('href', '#').addClass(inline ? 'link-button' : 'normal-link');
    },
    _updateFeatureButtons: function() {
      var self = this;
      var options = this.data('options');
      var buttonElements = this.find('.feature-buttons');

      var canDelete = options.allowEmpty || buttonElements.length > 1;

      buttonElements.each(function() {
        var element = $(this);

        element.empty();
        if (options.canRename) {
          self.structureFeatureTable('_createLink', 'rename', true).attr('op', 'rename').appendTo(element);
        }

        if (canDelete) {
          if (options.canRename) {
            element.append('&nbsp;&nbsp;');
          }
          self.structureFeatureTable('_createLink', 'delete', true).attr('op', 'delete').appendTo(element);
        }
      });

      this.find('.feature-buttons > a').click(function(event) {

        var button = $(this);
        var op = button.attr('op');
        var parent = button.parents('.feature-cell');
        var columnId = parent.attr('column-id');

        if (op === 'delete') {
          self.structureFeatureTable('_openColumnDialog', button, options.deleteDialogTitle, columnId, '_removeColumn', false);
        } else if (op === 'rename') {
          var title = parent.children('.feature-title').html();
          self.structureFeatureTable('_openColumnDialog', button, options.renameDialogTitle, columnId, '_renameColumn', true, title);
        }

        event.preventDefault();
        event.stopPropagation();
      });
    },
    _removeColumn: function(id) {
      this.find('td[column-id="' + id + '"]').remove();
      this.structureFeatureTable('_updateFeatureButtons');
      this.structureFeatureTable('_updateOddEvenClasses');
      this.structureFeatureTable('_dataModified');
    },
    _renameColumn: function(id, title) {
      this.find('td.feature-cell[column-id="' + id + '"] > div.feature-title').html(title);
      this.structureFeatureTable('_dataModified');
    },
    _buildColumn: function(column) {
      var options = this.data('options');
      var self = this;

      var headCell = $('<td>').addClass('feature-cell').attr('column-id', column.id);
      headCell
              .append($('<div>').addClass('feature-title').html(column.title))
              .append($('<div>').addClass('feature-buttons'));

      if (options.allowCreate) {
        this.find('tr.structure-feature-list').children(':last').before(headCell);
      } else {
        this.find('tr.structure-feature-list').append(headCell);
      }

      this.find('tr').not('.structure-feature-list').each(function() {
        var row = $(this);
        var valueCell = $('<td>').addClass('value-cell').attr('column-id', column.id);

        if (options.forItemType === 'all' || options.forItemType === row.attr('type')) {
          var itemId = row.attr('item-id');
          var value = typeof column.values[itemId] !== 'undefined' ?
                  column.values[itemId] : options.defaultValue;

          valueCell.attr('item-id', itemId).attr('value', value);

          self.structureFeatureTable('_createLink', options.values[value], false).click(function(event) {
            var cell = $(this).parent();
            self.structureFeatureTable('_openValueDialog', cell);
            event.preventDefault();
            event.stopPropagation();
          }).appendTo(valueCell);
        }

        if (options.allowCreate) {
          row.children(':last').before(valueCell);
        } else {
          row.append(valueCell);
        }
      });

      this.structureFeatureTable('_updateOddEvenClasses');
      this.structureFeatureTable('_updateFeatureButtons');
    },
    _addNewColumn: function(id, title) {
      if (title.length > 0) {
        var n = this.data('nextNewId');
        this.data('nextNewId', n + 1);
        this.structureFeatureTable('_buildColumn', {id: 'new' + n, title: title, values: {}});
        this.structureFeatureTable('_dataModified');
      }
    },
    _openColumnDialog: function(element, title, columnId, callback, useInput, inputValue) {
      var self = this;

      element.nQuireTooltip({
        creationCallback: function(content) {
          content.append($('<p>').html(title));

          if (useInput) {
            var input = $('<input>').attr('id', 'new-column-input').appendTo(content);
            if (inputValue) {
              input.val(inputValue);
            }
            
            input.focus();

            input.keypress(function(event) {
              if (event.which === 13) {
                event.preventDefault();
                event.stopPropagation();
                self.structureFeatureTable(callback, columnId, $(this).val());
                self.nQuireTooltip('close');
              }
            });
          }

          var buttons = $('<div>').addClass('tooltip-buttons').appendTo(content);
          var ok = $('<button>').html('Ok').appendTo(buttons);
          var cancel = $('<button>').html('Cancel').appendTo(buttons);

          ok.click(function() {
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
      var currentValue = cell.attr('value');

      cell.find('a').nQuireTooltip({
        creationCallback: function(content) {
          for (var key in options.values) {
            var element;
            var row = $('<div>').addClass('feature-value-option').appendTo(content);
            row.append($('<div>').addClass('nQuire-tooltip-bullet-leaf'));
            if (key !== currentValue) {
              element = self.structureFeatureTable('_createLink', options.values[key], false)
                      .attr('value', key)
                      .click(function() {
                self.structureFeatureTable('_changeValue', cell.attr('column-id'), cell.attr('item-id'), $(this).attr('value'));
                self.nQuireTooltip('close');
              });
            } else {
              element = $('<div>').html(options.values[key]);
            }
            row.append(element);
          }
        }
      });
    },
    _changeValue: function(columnId, itemId, value) {
      var options = this.data('options');
      var cell = $('td[column-id="' + columnId + '"][item-id="' + itemId + '"]');
      cell.attr('value', value);
      cell.find('a').html(options.values[value]);
      this.structureFeatureTable('_dataModified');
    },
    _dataModified: function() {
      var self = this;

      var data = [];

      this.find('td.feature-cell').each(function() {
        var cell = $(this);
        var id = cell.attr('column-id');
        var column = {
          id: id,
          title: cell.children('.feature-title').html(),
          values: {}
        };
        self.find('td.value-cell[column-id="' + id + '"][item-id]').each(function() {
          var valueCell = $(this);
          column.values[valueCell.attr('item-id')] = valueCell.attr('value');
        });
        data.push(column);
      });

      this.nQuireWidget('setDataValue', data);
      this.structureFeatureTable('_enableSaveButton', true);
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

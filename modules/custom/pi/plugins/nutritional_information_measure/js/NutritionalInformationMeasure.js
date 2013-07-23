
/*global nQuireJsSupport, $*/

$(function() {

  var NutritionalInformationMeasureManager = function(element, data) {
    var self = this;
    this._data = data;
    this._element = $(element);
    this._tableBodyElement = this._element.find('table.food_table > tbody');
    this._addButton = this._element.find('a.add_food');
    this._addButton.nQuireDynamicMeasureLink({
      callback: function() {

        self._addButton.nQuireTooltip({
          creationCallback: function(content) {
            var c1 = $('<div style="display:inline-block;width: 250px;">').appendTo(content);
            var c2 = $('<div style="display:inline-block;width: 250px;">').appendTo(content);
            var l1 = $('<ul>').appendTo(c1);
            var l2 = $('<ul>').appendTo(c2);

            for (var key in data.foods) {
              var link = $('<a>').attr('href', '#').attr('food', key).html(data.foods[key].title);
              link.click(function() {
                self._addFood($(this).attr('food'));
                self._addButton.nQuireTooltip('close');
              });
              $('<li>').appendTo(key % 2 ? l1 : l2).append(link);
            }

            $('<a>').attr('href', '#').html('Cancel').click(function() {
              self._addButton.nQuireTooltip('close');
            }).appendTo($('<p>').appendTo(content));
          }
        });
      },
      disabled: false
    });
    this._value = null;
  };
  NutritionalInformationMeasureManager.prototype.setServiceDelegate = function(delegate) {
    this._serviceDelegate = delegate;
  };
  NutritionalInformationMeasureManager.prototype.initMeasureValue = function(value) {
    if (value && value.length > 0) {
      try {
        this._value = JSON.parse(value);
      } catch (e) {
        this._value = null;
      }
    }

    this._updateTable();
  };
  NutritionalInformationMeasureManager.prototype._addFood = function(food) {
    if (this._data.foods[food]) {
      if (!this._value) {
        this._value = {};
      }

      if (!this._value[food]) {
        this._value[food] = 1;
      } else {
        this._value[food]++;
      }

      this._updateData();
      this._updateTable();
    }
  };

  NutritionalInformationMeasureManager.prototype._removeFood = function(food) {
    if (this._value && this._value[food]) {
      this._value[food]--;
      if (this._value[food] === 0) {
        delete(this._value[food]);
      }
    }

    this._updateData();
    this._updateTable();
  };

  NutritionalInformationMeasureManager.prototype.clearValue = function() {
    this._value = null;
    this._updateData();
    this._updateTable();
  };

  NutritionalInformationMeasureManager.prototype._updateData = function() {
    var value = this._value ? JSON.stringify(this._value) : '';
    this._serviceDelegate.saveData(value);
  };

  NutritionalInformationMeasureManager.prototype._updateTable = function() {
    this._tableBodyElement.find('tr').remove();
    if (this._value) {
      var self = this;

      var total = {};
      var odd = true;
      for (var key in this._value) {
        var count = this._value[key];
        if (count > 0) {
          var foodInfo = this._data.foods[key];
          if (foodInfo) {
            var tr = $('<tr>').addClass('food_row').addClass(odd ? 'odd' : 'even').appendTo(this._tableBodyElement);
            odd = !odd;


            var title = foodInfo.title + ':<br/>' + count + ' ';
            var portion = foodInfo.portion;
            if (count > 1) {
              portion += portion.substr(portion.length - 1) === 's' ? 'es' : 's';
            }
            title += portion;

            var link = $('<a>').attr('href', '#').attr('food', key).html('delete').click(function() {
              self._removeFood($(this).attr('food'));
            });
            tr.append($('<td>').append(title).append('<br/>').append(link));

            for (var i in this._data.components) {
              var td = $('<td>').appendTo(tr);
              var amount = foodInfo.value[i];

              var compAmount = amount * count;
              if (total[i]) {
                total[i] += compAmount;
              } else {
                total[i] = compAmount;
              }
              
              compAmount += ' ' + this._data.units[i];

              if (count !== 1) {
                compAmount += '<br/>(' + amount + '*' + count + ')';
              }
              td.html(compAmount);

            }
          }
        }
      }

      var tr = $('<tr>').addClass(odd ? 'odd' : 'even').addClass('food_row').attr('style', 'font-weight: bold').appendTo(this._tableBodyElement);
      tr.append($('<td>').html('Total'));
      for (var i in this._data.components) {
        var td = $('<td>').appendTo(tr);
        td.html(total[i] + ' ' + this._data.units[i]);
      }
    }
  };

  nQuireJsSupport.register('NutritionalInformationMeasure', {
    init: function(dependencies) {
      $('div[measure_type="measure_widget__nutritionalinformation"]').each(function() {
        var manager = new NutritionalInformationMeasureManager(this, dependencies.NutritionalInformationMeasureData);
        var inputElementId = $(this).attr('input_element_id');
        dependencies.DynamicMeasureService.registerMeasure(inputElementId, manager);
      });
    }
  }, ['DynamicMeasureService', 'NutritionalInformationMeasureData']);
});



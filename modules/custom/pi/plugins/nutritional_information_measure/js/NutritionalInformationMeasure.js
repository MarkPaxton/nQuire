
/*global nQuireJsSupport, google, $*/

$(function() {

	var NutritionalInformationMeasureManager = function(element, information) {
		var self = this;
		this._information = information;
		this._element = $(element);
		this._tableBodyElement = this._element.find('table.food_table > tbody');
		this._addButton = this._element.find('a.add_food');
		this._addButton.nQuireDynamicMeasureLink({
			callback: function() {

				self._addButton.nQuireTooltip({
					creationCallback: function(content) {
						var list = $('<ul>').appendTo(content);
						for (var key in information.foods) {
							var link = $('<a>').attr('href', '#').attr('food', key).html(information.foods[key].name);
							link.click(function() {
								self._addFood($(this).attr('food'));
								self._addButton.nQuireTooltip('close');
							});
							$('<li>').appendTo(list).append(link);
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
		if (this._information.foods[food]) {
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
					var foodInfo = this._information.foods[key];
					if (foodInfo) {
						var tr = $('<tr>').addClass('food_row').addClass(odd ? 'odd' : 'even').appendTo(this._tableBodyElement);
						odd = !odd;


						var title = foodInfo.name;
						if (count !== 1) {
							title += ' (x' + count + ')';
						}
						var link = $('<a>').attr('href', '#').attr('food', key).html('delete').click(function() {
							self._removeFood($(this).attr('food'));
						});
						tr.append($('<td>').append(title).append('<br/>').append(link));

						for (var component in this._information.components) {
							var td = $('<td>').appendTo(tr);
							var amount = foodInfo.values[component];
							if (amount) {
								var compAmount = amount * count;
								if (total[component]) {
									total[component] += compAmount;
								} else {
									total[component] = compAmount;
								}

								if (count !== 1) {
									compAmount += ' (' + amount + '*' + count + ')';
								}
								td.html(compAmount);
							}
						}
					}
				}
			}

			var tr = $('<tr>').addClass(odd ? 'odd' : 'even').addClass('food_row').attr('style', 'font-weight: bold').appendTo(this._tableBodyElement);
			tr.append($('<td>').html('Total'));
			for (var component in this._information.components) {
				var td = $('<td>').appendTo(tr);
				if (total[component]) {
					td.html(total[component]);
				}
			}
		}
	};

	nQuireJsSupport.register('NutritionalInformationMeasure', {
		init: function(dependencies) {
			$('div[measure_type="measure_widget__nutritionalinformation"]').each(function() {
				var manager = new NutritionalInformationMeasureManager(this, dependencies.NutritionalInformationMeasureData.information);
				var inputElementId = $(this).attr('input_element_id');
				dependencies.DynamicMeasureService.registerMeasure(inputElementId, manager);
			});
		}
	}, ['DynamicMeasureService', 'NutritionalInformationMeasureData']);
});



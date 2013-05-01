
/*global nQuireJsSupport, google, $*/

$(function() {

  var NutritionalInformationMeasureManager = function(element) {
    var self = this;
    this._element = $(element);
    this._tableElement = this._element.find('div.meal_table');

    this._addButton = this._element.find('a.add_button');
    this._addButton.nQuireDynamicMeasureLink({
      callback: function() {
				alert('add');
//        self.setMarker();
 //       self.updateData();
      },
      disabled: false
    });
		
		this._value = null;
  };

  NutritionalInformationMeasureManager.prototype.setServiceDelegate = function(delegate) {
    this._serviceDelegate = delegate;
  };

  NutritionalInformationMeasureManager.prototype.initMeasureValue = function(value) {
    var self = this;

    this._value = null;
    if (value && value.length > 0) {
      try {
        this._value = JSON.parse(value);
      } catch (e) {
        this._value = null;
      }
    }
		
		this._updateTable();
	};

  NutritionalInformationMeasureManager.prototype.clearValue = function() {
		this._value = null;
		this._updateData();
		this._updateTable();
  };

  NutritionalInformationMeasureManager.prototype._updateData = function() {
		var value = this._value ? JSON.stringify(this._value) : '';
		this._serviceDelegate.saveData('');
  };





  nQuireJsSupport.register('NutritionalInformationMeasure', {
    init: function(dependencies) {
      $('div[measure_type="measure_widget__nutritionalinformation"]').each(function() {
        var manager = new NutritionalInformationMeasureManager(this);
        var inputElementId = $(this).attr('input_element_id');
        dependencies.DynamicMeasureService.registerMeasure(inputElementId, manager);
      });
    }
  }, ['DynamicMeasureService', 'NutritionalInformationMeasureData']);
});



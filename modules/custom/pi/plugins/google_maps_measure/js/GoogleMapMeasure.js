
/*global nQuireJsSupport, google, $*/

$(function() {

  var GoogleMapMeasureManager = function(element, measureService) {
    this._measureService = measureService;
    this._element = $(element);
    this._mapElement = this._element.find('div.map_canvas');
    this._markButton = this._element.find('a.add_marker');
    this._forgetButton = this._element.find('a.forget_marker');
    this._ready = false;
  };

  GoogleMapMeasureManager.prototype.setServiceDelegate = function(delegate) {
    this._serviceDelegate = delegate;
  };

  GoogleMapMeasureManager.prototype.initMeasureValue = function(value) {
    console.log('value: ' + value);

    var myCenter = new google.maps.LatLng(60.0, 105.0);
    var myOptions = {
      zoom: 6,
      center: myCenter,
      mapTypeId: google.maps.MapTypeId.ROADMAP
    };
    this._map = new google.maps.Map(this._mapElement[0], myOptions);
  };





  nQuireJsSupport.register('GoogleMapMeasure', {
    init: function(dependencies) {
      $('div[measureType="measure_widget__googlemap"]').each(function() {
        var manager = new GoogleMapMeasureManager(this);
        var inputElementId = $(this).attr('input-element-id');
        dependencies.DynamicMeasureService.registerMeasure(inputElementId, manager);
      });
    }
  }, ['DynamicMeasureService']);
});



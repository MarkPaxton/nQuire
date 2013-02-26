
/*global nQuireJsSupport, google, $*/

$(function() {

  var GoogleMapMeasureManager = function(element, measureService) {
    var self = this;
    this._measureService = measureService;
    this._element = $(element);
    this._mapElement = this._element.find('div.map_canvas');

    this._map = null;
    this._marker = null;
    this._elevator = null;

    this._markButton = this._element.find('a.add_marker');
    this._markButton.nQuireDynamicMeasureLink({
      callback: function() {
        self.setMarker();
        self.updateData();
      },
      disabled: true
    });
    this._forgetButton = this._element.find('a.forget_marker');
    this._forgetButton.nQuireDynamicMeasureLink({
      callback: function() {
        self.forgetMarker();
        self.updateData();
      },
      disabled: true
    });

    this._ready = false;
  };

  GoogleMapMeasureManager.prototype.setServiceDelegate = function(delegate) {
    this._serviceDelegate = delegate;
  };

  GoogleMapMeasureManager.prototype.initMeasureValue = function(value) {
    var self = this;
    console.log('value: ' + value);

    var data = null;
    var markerSet = false;
    if (value && value.length > 0) {
      try {
        data = JSON.parse(value);
        markerSet = true;
      } catch (e) {
        data = null;
      }
    }

    if (!data && $.cookie(self._element.attr('id'))) {
      data = JSON.parse($.cookie(self._element.attr('id')));
    }

    if (!data || typeof(data.latitude) === 'undefined' || typeof(data.longitude) === 'undefined' || typeof(data.zoom) === 'undefined') {
      data = {
        'latitude': 54.67500344265723,
        'longitude': -3.4570312499999734,
        'zoom': 4
      };
    }


    var myCenter = new google.maps.LatLng(data.latitude, data.longitude);
    var myOptions = {
      zoom: data.zoom,
      center: myCenter,
      mapTypeId: google.maps.MapTypeId.TERRAIN
    };
    this._map = new google.maps.Map(this._mapElement[0], myOptions);
    if (markerSet) {
      this.setMarker(data);
    }

    this._elevator = new google.maps.ElevationService();


    google.maps.event.addListener(this._map, 'bounds_changed', function() {
      var center = self._map.getCenter();
      var pos = {
        'latitude': center.lat(),
        'longitude': center.lng(),
        'zoom': self._map.getZoom()
      };

      var posStr = JSON.stringify(pos);
      $.cookie(self._element.attr('id'), posStr);
    });

    this._markButton.nQuireDynamicMeasureLink('enable');
  };

  GoogleMapMeasureManager.prototype.setMarker = function(pos) {
    var self = this;

    if (!pos) {
      var center = this._map.getCenter();
      pos = {
        'latitude': center.lat(),
        'longitude': center.lng()
      };
    }

    var latLng = new google.maps.LatLng(pos.latitude, pos.longitude);

    if (!this._marker) {
      this._marker = new google.maps.Marker({
        position: latLng,
        map: this._map,
        draggable: true
      });
      google.maps.event.addListener(this._marker, 'dragend', function() {
        self.updateData();
      });
    } else {
      this._marker.setPosition(latLng);
    }

    this._forgetButton.nQuireDynamicMeasureLink('enable');
  };

  GoogleMapMeasureManager.prototype.forgetMarker = function() {
    if (this._marker) {
      this._marker.setMap(null);
      this._marker = null;
    }
  };

  GoogleMapMeasureManager.prototype.updateData = function() {
    if (this._marker) {
      var self = this;

      var markerPos = this._marker.getPosition();
      var positionalRequest = {
        'locations': [markerPos]
      };

      var pos = {
        'latitude': markerPos.lat(),
        'longitude': markerPos.lng(),
        'zoom': this._map.getZoom()
      };


      this._serviceDelegate.randomDelayProcessStarted();
      this._elevator.getElevationForLocations(positionalRequest, function(results, status) {

        if (status === google.maps.ElevationStatus.OK && results.length === 1) {
          pos.altitude = results[0].elevation;
        }

        var value = JSON.stringify(pos);
        self._serviceDelegate.saveData(value);
        self._serviceDelegate.randomDelayProcessStopped();
      });
    } else {
      this._serviceDelegate.saveData('');
    }
  };





  nQuireJsSupport.register('GoogleMapMeasure', {
    init: function(dependencies) {
      $('div[measure_type="measure_widget__googlemap"]').each(function() {
        var manager = new GoogleMapMeasureManager(this);
        var inputElementId = $(this).attr('input_element_id');
        dependencies.DynamicMeasureService.registerMeasure(inputElementId, manager);
      });
    }
  }, ['DynamicMeasureService']);
});




/*global nQuireJsSupport, google, $*/

$(function() {

	var GoogleMapMeasureManager = function(element) {
		var self = this;
		this._element = $(element);
		this._mapElement = this._element.find('div.map_canvas');
		this._searchBox = this._element.find('input.map_search_box');

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
		this._gotoButton = this._element.find('a.goto_marker');
		this._gotoButton.nQuireDynamicMeasureLink({
			callback: function() {
				if (self._marker) {
					self._map.panTo(self._marker.getPosition());
				}
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


		var geocoder = new google.maps.Geocoder();

		var months = ['January', 'February', 'March', 'April', 'May', 'June', 'July', 'August', 'September', 'October', 'November', 'December'];
		this._searchBox.autocomplete(function(term, response) {
			geocoder.geocode({'address': term}, function(results, status) {
				if (status == google.maps.GeocoderStatus.OK) {

					var searchLoc = results[0].geometry.location;
					var lat = results[0].geometry.location.lat();
					var lng = results[0].geometry.location.lng();
					var latlng = new google.maps.LatLng(lat, lng);
					var bounds = results[0].geometry.bounds;

					geocoder.geocode({'latLng': latlng}, function(results1, status1) {
						if (status1 == google.maps.GeocoderStatus.OK) {
							if (results1[1]) {
								var result = $.map(results1, function(loc) {
									console.log(loc.geometry);

									return {
										result: loc.formatted_address,
										display: loc.formatted_address,
										data: [loc.formatted_address, loc.geometry]
									};
								});
								response(result);
							}
						}
					});
				}
			});
		}, {asynchronousFunction: true});

		this._searchBox.result(function(target, data) {
			console.log(data[1]);
			self._map.setZoom(14);
			
			if (data[1].bounds) {
				self._map.panToBounds(data[1].bounds);
			} else if (data[1].location) {
				self._map.panTo(data[1].location);
			}

			return false;
		});

		this._markButton.nQuireDynamicMeasureLink('enable');
	};

	GoogleMapMeasureManager.prototype.clearValue = function() {
		this.forgetMarker();
		this.updateData();
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
		this._gotoButton.nQuireDynamicMeasureLink('enable');
	};

	GoogleMapMeasureManager.prototype.forgetMarker = function() {
		if (this._marker) {
			this._marker.setMap(null);
			this._marker = null;
		}
		this._forgetButton.nQuireDynamicMeasureLink('disable');
		this._gotoButton.nQuireDynamicMeasureLink('disable');
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



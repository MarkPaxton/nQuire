

$(function() {
  nQuireJsSupport.register('VirtualMicroscopeManager', {
    _iframeOrigin: null,
    _messageCallbacks: null,
    _status: null,
    _statusListeners: null,
    _position: null,
    _positionListeners: null,
    _measure: null,
    _measureListeners: null,
    _getViewUrlListeners: null,
    _getSnapshotListeners: null,
    _sample: null,
    _divContainer: null,
    _iframe: null,
    _vmData: null,
    init: function(dependencies) {
      this._messageListeners = {};
      this._status = false;
      this._statusListeners = [];
      this._position = null;
      this._positionListeners = [];
      this._measure = null;
      this._measureListeners = [];
      this._getViewUrlListeners = [];
      this._getSnapshotListeners = [];
      this._vmData = dependencies.VirtualMicroscopeData;
      this._divContainer = $('#virtual_microscope_container');
      var self = this;
      window.addEventListener("message", function(event) {
        self._receiveMessage(event);
      }, false);
      this._captureMessages('monitor', function(msg) {
        self._monitorMessageReceived(msg);
      });
      this._captureMessages('get', function(msg) {
        self._getMessageReceived(msg);
      });
    },
    addStatusListener: function(callback) {
      this._statusListeners.push(callback);
      callback(this._status);
    },
    addPositionListener: function(callback) {
      this._positionListeners.push(callback);
      callback(this._position);
    },
    addMeasureListener: function(callback) {
      this._measureListeners.push(callback);
      callback(this._measure);
    },
    setMeasureValue: function(value) {
      this._post('set', 'FeatureState', {
        measure: true
      });
      this._post('set', 'MeasureMM', value);
    },
    stopMeasureTool: function() {
      this._post('set', 'FeatureState', {
        measure: false
      });
    },
    getUrlView: function(callback) {
      this._getViewUrlListeners.push(callback);
      this._post('get', 'viewURL');
    },
    getSnapshot: function(callback) {
      this._getSnapshotListeners.push(callback);
      this._post('get', 'Snapshot', {});
    },
    _captureMessages: function(message, callback, justOne) {
      this._messageListeners[message] = {
        callback: callback,
        justOne: justOne
      };
    },
    _stopCapturingMessage: function(message) {
      if (this._messageListeners[message]) {
        delete this._messageListeners[message];
      }
    },
    _monitorMessageReceived: function(msg) {
      if (msg.param === 'PositionPixels') {
        if (msg.content) {
          if (!this._position || this._position.sample !== this._sample ||
                  this._position.position.x !== msg.content.x || this._position.position.y !== msg.content.y ||
                  this._position.position.zoom !== msg.content.zoom) {

            this._position = {
              sample: this._sample,
              position: msg.content
            };
            for (var i in this._positionListeners) {
              this._positionListeners[i](this._position);
            }
          }
        }
      } else if (msg.param === 'MeasureMM') {
        if (msg.content) {
          this._measure = msg.content;
          for (var i in this._measureListeners) {
            this._measureListeners[i](this._measure);
          }
        }
      }
    },
    _getMessageReceived: function(msg) {
      if (msg.param === 'viewURL') {
        if (msg.content) {
          var list = this._getViewUrlListeners;
          this._getViewUrlListeners = [];
          for (var i in list) {
            list[i](msg.content);
          }
        }
      } else if (msg.param === 'Snapshot') {
        if (msg.content) {
          var list = this._getSnapshotListeners;
          this._getSnapshotListeners = [];
          for (var i in list) {
            list[i](msg.content);
          }
        }
      }
    },
    setSample: function(sampleView) {
      var sample = null, query = null;
      if (sampleView && typeof sampleView === 'object') {
        sample = sampleView.sample;
        query = sampleView.viewQuery;
      } else {
        sample = sampleView;
      }

      if (sample !== this._sample) {
        this._fireStatusChanged(false);
        this._iframeOrigin = null;
        this._sample = sample;
        this._divContainer.empty();
        if (this._iframe) {
          this._iframe = null;
        }

        if (sample) {
          this._iframe = $('<iframe>').addClass('virtual_microscope_iframe').appendTo(this._divContainer);
          $('<div>').addClass('virtual_microscope_hide_controls').appendTo(this._divContainer);

          var src = this._vmData.path + sample + '/index.html';
          if (query) {
            src += "?" + query;
          }
          this._iframe.attr('src', src);
          var self = this;
          this._captureMessages('ready', function(msg) {
            self._fireStatusChanged(true);
          }, true);
          return false;
        }
      }
      return true;
    },
    getSectionSize: function() {
      return this._vmData.sizes[this._sample];
    },
    setPosition: function(pos) {
      var content = {
        x: pos.x,
        y: pos.y,
        zoom: Math.max(0.00001, pos.zoom)
      };
      this._post('set', 'PositionPixels', content);
    },
    resetPosition: function() {
      if (this._position) {
        this.setPosition({x: this._position.position.x, y: this._position.position.y, zoom: 0.0});
      }
    },
    getCurrentSample: function() {
      return this._sample;
    },
    _fireStatusChanged: function(status) {
      this._status = status;
      if (status) {
        this._post('monitor', 'MeasureMM');
        this._post('monitor', 'PositionPixels');
      } else {
        this._position = this._measure = null;
      }

      for (var i = 0; i < this._statusListeners.length; i++) {
        this._statusListeners[i](status);
      }
    },
    /* VM communication functions */

    _post: function(action, param, value) {
      try {
        var iframe = this._iframe[0].contentWindow;

        var msg = {
          action: action
        };
        if (action !== 'list') {
          msg.activityId = 0;
        }
        if (typeof(param) !== 'undefined') {
          msg.param = param;
        }
        if (value) {
          msg.value = value;
        }
        if (iframe && iframe.postMessage && this._iframeOrigin) {
          iframe.postMessage(JSON.stringify(msg), this._iframeOrigin);
        } else {
          console.log('ERROR: Messaging is not available');
        }
      } catch (err) {
        console.log(err);
      }
    },
    _receiveMessage: function(event) {
      var msg = event.data;
      if (typeof msg === 'string') {
        try {
          msg = JSON.parse(msg);
        } catch (e) {
        }
      }


      var listener = null;
      if (msg.msg === 'VM ready') {
        this._iframeOrigin = event.origin;
        listener = this._messageListeners['ready'];
      } else {
        listener = this._messageListeners[msg.action];
      }

      if (listener) {
        if (listener.justOne) {
          var c = listener.callback;
          this._stopCapturingMessage(msg.action);
          c(msg);
        } else {
          listener.callback(msg);
        }
      }
    }
  }, ['VirtualMicroscopeData']);
});

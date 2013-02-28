

$(function() {
  nQuireJsSupport.register('VirtualMicroscopeManager', {
    _status: null,
    _statusListeners: null,
    _messageListeners: null,
    _sample: null,
    _divContainer: null,
    _iframe: null,
    _samplesPath: null,
    init: function(dependencies) {
      this._status = false;
      this._statusListeners = [];
      this._messageListeners = {};


      this._samplesPath = dependencies.VirtualMicroscopePath.path;
      this._divContainer = $('#virtual_microscope_container');

      var self = this;
      window.addEventListener("message", function(event) {
        self._receiveMessage(event);
      }, false);
    },
    addStatusListener: function(callback) {
      this._statusListeners.push(callback);
      callback(this._status);
    },
    captureMessages: function(message, listenerId, callback, greedy) {
      if (!this._messageListeners[message]) {
        this._messageListeners[message] = {
          greedy: null,
          listeners: {}
        };
      }

      this._messageListeners[message].listeners[listenerId] = callback;
      if (greedy) {
        this._messageListeners[message].greedy = listenerId;
      }
      ;
    },
    stopCapturingMessage: function(message, listenerId) {
      if (this._messageListeners[message]) {
        delete this._messageListeners[message].listeners[listenerId];
        if (this._messageListeners[message].greedy === listenerId) {
          this._messageListeners[message].greedy = null;
        }
      }
    },
    setSample: function(sample) {
      if (sample !== this._sample) {
        this._fireStatusChanged(false);
        this._sample = sample;
        /* TODO load microscope */

        if (this._iframe) {
          this._iframe.remove();
          this._iframe = null;
        }

        if (sample) {
          this._iframe = $('<iframe>').addClass('virtual_microscope_iframe').appendTo(this._divContainer);
          this._iframe.attr('src', this._samplesPath + sample);

          var probing = true;
          var self = this;
          this.captureMessages('list', 'probe', function() {
            probing = false;
            self._fireStatusChanged(true);
            console.log('vm ready!');
          }, true);
          var schedule = function() {
            setTimeout(probe, 10);
          };
          var probe = function() {
            if (probing) {
              console.log('probing...');
              self._post('list');
              schedule();
            }
          };
          schedule();
          return false;
        }
      }
      return true;
    },
    _fireStatusChanged: function(status) {
      this._status = status;
      for (var i = 0; i < this._statusListeners.length; i++) {
        this._statusListeners[i](status);
      }
    },
    /* VM communication functions */

    _post: function(action, param, value) {
      try {
        var iframe = this._iframe[0].contentWindow;
        if (!iframe.onerror) {
          iframe.onerror = function(error) {
            console.log('iframe error: ' + error);
            return true;
          };
        }
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

        if (iframe && iframe.postMessage) {
          iframe.postMessage(msg, location.href);
        } else {
          console.log('ERROR: Messaging is not available');
        }
      } catch (err) {
        console.log(err);
      }
    },
    _receiveMessage: function(event) {
      var msg = event.data;
      var listeners = this._messageListeners[msg.action];
      if (listeners) {
        if (listeners.greedy) {
          var lid = listeners.greedy;
          var l = listeners.listeners[lid];
          this.stopCapturingMessage(msg.action, lid);
          l(msg);
        }
      }
    }
  }, ['VirtualMicroscopePath']);
});

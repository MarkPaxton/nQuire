

$(function() {
  nQuireJsSupport.register('VirtualMicroscopeManager', {
    _status: null,
    _statusListeners: null,
    _messageListeners: null,
    init: function() {
      this._status = false;
      this._statusListeners = [];
      this._messageListeners = {};
    },
    addStatusListener: function(listener) {
      this._statusListeners.push(listener);
      listener.vmReadyStatus(this._status);
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
      this._fireStatusChange(false);
      /* TODO load microscope */

      if (sample) {
        var probing = true;
        var self = this;
        this.captureMessages('list', 'probe', function() {
          probing = false;
          self._fireStatusChange(true);
        }, true);
        var schedule = function() {
          setTimeout(probe, 10);
        };
        var probe = function() {
          if (probing) {
            self._post('list');
            schedule();
          }
        };
        schedule();
      }

    },
    _fireStatusChanged: function(status) {
      this._status = status;
      for (var i = 0; i < this._statusListeners.length; i++) {
        this._statusListeners[i].vmReadyStatus(status);
      }
    },
    /* VM communication functions */

    _post: function(action, param, value) {
      try {
        var iframe = $('#moonrock-vm-iframe')[0].contentWindow;
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
  }, ['LayoutManager']);
});



$(function() {
  nQuireJsSupport.register('VirtualMicroscopeManager', {
    _status: null,
    _statusListeners: null,
    init: function() {
      this._status = false;
      this._statusListeners = [];
    },
    addStatusListener: function(listener) {
      this._statusListeners.push(listener);
      listener.vmReadyStatus(this._status);
    },
    setSample: function() {
      this._fireStatusChange(false);
    },
    _fireStatusChanged: function(status) {
      this._status = status;
      for (var i = 0; i < this._statusListeners.length; i++) {
        this._statusListeners[i].vmReadyStatus(status);
      }
    }
  }, ['LayoutManager']);
});

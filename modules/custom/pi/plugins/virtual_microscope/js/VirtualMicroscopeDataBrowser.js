

$(function() {
  nQuireJsSupport.register('VirtualMicroscopeDataBrowser', {
    _ajaxService: null,
    _vmManager: null,
    init: function(dependencies) {
      var self = this;
      this._ajaxService = dependencies.AjaxDataService;
      this._vmManager = dependencies.VirtualMicroscopeManager;

      this._vmManager.addStatusListener(function(ready) {
        self.vmReadyStatus(ready);
      });
    },
    vmReadyStatus: function(ready) {
      if (ready) {
        this._ajaxService.enableDataInput();
      } else {
        this._ajaxService.disableDataInput();
      }
    }

  }, ['AjaxDataService', 'VirtualMicroscopeManager']);
});

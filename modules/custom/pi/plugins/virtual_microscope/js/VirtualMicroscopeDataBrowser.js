

$(function() {
  nQuireJsSupport.register('VirtualMicroscopeDataBrowser', {
    _ajaxService: null,
    _vmManager: null,
    init: function(dependencies) {
      this._ajaxService = dependencies.AjaxDataService;
      this._vmManager = dependencies.VirtualMicroscopeManager;

      this._vmManager.addStatusListener(this);
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

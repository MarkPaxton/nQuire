

$(function() {
  nQuireJsSupport.register('VirtualMicroscopeDataBrowser', {
    _ajaxService: null,
    _vmManager: null,
    _snapshotMeasure: null,
    init: function(dependencies) {
      var self = this;
      this._ajaxService = dependencies.AjaxDataService;
      this._vmManager = dependencies.VirtualMicroscopeManager;
      
      this._snapshotMeasure = dependencies.VirtualMicroscopeDataBrowserSnapshotMeasureNid.measure;
      
      this._vmManager.addStatusListener(function(ready) {
        self.vmReadyStatus(ready);
      });
      
      
      /*var handler = this._ajaxService.getMeasureHandler(this._snapshotMeasure);
       * TODO react when form is initialized
       */
    },
    vmReadyStatus: function(ready) {
      this._ajaxService.clearData();
      if (ready) {
        this._ajaxService.enableDataInput();
      } else {
        this._ajaxService.disableDataInput();
      }
    }

  }, ['AjaxDataService', 'VirtualMicroscopeManager', 'VirtualMicroscopeSnapshotMeasure', 'VirtualMicroscopeDataBrowserSnapshotMeasureNid']);
});



$(function() {
  nQuireJsSupport.register('VirtualMicroscopeDataBrowser', {
    _ajaxService: null,
    _vmManager: null,
    _snapshotMeasure: null,
    _vmViewMeasureHandler: null,
    _loadingData: null,
    init: function(dependencies) {
      var self = this;
      this._ajaxService = dependencies.AjaxDataService;
      this._vmManager = dependencies.VirtualMicroscopeManager;
      this._pageManager = dependencies.VirtualMicroscopePageManager;

      this._vmManager.addStatusListener(function(ready) {
        self.vmReadyStatus(ready);
      });

    },
    initVirtualMicroscopeViewMeasureHandler: function(handler) {
      this._vmViewMeasureHandler = handler;

      var data = this._ajaxService.getCurrentData(this._snapshotMeasure);
      var viewValue = handler.parseViewFromData(data);
      console.log('load data: ');
      console.log(viewValue);

      if (viewValue) {
        this._loadingData = data.id;
        this._pageManager.openSampleView(viewValue);
      }

    },
    vmReadyStatus: function(ready) {
      if (this._loadingData && ready) {
        this._ajaxService.setData(this._loadingData);
        this._loadingData = null;
      } else {
        this._ajaxService.clearData(ready);
      }
    }

  }, ['AjaxDataService', 'VirtualMicroscopeManager', 'VirtualMicroscopePageManager']);
});

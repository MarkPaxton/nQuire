

$(function() {
  nQuireJsSupport.register('VirtualMicroscopeDataBrowser', {
    _ajaxService: null,
    _vmManager: null,
    _snapshotMeasure: null,
    _vmViewMeasureHandler: null,
    _whiteboard: null,
    _loadingData: null,
    _ready: null,
    init: function(dependencies) {
      var self = this;
      this._ready = false;
      this._ajaxService = dependencies.AjaxDataService;
      this._vmManager = dependencies.VirtualMicroscopeManager;
      this._pageManager = dependencies.VirtualMicroscopePageManager;
      this._whiteboard = dependencies.VirtualMicroscopeWhiteboard;

      this._whiteboard.addWhiteboardReadyListener(function(ready) {
        self.whiteboardReadyStatus(ready);
      });
      this._whiteboard.addResizeListener(function() {
        if (self._ready) {
          self._updateViewShadows();
        }
      });
    },
    initVirtualMicroscopeViewMeasureHandler: function(handler) {
      this._vmViewMeasureHandler = handler;

      var data = this._ajaxService.getCurrentData(this._snapshotMeasure);
      var viewValue = handler.parseViewFromData(data);

      if (viewValue) {
        this._loadingData = data.id;
        this._pageManager.openSampleView(viewValue);
      }
    },
    whiteboardReadyStatus: function(ready) {
      this.ready = ready;

      if (this._loadingData && ready) {
        this._ajaxService.setData(this._loadingData);
        this._loadingData = null;
      } else {
        this._ajaxService.clearData(ready);
      }

      if (ready) {
        var data = this._ajaxService.getData();
        for (var id in data) {
          var view = this._vmViewMeasureHandler.parseViewFromData(data[id]);
          if (view.sample === this._vmManager.getCurrentSample()) {
            this._createLabel(id, view.position.x, view.position.y);
          }
        }
        this._updateViewShadows();
      }
    },
    _updateViewShadows: function() {
      var data = this._ajaxService.getData();
      for (var id in data) {
        var view = this._vmViewMeasureHandler.parseViewFromData(data[id]);
        if (view.sample === this._vmManager.getCurrentSample()) {
          this._createViewShadow(id, view.position);
        }
      }
    },
    _createLabel: function(id, x, y) {
      var shape = {
        type: 'circle',
        cx: x,
        cy: y,
        r: 15,
        settings: {
          fill: '#FDD017',
          stroke: '#AF7817',
          strokeWidth: 2
        }
      };
      this._whiteboard.draw(id + '-label', shape, true);
    },
    _createViewShadow: function(id, pos) {

    }
  }, ['AjaxDataService', 'VirtualMicroscopeManager', 'VirtualMicroscopePageManager', 'VirtualMicroscopeWhiteboard']);
});

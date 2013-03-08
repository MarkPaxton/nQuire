

$(function() {

  var LabelPainter = function(viewMeasureHandler, actionCallbacks) {
    this._viewMeasureHandler = viewMeasureHandler;
    this._actionCallbacks = actionCallbacks;
  };
  LabelPainter.prototype.createPaintShape = function(data, options) {
    if (options.mode === 'selected') {
      return false;
    } else {
      var position = this._viewMeasureHandler.parsePositionFromData(data);
      var shape = {
        pos: {
          x: position.x,
          y: position.y
        },
        shape: {
          type: 'circle',
          r: 12,
          settings: options.mode === 'hover' ?
                  {fill: '#FFFF49', stroke: '#E1AA49', strokeWidth: 2} :
                  {fill: '#FDD017', stroke: '#AF7817', strokeWidth: 2},
          callbacks: this._actionCallbacks
        },
        dontScale: true,
        position: 'front'
      };
      return shape;
    }
  };

  var ShadowPainter = function(viewMeasureHandler, whiteboard) {
    this._viewMeasureHandler = viewMeasureHandler;
    this._whiteboard = whiteboard;
    this._settings = {
      normal: {fill: 'black', fillOpacity: .3, stroke: 'black', strokeWidth: 1, strokeOpacity: .3},
      hover: {fill: 'black', fillOpacity: .2, stroke: 'black', strokeWidth: 1, strokeOpacity: .2},
      selected: {fill: 'none', stroke: 'red', strokeWidth: 4, strokeOpacity: 1, 'stroke-dasharray': '8, 4', 'vector-effect': 'non-scaling-stroke'}
    };
  };
  ShadowPainter.prototype.createPaintShape = function(data, options) {
    var position = this._viewMeasureHandler.parsePositionFromData(data);
    var w = this._whiteboard.getViewWindow(position);

    return {
      pos: {
        x: w.cx,
        y: w.cy
      },
      shape: {
        type: 'rect',
        w: w.w,
        h: w.h,
        settings: this._settings[options.mode]
      },
      position: 'back'
    };
  };

  nQuireJsSupport.register('VirtualMicroscopeDataBrowser', {
    _ajaxService: null,
    _vmManager: null,
    _snapshotMeasure: null,
    _vmViewMeasureHandler: null,
    _whiteboard: null,
    _loadingData: null,
    _ready: null,
    _paintFeatures: null,
    init: function(dependencies) {
      var self = this;
      this._ready = false;
      this._vmManager = dependencies.VirtualMicroscopeManager;
      this._pageManager = dependencies.VirtualMicroscopePageManager;
      this._whiteboard = dependencies.VirtualMicroscopeWhiteboard;
      this._paintFeatures = [];

      this._ajaxService = dependencies.AjaxDataService;
      this._ajaxService.addDataListener(function(event, data) {
        self._dataModified(event, data);
      });

      this._whiteboard.addWhiteboardReadyListener(function(ready) {
        self.whiteboardReadyStatus(ready);
      });
      this._whiteboard.addResizeListener(function() {
        if (self._ready) {
          self._updatePaint();
        }
      });

      $('#virtual_microscope_view_menu_data').change(function() {
        $('#virtual_microscope_view_menu').find('.virtual_microscope_view_menu_popup').find('input').attr('disabled', !$(this).attr('checked'));

        if (self._ready) {
          self._updatePaint();
        }
      });
    },
    _addDrawableFeature: function(feature, title, handler, first) {
      this._paintFeatures.push({
        feature: feature,
        enabled: true,
        handler: handler
      });

      var self = this;

      $('<label>').appendTo($('<div>')[first ? 'prependTo' : 'appendTo']($('.virtual_microscope_view_menu_popup')))
              .append($('<input>').attr('type', 'checkbox').attr('name', 'feature_' + feature).attr('checked', true)).append(title)
              .change(function() {
        if (self._ready) {
          self._updatePaint();
        }
      });
    },
    initVirtualMicroscopeViewMeasureHandler: function(handler) {
      this._vmViewMeasureHandler = handler;
      var self = this;

      var actionCallbacks = {
        hover: function(featureName, inside) {
          var id = "" + parseInt(featureName);
          var data = self._ajaxService.getData(id);
          self._updateDataPaint(data, inside ? 'hover' : null);
        },
        click: function(featureName) {
          var id = "" + parseInt(featureName);
          var pos = self._vmViewMeasureHandler.parsePositionFromData(self._ajaxService.getData(id));
          self._vmManager.setPosition(pos);
          self._ajaxService.setData(id, function() {
            self._updatePaint();
          });
        }
      };

      this._addDrawableFeature('shadow', 'Shadows', new ShadowPainter(handler, this._whiteboard), true);
      this._addDrawableFeature('label', 'Labels', new LabelPainter(handler, actionCallbacks), true);

      var data = this._ajaxService.getCurrentData(this._snapshotMeasure);
      var viewValue = handler.parseViewFromData(data);

      if (viewValue) {
        this._loadingData = data.id;
        this._pageManager.openSampleView(viewValue);
      }
    },
    registerPaintFeature: function(name, title, handler) {
      this._addDrawableFeature(name, title, handler);
      if (this._ready) {
        this._updatePaint();
      }
    },
    whiteboardReadyStatus: function(ready) {
      this._ready = ready;

      if (this._loadingData && ready) {
        this._ajaxService.setData(this._loadingData);
        this._loadingData = null;
      } else {
        this._ajaxService.clearData(ready);
      }

      if (ready) {
        this._updatePaint();
      }
    },
    _dataModified: function(event, data) {
      this._updatePaint();
    },
    _updatePaint: function() {
      //var currentData = this._ajaxService.getCurrentData();
      var sample = this._vmManager.getCurrentSample();
      if (sample) {
        if ($('#virtual_microscope_view_menu_data').attr('checked')) {
          var dataList = this._ajaxService.getDataList();
          for (var id in dataList) {
            var data = dataList[id];
            if (this._vmViewMeasureHandler.parseSampleFromData(data) === sample) {
              this._updateDataPaint(data);
            }
          }
        } else {
          this._whiteboard.clear();
        }
      }
    },
    _updateDataPaint: function(data, state) {
      var _state = state ? state : (data.id === this._ajaxService.getCurrentDataId() ? 'selected' : 'normal');

      for (var i in this._paintFeatures) {
        var fh = this._paintFeatures[i];
        var fid = data.id + '-' + fh.feature;
        var remove = true;
        if ($('#virtual_microscope_view_menu').find('.virtual_microscope_view_menu_popup').find('input[name="feature_' + fh.feature + '"]').attr('checked')) {
          if (fh.enabled) {
            var shape = fh.handler.createPaintShape(data, {mode: _state});
            if (shape) {
              var layer;
              switch (shape.position) {
                case 'front':
                  layer = 2;
                  break;
                case 'back':
                  layer = 0;
                  break;
                default:
                  layer = 1;
                  break;
              }
              this._whiteboard.draw(fid, layer, shape);
              remove = false;
            }
          }
        }
        if (remove) {
          this._whiteboard.remove(fid);
        }
      }
    }
  }, ['AjaxDataService', 'VirtualMicroscopeManager', 'VirtualMicroscopePageManager', 'VirtualMicroscopeWhiteboard']);
});

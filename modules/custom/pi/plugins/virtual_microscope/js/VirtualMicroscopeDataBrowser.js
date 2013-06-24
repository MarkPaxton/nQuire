

$(function() {

  var LabelPainter = function(viewMeasureHandler, independentMeasureManager, whiteboard, actionCallbacks) {
    this._viewMeasureHandler = viewMeasureHandler;
    this._independentMeasureManager = independentMeasureManager;
    this._whiteboard = whiteboard;
    this._actionCallbacks = actionCallbacks;

    this._styles = {
      'normal': {
        label: {fill: 'orange', stroke: '#dd6000', strokeWidth: 1, 'vector-effect': 'non-scaling-stroke'},
        text: {fontWeight: 'normal', fontSize: 12, fill: 'black', 'dominant-baseline': 'central', 'text-anchor': 'middle'},
        shadow: {stroke: '#333', strokeWidth: 1, fill: 'gray', 'fill-opacity': .2, 'vector-effect': 'non-scaling-stroke'}
      },
      'hover': {
        label: {fill: 'orange', stroke: '#dd6000', strokeWidth: 1, 'vector-effect': 'non-scaling-stroke'},
        text: {fontWeight: 'bold', fontSize: 12, fill: 'black', 'dominant-baseline': 'central', 'text-anchor': 'middle'},
        shadow: {stroke: '#333', strokeWidth: 1, fill: 'gray', 'fill-opacity': .2, 'vector-effect': 'non-scaling-stroke'}
      },
      'selected': {
        label: {fill: 'yellow', stroke: 'orange', strokeWidth: 3, 'vector-effect': 'non-scaling-stroke'},
        text: {fontWeight: 'bold', fontSize: 12, fill: 'black', 'dominant-baseline': 'central', 'text-anchor': 'middle'},
        shadow: {stroke: '#373', strokeWidth: 3, fill: 'none', 'vector-effect': 'non-scaling-stroke'}
      },
      'other': {
        label: {fill: 'orange', stroke: 'darkorange', strokeWidth: 1, 'vector-effect': 'non-scaling-stroke'},
        text: {fontWeight: 'normal', fontSize: 12, fill: 'black', 'dominant-baseline': 'central', 'text-anchor': 'middle'},
        shadow: {stroke: '#333', strokeWidth: 0, fill: 'gray', 'fill-opacity': .2, 'vector-effect': 'non-scaling-stroke'}
      },
      'other_hover': {
        label: {fill: 'orange', stroke: 'darkorange', strokeWidth: 1, 'vector-effect': 'non-scaling-stroke'},
        text: {fontWeight: 'bold', fontSize: 12, fill: 'black', 'dominant-baseline': 'central', 'text-anchor': 'middle'},
        shadow: {stroke: '#333', strokeWidth: 1, fill: 'gray', 'fill-opacity': .2, 'vector-effect': 'non-scaling-stroke'}
      }
    };
  };

  LabelPainter.prototype.createPaintShape = function(data, options, index) {
    var features = this._independentMeasureManager.getIndependentPaintMeasures();

    var position = this._viewMeasureHandler.parsePositionFromData(data);

    var bbox = null;
    for (var i in features) {
      var id = data.id + '-' + features[i];
      var fbbox = this._whiteboard.getBoundingBox(id);
      if (fbbox) {
        if (bbox) {
          if (fbbox.x0 < bbox.x0) {
            bbox.x0 = fbbox.x0;
          }
          if (fbbox.y0 < bbox.y0) {
            bbox.y0 = fbbox.y0;
          }
          if (fbbox.x1 > bbox.x1) {
            bbox.x1 = fbbox.x1;
          }
          if (fbbox.y1 > bbox.y1) {
            bbox.y1 = fbbox.y1;
          }
        } else {
          bbox = fbbox;
        }
      }
    }

    var style = options.isSelected ? 'selected' : (options.selectedData ? (options.hover ? 'other_hover' : 'other') : (options.hover ? 'hover' : 'normal'));
    var settings = this._styles[style];


    var shape = {
      pos: null,
      shapes: [],
      position: 'front'
    };

    var labelSize = 24;


    if (bbox) {
      var scale = this._whiteboard.getScaleAtZoom(position.zoom);
      var labelVmHeight = labelSize * scale;
      var labelVmWidth = labelSize * this._whiteboard.getScaleAtZoom(0);
      var gap = 10 * scale;

      bbox.x0 -= gap;
      bbox.x1 += gap;
      bbox.y0 -= gap + labelVmHeight;
      bbox.y1 += gap;


      shape.pos = {x: bbox.x0, y: bbox.y0};

      var x0 = bbox.x0 - shape.pos.x, y0 = bbox.y0 - shape.pos.y, x1 = bbox.x1 - shape.pos.x, y1 = bbox.y1 - shape.pos.y;
      var points = [[x0, y0], [x1, y0], [x1, y1], [x0, y1]];
      shape.shapes.push({
        type: 'polygon',
        points: points,
        settings: settings.shadow
      });
    } else {
      shape.pos = position;
    }

    shape.shapes.push({
      dontScale: true,
      type: 'polygon',
      points: [[0, 0], [labelSize, 0], [labelSize, labelSize], [0, labelSize]],
      settings: settings.label,
      callbacks: this._actionCallbacks
    });
    shape.shapes.push({
      dontScale: true,
      type: 'text',
      text: "" + (1 + index),
      x: .5 * labelSize,
      y: .5 * labelSize,
      settings: settings.text
    });

    return shape;

  };


  nQuireJsSupport.register('VirtualMicroscopeDataBrowser', {
    _ajaxService: null,
    _vmManager: null,
    _snapshotMeasure: null,
    _vmViewMeasureHandler: null,
    _whiteboard: null,
    _loadingData: null,
    _ready: null,
    _paintFeatureHandlers: null,
    _independentPaintFeatures: null,
    _labelPaintFeature: null,
    _viewButtons: null,
    init: function(dependencies) {
      var self = this;
      this._ready = false;

      this._viewButtons = {
        reset: $('#virtual_microscope_view_menu_reset').click(function() {
          self._vmManager.resetPosition();
          return false;
        }),
        centerOnData: $('#virtual_microscope_view_menu_center').click(function() {
          var pos = self._vmViewMeasureHandler.parsePositionFromData(self._ajaxService.getCurrentData());
          self._vmManager.setPosition(pos);
        })
      };

      this._vmManager = dependencies.VirtualMicroscopeManager;
      this._vmManager.addPositionListener(function(position) {
        if (position) {
          self._viewButtons.reset[position.position.zoom > .001 ? 'removeClass' : 'addClass']('inactive');
        }
      });

      this._pageManager = dependencies.VirtualMicroscopePageManager;
      this._whiteboard = dependencies.VirtualMicroscopeWhiteboard;
      this._labelPaintFeature = null;
      this._paintFeatureHandlers = {};
      this._independentPaintFeatures = [];

      this._ajaxService = dependencies.AjaxDataService;
      this._ajaxService.addDataListener(function(event, data) {
        if (event === 'deleted') {
          self._deleteDataPaint(data);
        }

        self._updatePaint();
        self._viewButtons.centerOnData[event === 'unselected' ? 'addClass' : 'removeClass']('inactive');
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
    _addDrawableFeature: function(feature, title, handler, enabled, isDataPaintHandler) {
      if (isDataPaintHandler) {
        this._labelPaintFeature = feature;
      } else {
        this._independentPaintFeatures.push(feature);
      }
      this._paintFeatureHandlers[feature] = handler;

      var self = this;
      var input = $('<input>').attr('type', 'checkbox').attr('name', 'feature_' + feature);
      if (enabled) {
        input.attr('checked', true);
      }

      $('<label>').appendTo($('<div>')[isDataPaintHandler ? 'prependTo' : 'appendTo']($('.virtual_microscope_view_menu_popup')))
              .append(input).append(title)
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
          self._updateDataPaint(data, true, inside);
        },
        click: function(featureName) {
          var id = "" + parseInt(featureName);
          var pos = self._vmViewMeasureHandler.parsePositionFromData(self._ajaxService.getData(id));
          self._vmManager.setPosition(pos);
          self._ajaxService.setData(id);
        }
      };

      //this._addDrawableFeature('shadow', 'Shadows', new ShadowPainter(handler, this._whiteboard), true, false);
      this._addDrawableFeature('label', 'Labels', new LabelPainter(handler, this, this._whiteboard, actionCallbacks), true, true);

      var data = this._ajaxService.getCurrentData(this._snapshotMeasure);
      var viewValue = handler.parseViewFromData(data);

      if (viewValue) {
        this._loadingData = data.id;
        this._pageManager.openSampleView(viewValue);
      }
    },
    getIndependentPaintMeasures: function() {
      return this._independentPaintFeatures;
    },
    registerPaintFeature: function(name, title, handler, enabled) {
      this._addDrawableFeature(name, title, handler, enabled, false);
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
    updateCurrentDataFeaturePaint: function(feature) {
      var data = this._ajaxService.getCurrentData();
      this._updateDataFeaturePaint(data, feature, {isSelected: true}, false);
    },
    _updatePaint: function() {
      var sample = this._vmManager.getCurrentSample();
      if (sample) {
        var all = $('#virtual_microscope_view_menu_data').attr('checked');
        var dataList = this._ajaxService.getDataList();
        for (var id in dataList) {
          var data = dataList[id];
          if (this._vmViewMeasureHandler.parseSampleFromData(data) === sample) {
            this._updateDataPaint(data, all, false);
          }
        }
        this._whiteboard.clearTempShapes();
      }
    },
    _deleteDataPaint: function(dataId) {
      for (var feature in this._paintFeatureHandlers) {
        var fid = dataId + '-' + feature;
        this._whiteboard.remove(fid);
      }
    },
    _updateDataPaint: function(data, all, hover) {
      var selectedData = this._ajaxService.getCurrentDataId();
      var options = {
        hover: hover,
        paintAll: all,
        isSelected: selectedData === data.id,
        selectedData: selectedData
      };

      for (var i in this._independentPaintFeatures) {
        this._updateDataFeaturePaint(data, this._independentPaintFeatures[i], options, all);
      }

      this._updateDataFeaturePaint(data, this._labelPaintFeature, options, all);
    },
    _updateDataFeaturePaint: function(data, feature, options, all) {
      var fh = this._paintFeatureHandlers[feature];

      if (fh) {
        var fid = data ? data.id + '-' + feature : 'temp';
        var remove = true;
        if (options.isSelected || (all && $('#virtual_microscope_view_menu').find('.virtual_microscope_view_menu_popup').find('input[name="feature_' + feature + '"]').attr('checked'))) {
          var shape = fh.createPaintShape(data, options, data ? data.index : -1);
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
            remove = false;
            this._whiteboard.draw(fid, layer, shape);
          }
        }
        if (remove) {
          this._whiteboard.remove(fid);
        }
      }
    },
    getCurrentDataSvg: function() {
      var svg = $(this._whiteboard.getCurrentSvg());
      svg.removeClass('virtual-microscope-whiteboard').removeClass('whiteboard-inactive');

      var self = this;
      var currentData = this._ajaxService.getCurrentDataId();

      var filter = function(shapeName) {
        if (currentData) {
        var keys = shapeName.split('-');
        return keys.length !== 2 || keys[0] !== currentData || keys[1] === 'label';
        } else {
          return shapeName !== 'temp';
        }
      };

      svg.find('g[shape-name]').each(function() {
        var shapeGroup = $(this);
        var shapeName = shapeGroup.attr('shape-name');
        if (filter(shapeName)) {
          shapeGroup.remove();
        }
      });

      return svg;
    }
  }, ['AjaxDataService', 'VirtualMicroscopeManager', 'VirtualMicroscopePageManager', 'VirtualMicroscopeWhiteboard']);
});

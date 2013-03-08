

$(function() {
  var AnnotationMeasureManager = function(element, menuManager, whiteboard, measureId, dataBrowser) {
    this._element = $(element);
    this._startButton = this._element.find('button');
    this._measureId = measureId;
    this._menuManager = menuManager;
    this._whiteboard = whiteboard;

    this._baseStyleColors = {blue: '#0000ff', red: '#ff0000', yellow: '#ffff00', green: '#00ff00'};
    this._baseStyle = {fill: 'none', 'stroke-width': 20, 'stroke-linecap': 'round', 'stroke-linejoin': 'round'};

    this._shapes = [];
    this._shapesCache = null;
    this._tempShapes = null;

    dataBrowser.registerPaintFeature(measureId, 'Annotation', this);

    var self = this;
    this._startButton.click(function() {
      self._startAnnotation();
      return false;
    });
  };

  AnnotationMeasureManager.prototype.setServiceDelegate = function(delegate) {
    this._serviceDelegate = delegate;
  };

  AnnotationMeasureManager.prototype.initMeasureValue = function(value) {
    try {
      this._shapes = JSON.parse(value);
    } catch (e) {
      this._shapes = [];
    }
  };


  AnnotationMeasureManager.prototype.clearValue = function() {
    this._serviceDelegate.saveData('');
    this._shapes = [];
  };

  AnnotationMeasureManager.prototype._updateCachedShape = function(data, i, shape) {
    this._shapesCache[i].points = shape.points;
  },
          AnnotationMeasureManager.prototype.createPaintShape = function(data, options) {
    if (!this._shapesCache) {
      var shapes = this._tempShapes ? this._tempShapes : this._shapes;
      this._shapesCache = [];
      for (var i in shapes) {
        this._shapesCache.push({
          shape: {
            type: 'polyline',
            points: shapes[i].points
          },
          settings: $.extend({}, this._baseStyle, {stroke: this._baseStyleColors[shapes[i].color]})
        });
      }
    }

    return this._shapesCache;
  };


  AnnotationMeasureManager.prototype.stopUserDelayProcess = function() {
    /* since this is not cancel, accept temporal data and close */
    this._stopAndSaveChanges();
  };

  AnnotationMeasureManager.prototype._stopAndSaveChanges = function() {
    this._shapes = this._tempShapes;
    this._serviceDelegate.saveData(JSON.stringify(this._shapes));
    this._stopInput();
  };

  AnnotationMeasureManager.prototype._stopInput = function() {
    this._tempShapes = null;
    this._whiteboard.stopPathCapture();
    this._menuManager.closeMenu();
    this._serviceDelegate.userDelayProcessStopped();
    this._startButton.removeAttr('disabled');
  };

  AnnotationMeasureManager.prototype._startAnnotation = function() {
    this._startButton.attr('disabled', 'disabled');
    var self = this;
    this._serviceDelegate.userDelayProcessStarted();
    this._menuManager.openMenu(function(event, data) {
      self._processMenuEvent(event, data);
    });
    this._whiteboard.captureUserInputPath(null);
  };


  AnnotationMeasureManager.prototype._processMenuEvent = function(event, data) {
    switch (event) {
      case 'mode':
        break;
      case 'done':
        this._stopAndSaveChanges();
        break;
      case 'cancel':
        this._stopInput();
        break;
    }
  };


  nQuireJsSupport.register('VirtualMicroscopeAnnotationMeasure', {
    init: function(dependencies) {
      this._createPaintMenu();
      this._listener = null;

      var self = this;

      $('div[measure_type="measure_widget_vm_annotation"]').each(function() {
        var inputElementId = $(this).attr('input_element_id');
        var manager = new AnnotationMeasureManager(this, self, dependencies.VirtualMicroscopeWhiteboard, inputElementId, dependencies.VirtualMicroscopeDataBrowser);
        dependencies.DynamicMeasureService.registerMeasure(inputElementId, manager);
      });
    },
    openMenu: function(listener) {
      this._listener = listener;
      this._menuContainer.removeClass('virtual-microscope-annotation-menu-disabled');
    },
    closeMenu: function() {
      this._menuContainer.addClass('virtual-microscope-annotation-menu-disabled');
      this._listener = null;
    },
    _createPaintMenu: function() {
      this._menuContainer = $('<div>').addClass('virtual-microscope-annotation-menu virtual-microscope-annotation-menu-disabled').appendTo('#virtual_microscope_container');
      var colors = ['blue', 'green', 'yellow', 'red'];
      for (var i in colors) {
        $('<div>').addClass('virtual-microscope-annotation-mode-selector virtual-microscope-annotation-color virtual-microscope-annotation-color-' + colors[i])
                .attr('mode', colors[i])
                .appendTo(this._menuContainer);
      }

      $('<div>').addClass('virtual-microscope-annotation-mode-selector virtual-microscope-annotation-erase')
              .attr('mode', 'erase')
              .appendTo(this._menuContainer);

      var self = this;
      this._menuContainer.find('.virtual-microscope-annotation-mode-selector').click(function() {
        self._setMode($(this).attr('mode'));
      });

      var buttonContainer = $('<div>').addClass('virtual-microscope-annotation-buttons-container').appendTo(this._menuContainer);
      $('<button>').html('Done').appendTo(buttonContainer).click(function() {
        if (self._listener) {
          self._listener('done');
        }
        return false;
      });
      $('<button>').html('Cancel').appendTo(buttonContainer).click(function() {
        if (self._listener) {
          self._listener('cancel');
        }
        return false;
      });

      this._setMode('blue');
    },
    _setMode: function(mode) {
      if (this._mode !== mode) {
        this._mode = mode;
        this._menuContainer.find('.virtual-microscope-annotation-mode-selector').each(function() {
          var element = $(this);
          if (element.attr('mode') === mode) {
            element.addClass('virtual-microscope-annotation-mode-selected');
          } else {
            element.removeClass('virtual-microscope-annotation-mode-selected');
          }
        });
        if (this._listener) {
          this._listener('mode', mode);
        }
      }
    }
  }, ['DynamicMeasureService', 'VirtualMicroscopeDataBrowser', 'VirtualMicroscopeWhiteboard']);
});



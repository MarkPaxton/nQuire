


$(function() {
  nQuireJsSupport.register('VirtualMicroscopeWhiteboard', {
    _sizes: null,
    _vmManager: null,
    _transform: null,
    _iframe: null,
    _shapes: null,
    _readyListeners: null,
    _resizeListeners: null,
    init: function(dependencies) {
      this._parent = $('#virtual_microscope_container');
      this._sizes = dependencies.VirtualMicroscopeSizeData.data;
      this._vmManager = dependencies.VirtualMicroscopeManager;
      this._readyListeners = [];
      this._resizeListeners = [];

      var self = this;

      dependencies.LayoutManager.addResizeListener(function() {
        self.windowResized();
      });

      this._vmManager.addStatusListener(function(ready) {
        self.vmReady(ready);
      });
    },
    /* drawing functions */

    _drawCreateSvgParent: function(layer, x, y) {
      var parent = this._svg.group(this._svgLayers[layer]);
      $(parent).attr('transform', 'translate(' + x + ' ' + y + ')');
      return parent;
    },
    draw: function(name, shape, settings, layer, dontScale, callbacks) {
      var _layer = typeof layer === 'undefined' ? 1 : layer;
      var _dontScale = dontScale ? true : false;

      var shapeObj = null, parent = null;


      switch (shape.type) {
        case 'circle':
          parent = this._drawCreateSvgParent(_layer, shape.cx, shape.cy);
          shapeObj = this._svg.circle(parent, 0, 0, shape.r, settings);
          break;
        case 'rect':
          var hw = .5 * shape.w, hh = .5 * shape.h;
          parent = this._drawCreateSvgParent(_layer, shape.x + hw, shape.y + hh);
          shapeObj = this._svg.rect(parent, -hw, -hh, shape.w, shape.h, 0, 0, settings);
          break;
      }


      this.remove(name);

      if (shapeObj) {
        this._shapes[name] = {group: parent, dontScale: _dontScale, object: shapeObj};
        if (_dontScale) {
          $(shapeObj).attr('transform', this._transform.labelTransform);
        }

        if (callbacks) {
          var element = $(shapeObj);
          element.addClass('whiteboard-active-element');

          if (callbacks.hover) {
            element.customMouseInput('hover', function(inside) {
              callbacks.hover(name, inside);
            });
          }

          if (callbacks.click) {
            element.customMouseInput('click', function() {
              callbacks.click(name);
            });
          }
        }
      }
    },
    setShapeSettings: function(name, settings) {
      this._svg.change(this._shapes[name].object, settings);
    },
    remove: function(name) {
      if (this._shapes[name]) {
        this._svg.remove(this._shapes[name].group);
        this._shapes[name] = null;
      }
    },
    clear: function() {
      for (var name in this._shapes) {
        this._svg.remove(this._shapes[name].group);
      }
      this._shapes = {};
    },
    /* paint input functions */
    capturePath: function(callback) {

    },
    stopPathCapture: function() {

    },
    /* svg creation */

    addWhiteboardReadyListener: function(callback) {
      this._readyListeners.push(callback);
    },
    addResizeListener: function(callback) {
      this._resizeListeners.push(callback);
    },
    _fireWhiteboardReadyEvent: function(ready) {
      for (var i in this._readyListeners) {
        this._readyListeners[i](ready);
      }
    },
    _fireResizeEvent: function() {
      for (var i in this._resizeListeners) {
        this._resizeListeners[i]();
      }
    },
    vmReady: function(ready) {
      if (ready) {
        var self = this;

        this._svgElement = $('<svg class="virtual-microscope-whiteboard whiteboard-inactive" xmlns="http://www.w3.org/2000/svg">').appendTo(this._parent);
        this._svgElement.svg({
          onLoad: function(svg) {
            self._svg = svg;
            self._svgGroup = svg.group();
            self._svgLayers = [
              svg.group(self._svgGroup),
              svg.group(self._svgGroup),
              svg.group(self._svgGroup)
            ];

            self._svgElement.customMouseInput('rawdrag', function(action, point) {
              /*
               if (self._enabled) {
               switch (action) {
               case 'dragstart':
               console.log('wb start');
               console.log(point);
               self.startDrag(point);
               break;
               case 'drag':
               self.drag(point);
               break;
               case 'dragend':
               self.endDrag();
               break;
               }
               }*/
            });

            self._shapes = {};

            self._initTransform();

            self._vmManager.addPositionListener(function(pos) {
              if (pos) {
                self.vmPosChanged(pos.position);
              }
            });

            self._fireWhiteboardReadyEvent(true);
          }
        });


      } else {
        $('.virtual-microscope-whiteboard').remove();
        this._svgElement = null;
        this._transform = null;
        this._fireWhiteboardReadyEvent(false);
      }
    },
    /* geometry transform stuff */

    windowResized: function() {
      if (this._transform) {
        this._elementResized();
        if (this._transform.viewPos) {
          this._updateTransform();
          this._fireResizeEvent(true);
        }
      }
    },
    vmPosChanged: function(pos) {
      this._transform.viewPos = pos;
      this._transform.centerRelPos = {
        dx: pos.x, // - .5 * this._transform.sectionSize.width,
        dy: pos.y// - .5 * this._transform.sectionSize.height
      };

      this._updateTransform();
    },
    _initTransform: function() {
      this._transform = {
        sectionSize: this._sizes[this._vmManager.getCurrentSample()],
        frameSize: {},
        frameCenter: {},
        viewPos: null,
        vm2canvas: 0,
        canvas2vm: 0
      };

      this._elementResized();

      var wk = (this._transform.frameSize.width - 30) / this._transform.sectionSize.width;
      var hk = (this._transform.frameSize.height - 70) / this._transform.sectionSize.height;
      this._transform.zeroZoomScale = Math.min(wk, hk);
    },
    _elementResized: function() {
      this._transform.frameSize.width = parseFloat(this._parent.width());
      this._transform.frameSize.height = parseFloat(this._parent.height());
      this._transform.frameCenter.x = .5 * this._transform.frameSize.width;
      this._transform.frameCenter.y = .5 * (this._transform.frameSize.height - 40);

      this._svgElement.width(this._transform.frameSize.width);
      this._svgElement.height(this._transform.frameSize.height - 40);
    },
    _updateTransform: function() {
      this._transform.vm2canvas = this._transform.viewPos.zoom + this._transform.zeroZoomScale * (1 - this._transform.viewPos.zoom);
      this._transform.canvas2vm = 1. / this._transform.vm2canvas;
      this._transform.svgTransform =
              'translate(' + (this._transform.frameCenter.x) + ' ' + (this._transform.frameCenter.y) + ') ' +
              'scale(' + this._transform.vm2canvas + ') ' +
              'translate(' + (-this._transform.centerRelPos.dx) + ' ' + (-this._transform.centerRelPos.dy) + ') ';

      this._transform.labelTransform = 'scale(' + this._transform.canvas2vm + ')';

      $(this._svgGroup).attr('transform', this._transform.svgTransform);
      if (this._shapes) {
        for (var i in this._shapes) {
          if (this._shapes[i] && this._shapes[i].dontScale) {
            $(this._shapes[i].object).attr('transform', this._transform.labelTransform);
          }
        }
      }
    },
    getViewWindow: function(pos) {
      var vm2canvas = pos.zoom + this._transform.zeroZoomScale * (1 - pos.zoom);
      var canvas2vm = 1.0 / vm2canvas;

      var viewWidth = this._transform.frameSize.width * canvas2vm;
      var viewHeight = (this._transform.frameSize.height - 40) * canvas2vm;

      return {
        x: pos.x - .5 * viewWidth,
        y: pos.y - .5 * viewHeight,
        w: viewWidth,
        h: viewHeight
      };
    }
  }, ['VirtualMicroscopeManager', 'VirtualMicroscopeSizeData', 'LayoutManager']);
});




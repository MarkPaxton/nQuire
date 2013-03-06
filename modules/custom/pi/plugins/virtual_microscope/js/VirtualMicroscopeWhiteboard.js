


$(function() {
  nQuireJsSupport.register('VirtualMicroscopeWhiteboard', {
    _sizes: null,
    _vmManager: null,
    _transform: null,
    _iframe: null,
    _shapes: null,
    _readyListeners: null,
    init: function(dependencies) {
      this._parent = $('#virtual_microscope_container');
      this._sizes = dependencies.VirtualMicroscopeSizeData.data;
      this._vmManager = dependencies.VirtualMicroscopeManager;
      this._readyListeners = [];

      var self = this;

      dependencies.LayoutManager.addResizeListener(function() {
        self.windowResized();
      });

      this._vmManager.addStatusListener(function(ready) {
        self.vmReady(ready);
      });
    },
    /* drawing functions */

    _drawCreateSvgParent: function(x, y) {
      var parent = this._svg.group(this._svgGroup);
      $(parent).attr('transform', 'translate(' + x + ' ' + y + ')');
      return parent;
    },
    draw: function(name, shape, dontScale) {
      var shapeObj = null, parent = null;



      switch (shape.type) {
        case 'circle':
          if (dontScale) {
            parent = this._drawCreateSvgParent(shape.cx, shape.cy);
            shapeObj = this._svg.circle(parent, 0, 0, shape.r, shape.settings);
          } else {
            shapeObj = this._svg.circle(this._svgGroup, shape.cx, shape.cy, shape.r, shape.settings);
          }
      }

      if (shapeObj) {
        this.remove(name);
        if (dontScale) {
          $(shapeObj).attr('transform', this._transform.labelTransform);
          this._shapes[name] = {obj: parent, dontScale: true, scaleObj: shapeObj};
        } else {
          this._shapes[name] = {obj: shapeObj, dontScale: false};
        }
      }
    },
    remove: function(name) {
      if (this._shapes[name]) {
        this._svgGroup.removeChild(this._shapes[name].obj);
        this._shapes[name] = null;
      }
    },
    clear: function() {
      for (var name in this._shapes) {
        this._svgGroup.removeChild(this._shapes[name].obj);
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
    _fireWhiteboardReadyEvent: function(ready) {
      for (var i in this._readyListeners) {
        this._readyListeners[i](ready);
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

            self._svgElement.customMouseInput('rawdrag', function(action, point) {
              console.log('wb ' + action);
              console.log(point);
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
        }
      }
    },
    vmPosChanged: function(pos) {
      this._transform.viewPos = pos;
      this._transform.centerRelPos = {
        dx: pos.x,// - .5 * this._transform.sectionSize.width,
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
      console.log(wk + '  ' + hk);
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
          if (this._shapes[i].dontScale) {
            $(this._shapes[i].scaleObj).attr('transform', this._transform.labelTransform);
          }
        }
      }
    }
  }, ['VirtualMicroscopeManager', 'VirtualMicroscopeSizeData', 'LayoutManager']);
});




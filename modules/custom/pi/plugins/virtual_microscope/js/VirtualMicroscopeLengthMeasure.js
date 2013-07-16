/*global jquery, JSON*/

$(function() {

  var LengthPainter = function(measureId, dataBrowser, color, actionCallbacks) {
    this._measureId = measureId;
    this._dataBrowser = dataBrowser;
    this._actionCallbacks = actionCallbacks;

    this._activeInput = true;
    this._activeDataValue = null;

    var baseStyle = {
      fill: 'none',
      'stroke-linecap': 'butt',
      'stroke-linejoin': 'round'
    };

    var darkColor = color.replace(/f/g, 'a');

    this._styles = {
      'other': $.extend({stroke: darkColor, strokeWidth: 4}, baseStyle),
      'normal': $.extend({stroke: color, strokeWidth: 4}, baseStyle),
      'selected': $.extend({stroke: color, strokeWidth: 5}, baseStyle),
      'editing': $.extend({stroke: color, strokeWidth: 10, strokeOpacity: .5}, baseStyle)
    };
  };

  LengthPainter.prototype.getMeasureType = function() {
    return 'length';
  };

  LengthPainter.prototype.setPaintValueForSave = function(activeInput, value) {
    this._activeInput = activeInput;
    this._activeDataValue = value;
  };

  LengthPainter.prototype.createPaintShape = function(data, options, index) {
    try {
      var value = options.isSelected && this._activeDataValue ? this._activeDataValue : JSON.parse(data[this._measureId]);
    } catch (e) {
      value = null;
    }

    if (value) {
      var style = options.isSelected ? (this._activeInput ? 'editing' : 'selected') :
              (options.selectedData ? 'other' : 'normal');
      var settings = this._styles[style];

      var yl = value.y11 - value.y12;
      var xl = value.x11 - value.x12;
      var angle = Math.atan2(yl, xl) + .5 * Math.PI;
      var length = .1 * Math.sqrt(xl * xl + yl * yl);
      var dx = length * Math.cos(angle);
      var dy = length * Math.sin(angle);
      
      var scaleFunction = function(group, scale, zoom) {
        $(group).children().attr('stroke-width', settings.strokeWidth * scale);
      };
      
      var output = {
        pos: {x: 0, y: 0},
        shapes: [{
            type: 'line',
            points: {x1: value.x11 - dx, y1: value.y11 - dy, x2: value.x11 + dx, y2: value.y11 + dy},
            settings: settings,
            callbacks: this._actionCallbacks
          }, {
            type: 'line',
            points: {x1: value.x11, y1: value.y11, x2: value.x12, y2: value.y12},
            settings: settings,
            callbacks: this._actionCallbacks
          }, {
            type: 'line',
            points: {x1: value.x12 - dx, y1: value.y12 - dy, x2: value.x12 + dx, y2: value.y12 + dy},
            settings: settings,
            callbacks: this._actionCallbacks
          }],
        scaleFunction: scaleFunction
      };
      return output;
    }

    return null;
  };

  nQuireJsSupport.register('VirtualMicroscopeLengthMeasure', {
    init: function(dependencies) {
      $('div[measure_type="measure_widget_vm_length"]').each(function() {
        var inputElementId = $(this).attr('input_element_id');
        var inputElementTitle = $(this).attr('input_element_title');
        var measureColor = $(this).attr('measure_color');

        var actionCallbacks = {
          click: function(featureName) {
            var id = "" + parseInt(featureName);
            dependencies.AjaxDataService.setData(id);
          }
        };


        var lengthManager = new LengthPainter(inputElementId, dependencies.VirtualMicroscopeDataBrowser, measureColor, actionCallbacks);

        var manager = dependencies.VirtualMicroscopeNumberMeasure.createManager(this, inputElementId, lengthManager);
        dependencies.DynamicMeasureService.registerMeasure(inputElementId, manager);
        dependencies.VirtualMicroscopeDataBrowser.registerPaintFeature(inputElementId, inputElementTitle, lengthManager, true);
      });
    }
  }, ['VirtualMicroscopeNumberMeasure', 'VirtualMicroscopeDataBrowser', 'DynamicMeasureService', 'AjaxDataService']);
});

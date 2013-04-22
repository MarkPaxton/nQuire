/*global jquery, JSON*/

$(function() {
  
  nQuireJsSupport.register('VirtualMicroscopeLengthMeasure', {
    init: function(dependencies) {
      $('div[measure_type="measure_widget_vm_length"]').each(function() {
        var inputElementId = $(this).attr('input_element_id');
        var manager = dependencies.VirtualMicroscopeNumberMeasure.createManager(this, inputElementId, 'angle');
        dependencies.DynamicMeasureService.registerMeasure(inputElementId, manager);
      });
    }
  }, ['VirtualMicroscopeNumberMeasure', 'DynamicMeasureService']);
});



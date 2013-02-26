
$(function() {
  nQuireJsSupport.register('AjaxDataService', {
    _measuresService: null,
    init: function(dependencies) {
      this._measuresService = dependencies.DynamicMeasureService;
    },
    saveData: function() {
      var self = this;
      this._measuresService.endDataInput(function() {
        self._submitData();
      });
    },
    enableDataInput: function() {
      /* TODO hide overlay */
    },
    disableDataInput: function() {
      /* TODO show overlay */
      this._measuresService.stopUserInputProcesses();
    },
    _submitData: function() {

    },
    setData: function(data) {
      this.disableDataInput();
      
      /* TODO update data and notify measure handlers */
      
      this.enableDataInput();      
    }
  }, ['DynamicMeasureService']);
});


$(function() {
  nQuireJsSupport.register('AjaxDataService', {
    _measuresService: null,
    _data: null,
    _container: null,
    _overlay: null,
    init: function(dependencies) {
      this._measuresService = dependencies.DynamicMeasureService;
      this._data = dependencies.AjaxDataServiceInitialData;

      this._setButtonsMode(this._data.current ? 'saved' : 'new');

      this._container = $('#data_form_container');
      this._overlay = $('<div>').addClass('nquire-data-input-overlay').css({
        display: 'none'
      }).appendTo(this._container);
    },
    getCurrentData: function() {
      return this._data.current ? this._data.all[this._data.current] : null;
    },
    getMeasureValue: function(measure) {
      var element = $('[name="measure_' + measure + '"]');
      var value = element.val();
      return value;
    },
    getMeasureHandler: function(measure) {
      return this._measuresService.getMeasureHandler('measure_' + measure);
    },
    clearData: function() {
      var self = this;
      this._setButtonsMode('new');
      $('[name="data_id"]').val('');
      $('[name^="measure_"]').each(function() {
        var element = $(this);
        var handler = self._measuresService.getMeasureHandler(element.attr('name'));
        if (handler) {
          console.log('clear ' + element.attr('name') + ' through handler');
          handler.clearValue();
        } else {
          console.log('clear ' + element.attr('name') + ' w/o handler');
          element.val('');
        }
      });
    },
    saveData: function() {
      var self = this;
      this._measuresService.endDataInput(function() {
        self._submitData();
      });
    },
    enableDataInput: function() {
      this._overlay.css('display', 'none');
    },
    disableDataInput: function() {
      this._measuresService.stopUserInputProcesses();
      this._overlay.css('display', 'block');
    },
    _submitData: function() {

    },
    setData: function(dataId) {
      this.disableDataInput();
      this.clearData();

      if (this._data.all[dataId]) {
        this._data.current = dataId;
        $('[name="data_id"]').val(dataId);
        var data = this._data.all[dataId];
        for (var measure in data) {
          var element = $('[name="' + measure + '"]');
          var handler = self._measuresService.getMeasureHandler(measure);
          if (handler) {
            console.log('set ' + measure + ' through handler');
            handler.initMeasureValue(data[measure]);
          } else {
            console.log('set ' + measure + ' w/o handler');
            element.val(data[measure]);
          }
        }
        this._setButtonsMode('saved');
      } else {
        this._data.current = null;
        this._setButtonsMode('new');
      }
      
      this.enableDataInput();
    },
    _setButtonsMode: function(mode) {
      var s = {};
      switch (mode) {
        case 'new':
          s['savenew'] = 'enabled';
          s['savechanges'] = s['saving'] = s['saved'] = s['newdata'] = s['deletedata'] = 'hidden';
          break;
        case 'saved':
          s['savenew'] = s['saving'] = 'hidden';
          s['savechanges'] = 'disabled';
          s['saved'] = s['newdata'] = s['deletedata'] = 'enabled';
          break;
        case 'modified':
          s['savenew'] = s['saved'] = s['saving'] = 'hidden';
          s['savechanges'] = 'enabled';
          s['newdata'] = s['deletedata'] = 'enabled';
          break;
        case 'saving':
          s['saving'] = 'enabled';
          s['savenew'] = s['saved'] = 'hidden';
          s['savechanges'] = 'hidden';
          s['newdata'] = s['deletedata'] = 'disabledIfShown';
          break;
      }
      for (var id in s) {
        var element = $('#nquire-data-input-button-' + id);
        switch (s[id]) {
          case 'enabled':
            element.show().removeAttr('disabled');
            break;
          case 'disabled':
            element.show().attr('disabled', 'disabled');
            break;
          case 'disabledIfShown':
            element.attr('disabled', 'disabled');
            break;
          case 'hidden':
            element.hide();
            break;
        }
      }
    }
  }, ['AjaxDataServiceInitialData', 'DynamicMeasureService']);
});

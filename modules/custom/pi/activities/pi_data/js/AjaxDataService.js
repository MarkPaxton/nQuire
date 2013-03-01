
$(function() {
  nQuireJsSupport.register('AjaxDataService', {
    _buttonMode: null,
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

      var self = this;
      $('#nquire-data-input-button-savenew').click(function() {
        self._submitData();
        return false;
      });

      var change = function() {
        self._dataChanged();
      };
      this._measuresService.addUserChangeListener(change);
      $('[name^="measure_"]').each(function() {
        var element = $(this);
        if ((element.is('input') && element.attr('type') === 'text') || element.is('textarea')) {
          element.keydown(change);
        } else if (element.is('select') || element.is('radio') || element.is('checkbox')) {
          element.change(change);
        }
      });
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
    clearData: function(enabledAtEnd) {
      var self = this;
      this._setButtonsMode('new');

      var clear = function() {
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
        if (enabledAtEnd) {
          self._enableDataInput();
        }
      };

      if (!this._measuresService.endDataInput(clear) || !enabledAtEnd) {
        this._disableDataInput();
      }
    },
    _enableDataInput: function() {
      this._overlay.css('display', 'none');
    },
    _disableDataInput: function() {
      this._measuresService.stopUserInputProcesses();
      this._overlay.css('display', 'block');
    },
    _dataChanged: function() {
if (this._sample) {
  
}
    },
    _submitData: function() {
      var self = this;

      var processResponse = function(success, data) {
        if (success) {
          var id = data.id;
          self._data.current = id;
          self._data.all[id] = data;
        } else {
          console.log(data);
        }
        self._enableDataInput();
      };
      var submit = function() {
        self._ajaxCall('submit', $('form').serialize(), processResponse);
      };
      this._disableDataInput();
      this._measuresService.endDataInput(submit);
    },
    _ajaxCall: function(op, data, callback) {
      var url = location.origin + location.pathname + '/data/' + op;
      $.ajax({
        url: url,
        type: "POST",
        dataType: 'json',
        data: data,
        success: function(data) {
          if (data.status) {
            callback(true, data.data);
          } else {
            callback(false, data.error);
          }
        },
        error: function(jqXHR, textStatus) {
          callback(false, 'ajax call error: ' + textStatus);
        }
      });
    },
    setData: function(dataId) {
      var data = null;
      if (this._data.all[dataId]) {
        this._data.current = dataId;
        data = this._data.all[dataId];
        this._setButtonsMode('saved');
      } else {
        this._data.current = null;
        data = {};
        this._setButtonsMode('new');
      }

      var self = this;
      var set = function() {
        $('[name="data_id"]').val(typeof data['id'] !== 'undefined' ? data['id'] : '');
        $('[name^="measure_"]').each(function() {
          var element = $(this);
          var measure = element.attr('name');
          var hasValue = typeof data[measure] !== 'undefined';
          var handler = self._measuresService.getMeasureHandler(measure);
          if (handler) {
            console.log('set ' + element.attr('name') + ' through handler');
            if (hasValue) {
              handler.initMeasureValue(data[measure]);
            } else {
              handler.clearValue();
            }
          } else {
            console.log('set ' + element.attr('name') + ' w/o handler');
            if (hasValue) {
              element.val(data[measure]);
            } else {
              element.val('');
            }
          }
        });
        self._enableDataInput();
      };

      if (!this._measuresService.endDataInput(set)) {
        this.disableDataInput();
      }
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

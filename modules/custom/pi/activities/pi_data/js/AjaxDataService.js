
$(function() {
  nQuireJsSupport.register('AjaxDataService', {
    _buttonMode: null,
    _measuresService: null,
    _data: null,
    _container: null,
    _overlay: null,
    _dataListeners: null,
    init: function(dependencies) {
      this._measuresService = dependencies.DynamicMeasureService;
      this._data = dependencies.AjaxDataServiceInitialData;
      this._dataListeners = [];

      this._setButtonsMode(this._data.current ? 'saved' : 'new');

      this._container = $('#data_form_container');
      this._overlay = $('<div>').addClass('nquire-data-input-overlay').css({
        display: 'none'
      }).appendTo(this._container);

      var self = this;
      $('#nquire-data-input-button-savenew, #nquire-data-input-button-savechanges').click(function() {
        self._submitData();
        return false;
      });

      $('#nquire-data-input-button-createdata').click(function() {
        self.setData(null, function() {
          self._fireDataChangeEvent('unselected', null);
        });
        return false;
      });

      var change = function() {
        self._userInputChanged();
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
    addDataListener: function(callback) {
      this._dataListeners.push(callback);
    },
    _fireDataChangeEvent: function(event, data) {
      for (var i in this._dataListeners) {
        this._dataListeners[i](event, data);
      }
    },
    getDataList: function() {
      return this._data.all;
    },
    getData: function(id) {
      return this._data.all[id];
    },
    getCurrentDataId: function() {
      return this._data.current;
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
    _userInputChanged: function() {
      if (this._buttonMode === 'saved') {
        this._setButtonsMode('modified');
      }
    },
    _submitData: function() {
      var self = this;
      var processResponse = function(success, data) {
        if (success) {
          var id = data.id;
          self._data.all[id] = data;
          self.setData(id, function() {
            self._fireDataChangeEvent('modified', data);
          });
        } else {
          console.log(data);
        }
      };
      var submit = function() {
        self._ajaxCall('submit', $('form').serialize(), processResponse);
      };

      this._disableDataInput();
      this._measuresService.prepareToSave(submit);
    },
    _ajaxCall: function(op, data, callback) {
      var urlEnd = location.href.indexOf('?');
      var url = urlEnd ? location.href.substr(0, urlEnd) : location.href;
      url += '/data/' + op;
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
    setData: function(dataId, callbackWhenDone) {
      var data = null;
      var nextButtonsMode = null;
      if (this._data.all[dataId]) {
        this._data.current = dataId;
        data = this._data.all[dataId];
        nextButtonsMode = 'saved';
      } else {
        this._data.current = null;
        data = {};
        nextButtonsMode = 'new';
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
            console.log('set ' + element.attr('name') + ' through handler: ' + data[measure]);
            if (hasValue) {
              element.val(data[measure]);
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
        self._setButtonsMode(nextButtonsMode);
        self._enableDataInput();
        if (callbackWhenDone) {
          callbackWhenDone();
        }
      };

      if (!this._measuresService.endDataInput(set)) {
        this.disableDataInput();
      }
    },
    _setButtonsMode: function(mode) {
      this._buttonMode = mode;
      var s = {};
      switch (mode) {
        case 'new':
          $('#nquire-data-input-header-edit').addClass('nquire-data-input-hidden');
          $('#nquire-data-input-header-new').removeClass('nquire-data-input-hidden');

          s['savenew'] = 'enabled';
          s['savechanges'] = s['saving'] = s['saved'] = s['createdata'] = s['deletedata'] = 'hidden';
          break;
        case 'saved':
          $('#nquire-data-input-header-edit').removeClass('nquire-data-input-hidden');
          $('#nquire-data-input-header-new').addClass('nquire-data-input-hidden');

          s['savechanges'] = s['savenew'] = s['saving'] = 'hidden';
          s['saved'] = s['createdata'] = s['deletedata'] = 'enabled';
          break;
        case 'modified':
          s['savenew'] = s['saved'] = s['saving'] = 'hidden';
          s['savechanges'] = s['createdata'] = s['deletedata'] = 'enabled';
          break;
        case 'saving':
          s['saving'] = 'enabled';
          s['savechanges'] = s['savenew'] = s['saved'] = 'hidden';
          s['createdata'] = s['deletedata'] = 'disabledIfShown';
          break;
      }
      for (var id in s) {
        var element = $('#nquire-data-input-button-' + id);
        switch (s[id]) {
          case 'enabled':
            element.removeClass('nquire-data-input-hidden').removeAttr('disabled');
            break;
          case 'disabled':
            element.removeClass('nquire-data-input-hidden').attr('disabled', 'disabled');
            break;
          case 'disabledIfShown':
            element.attr('disabled', 'disabled');
            break;
          case 'hidden':
            element.addClass('nquire-data-input-hidden');
            break;
        }
      }
    }
  }, ['AjaxDataServiceInitialData', 'DynamicMeasureService']);
});

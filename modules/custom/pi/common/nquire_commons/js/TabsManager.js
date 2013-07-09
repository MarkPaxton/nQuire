

$(function() {
  nQuireJsSupport.register('TabsManager', {
    _container: null,
    _currentTab: null,
    _enabled: null,
    _tabListeners: null,
    init: function() {
      var self = this;
      this._tabListeners = [];
      this._container = $('.nquire-tabs-container');
      this._container.find('.nquire-tabs-tab').click(function() {
        self._tabClicked($(this).attr('tab'));
      });
    },
    getFirstTab: function() {
      return this._container.find('.nquire-tabs-tab:first-child').attr('tab');
    },
    addListener: function(callback) {
      this._tabListeners.push(callback);
    },
    setEnabled: function(enabled) {
      this._enabled = enabled;
    },
    selectTab: function(tab) {
      if (tab !== this._currentTab) {
        this._currentTab = tab;
        this._updateUI();
      }
    },
    _tabClicked: function(tab) {
      if (this._enabled && tab !== this._currentTab) {
        this._currentTab = tab;
        this._updateUI();
        for (var i in this._tabListeners) {
          this._tabListeners[i](this._currentTab);
        }
      }
    },
    _updateUI: function() {
      var self = this;
      this._container.find('.nquire-tabs-tab').each(function() {
        var tab = $(this);
        if (tab.attr('tab') === self._currentTab) {
          tab.addClass('selected');
        } else {
          tab.removeClass('selected');
        }
      });
    }
  });
});

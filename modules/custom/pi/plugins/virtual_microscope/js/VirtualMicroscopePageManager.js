

$(function() {
  nQuireJsSupport.register('VirtualMicroscopePageManager', {
    _homePage: null,
    _vmPage: null,
    _tabsManager: null,
    _vmManager: null,
    _layoutManager: null,
    init: function(dependencies) {
      var self = this;

      this._homePage = $('#virtual-microscope-home');
      this._vmPage = $('#virtual-microscope-sample');

      this._vmManager = dependencies.VirtualMicroscopeManager;
      this._vmManager.addStatusListener(function(ready) {
        if (ready) {
          self._tabsManager.setEnabled(true);
        }
      });
      this._layoutManager = dependencies.LayoutManager;

      this._tabsManager = dependencies.TabsManager;
      this._tabsManager.addListener(function(tab) {
        self._openPage(tab);
      });

      this._tabsManager.selectTab('home');
      this._tabsManager.setEnabled(true);
      this._openPage('home');
    },
    _openPage: function(page) {
      var pages = page === 'home' ? [this._homePage, this._vmPage] : [this._vmPage, this._homePage];
      pages[1].addClass('virtual-microscope-page-hidden');
      pages[0].removeClass('virtual-microscope-page-hidden');

      if (page === 'home') {
        this._vmManager.setSample(null);
      } else {
        this._tabsManager.setEnabled(false);
        this._layoutManager.resizeRoots();
        this._vmManager.setSample(page);
      }
    }
  }, ['LayoutManager', 'TabsManager', 'VirtualMicroscopeManager']);
});

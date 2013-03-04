

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

      $('.virtual-microscope-open').click(function() {
        var sampleId = $(this).parents('.virtual-microscope-sample').attr('item-id');
        self._openPage(sampleId);
        self._tabsManager.selectTab(sampleId);
      });

      this._tabsManager.selectTab('home');
      this._tabsManager.setEnabled(true);
      this._openPage('home');
    },
    openSampleView: function(view) {
      this._openPage(view.sample, view);
      this._tabsManager.selectTab(view.sample);
    },
    _openPage: function(page, view) {
      var pages = page === 'home' ? [this._vmPage, this._homePage] : [this._homePage, this._vmPage];
      pages[0].addClass('virtual-microscope-page-hidden');
      pages[1].removeClass('virtual-microscope-page-hidden');

      if (page === 'home') {
        this._vmManager.setSample(null);
      } else {
        this._tabsManager.setEnabled(false);
        this._vmManager.setSample(view ? view : page);
        this._layoutManager.resizeRoots();
        this._layoutManager.resizeRoots();
      }
    }
  }, ['LayoutManager', 'TabsManager', 'VirtualMicroscopeManager']);
});

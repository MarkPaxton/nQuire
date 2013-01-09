
var nQuireJavascriptModules = {
  _ids: [],
  _modules: {},
  
  init: function() {
    var self = this;
    $(window).load(function() {
      for (var i = self._ids.length - 1; i >= 0; i--) {
        var id = self._ids[i];
        console.log('Init: ' + id);
        self._modules[id].init();
      }
    });
  },
  
  register: function(id, module, dependencies) {
    console.log('Registered: ' + id + (dependencies ? ('[' + dependencies.join(',') + ']') : ''));
    this._modules[id] = module;
    var min = this._ids.length;
    if (dependencies) {
      for (var i = 0; i < dependencies.length; i++) {
        var p = this._ids.indexOf(dependencies[i]);
        if (p >= 0) {
          min = Math.min(min, p);
        }
      }
    } 
    this._ids.splice(min, 0, id);
  }
};

$(function() {
  nQuireJavascriptModules.init();
});


var nQuireJsSupport = {
  _queue: [],
  _ready: {},
  _checkQueue: function() {
    var changed = true;
    while (changed) {
      changed = false;

      for (var i = this._queue.length - 1; i >= 0; i--) {
        var item = this._queue[i];
        var ready = true;
        var dependencies = {};
        for (var j in item.dependencies) {
          var dependencieName = item.dependencies[j];
          if (this._ready[dependencieName]) {
            dependencies[dependencieName] = this._ready[dependencieName];
          } else {
            ready = false;
          }
        }
        if (ready) {
          console.log('init: ' + item.name);
          item.object.init(dependencies);
          this._ready[item.name] = item.object;
          this._queue.splice(i, 1);
          changed = true;          
          break;
        }
      }
    }
  },
          
  register: function(name, object, dependencies) {
    this._queue.push({
      name: name,
      object: object,
      dependencies: dependencies ? dependencies : []
    });
    this._checkQueue();
  }
};



var nQuireListWidget = {
  init: function() {
    $('div[nquire-widget="nquire-list"]').each(function() {
      $(this).nQuireListWidget();
    });
  }
};
$(function() {
  nQuireJavascriptModules.register('nQuireListWidget', nQuireListWidget);
});

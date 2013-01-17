


var nQuireEditFormPosition = {
  init: function() {
    var margin = 10;
    
    var arrowPos = $('.item-cell > .arrow-head').offset().top + margin;
    var form = $('.item-edit-form-container');
    var formBottom = form.offset().top + form.height();
    
    if (arrowPos > formBottom) {
      form.css('top', (arrowPos - formBottom) + 'px');
    }
  }
};
$(function() {
  nQuireJavascriptModules.register('nQuireEditFormPosition', nQuireEditFormPosition);
});

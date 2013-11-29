

$(function() {
  nQuireJsSupport.register('EditFormPosition', {
    init: function() {
      var margin = 10;

      var arrowPos = $('.item-cell.arrow').offset().top + margin;
      var form = $('.item-edit-form-container');
      var formBottom = form.offset().top + form.height();
      
      if (arrowPos > formBottom) {
        form.css('top', (arrowPos - formBottom) + 'px');
        $().scrollTo(form);
      }
    }
  });
});

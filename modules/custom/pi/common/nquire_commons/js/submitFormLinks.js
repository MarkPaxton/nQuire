
var nQuireSubmitFormLinks = {
  init: function() {
    $('a.automatic-form-submit').click(function(event) {
      var url = $(this).attr('href');
      alert(url);
      event.stopPropagation();
      event.preventDefault();
    });
  }
};

$(function() {
  nQuireSubmitFormLinks.init();
});

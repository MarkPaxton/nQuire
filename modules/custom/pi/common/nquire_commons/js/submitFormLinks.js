
var nQuireSubmitFormLinks = {
  launch: function(button) {
    var path = $(button).attr('form_destination');
    var form = $('form[id^="inquiry-creator"]');
    if (path && form.length === 1) {
      var actionItem = form.find('input[name="external_link_action"]');
      var destinationItem = form.find('input[name="form_destination"]');
      if (destinationItem.length === 1 && actionItem.length === 1 && actionItem.attr('value') === 'submit') {
        destinationItem.attr('value', path);
        form.submit();
        return false;
      }
    }
    return true;
  }
};


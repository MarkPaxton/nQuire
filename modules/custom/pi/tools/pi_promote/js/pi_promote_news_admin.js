
$(function() {

  function update($fixedElement) {
    var fixedId = $fixedElement ? $fixedElement.attr('id') : '';

    var fixedValue = $fixedElement ? $fixedElement.val() : 0;

    var items = [];

    $('.news_item_selector select[id!="' + fixedId + '"]').each(function() {
      var $i = $(this);
      if ($i.val() > 0) {
        items.push({id: $(this).attr('id'), weight: parseInt($(this).val())});
      }
    });

    items.sort(function(a, b) {
      return a.weight - b.weight;
    });

    var value = 1;
    for (var i in items) {
      if (value == fixedValue) {
        value++;
      }

      $('#' + items[i].id).val(value++);
    }
  }

  $('.news_item_selector select').change(function() {
    update($(this));
  }
  );
});

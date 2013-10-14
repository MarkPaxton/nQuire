
$(function() {

  function update($fixedElement) {
    var fixedId = $fixedElement ? $fixedElement.attr('id') : '';

    var fixedValue = $fixedElement ? $fixedElement.val() : 0;

    var newsItems = [];
    var menuItems = [];

    $('.news_item_selector select[id!="' + fixedId + '"]').each(function() {
      var $i = $(this);
      if ($i.val() > 0) {
        newsItems.push({id: $(this).attr('id'), weight: parseInt($(this).val())});
      } else if ($i.val() < 0) {
        menuItems.push({id: $(this).attr('id'), weight: parseInt($(this).val())});
      }
    });

    newsItems.sort(function(a, b) {
      return a.weight - b.weight;
    });
    menuItems.sort(function(a, b) {
      return a.weight - b.weight;
    });

    var value = 1;
    for (var i in newsItems) {
      if (value == fixedValue) {
        value++;
      }

      $('#' + newsItems[i].id).val(value++);
    }
    
    value = -10;
    for (var i in menuItems) {
      if (value == fixedValue) {
        value++;
      }

      $('#' + menuItems[i].id).val(value++);
    }
    
  }

  $('.news_item_selector select').change(function() {
    update($(this));
  }
  );
});

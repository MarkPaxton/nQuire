

var PiSortDataFilter = {
  _select: null,
  _table: null,
  init: function() {
    var self = this;
    this._select = $('#pi-sort-data-key-question');

    this._select.change(function() {
      self._filter();
    });

    var currentQuestion = $.cookie('active_key_question');
    if (currentQuestion) {
      this._select.val(currentQuestion);
    } 
    
    this._filter();
  },
  _filter: function() {
    var question = this._select.val();
    $.cookie('active_key_question', question);

    $('div.pi-sort-data-list').each(function() {
      var table = $(this);
      var table_question = table.attr('key_question_id');
      if (table_question === question) {
        table.show();
      } else {
        table.hide();
      }
    });
  }
};

$(function() {
  MoonrockModules.register('PiSortDataFilter', PiSortDataFilter);
});

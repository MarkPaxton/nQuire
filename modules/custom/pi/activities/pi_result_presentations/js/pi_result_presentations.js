

var PiResultPresentations = {
  count: 0,
  init: function() {
    $('#pi-result-presentations-preview-loading, #pi-result-presentations-preview').hide();

    $('input[name="chart_type"]').change(function() {
      PiResultPresentations._upateGrahDef();
    });
    $('#edit-abscissa').change(function() {
      PiResultPresentations._updateAvailableValues(false);
      PiResultPresentations._updateGraphType();
      PiResultPresentations._upateGrahDef();
    });

    $('#edit-ordinate').change(function() {
      PiResultPresentations._updateGraphType();
      PiResultPresentations._upateGrahDef();
    });
    
    var questionSelector = $('select[name="key_question_id"]');
    var currentQuestion = $.cookie('active_key_question');
    if (currentQuestion) {
      questionSelector.val(currentQuestion);
    }
    
    var self = this;
    questionSelector.change(function() {
      $.cookie('active_key_question', questionSelector.val());
      self._upateGrahDef();
    });

    this._updateAvailableValues(true);
    this._updateGraphType();
    this._upateGrahDef();
  },
  getBaseURL: function() {
    var currentpath = window.location.search;
    var pos = currentpath.indexOf("activity/");
    var pos2 = currentpath.indexOf("/", pos + "activity/".length);
    if (pos >= 0 && pos2 >= 0) {
      return currentpath.substr(0, pos2) + "/tempchart.png";
    } else {
      return "?q=pi_chart/tempchart.png";
    }
  },
  _updateAvailableValues: function() {
    var abscissa = $('#edit-abscissa').attr('value');
    console.log("abscissa: " + abscissa);

    var needNewSelection = $('#edit-ordinate').val() == abscissa;

    $('#edit-ordinate > option').each(function() {
      if ($(this).attr('value') === abscissa) {
        $(this).removeAttr('selected');
        $(this).addClass('pi-result-presentations-measures-block-hidden');
      } else {
        $(this).removeClass('pi-result-presentations-measures-block-hidden');
        if (needNewSelection) {
          $('#edit-ordinate').val($(this).attr('value'));
          needNewSelection = false;
        }
      }
    });

  },
  _getChartType: function() {
    return $("input[value='line-bar']")[0].checked ? 'line-bar' : 'histogram';//attr('value');
  },
  _ordinateIsNumeric: function() {
    var ordinate = $('#edit-ordinate').attr('value');
    var numeric = $('#edit-numeric').attr('value').split(' ');
    return numeric.indexOf(ordinate) >= 0;
  },
  _updateGraphType: function() {
    if (this._ordinateIsNumeric()) {
      $('#pi-result-presentations-select-type').removeClass('pi-result-presentations-measures-block-hidden');
    } else {
      $('#pi-result-presentations-select-type').addClass('pi-result-presentations-measures-block-hidden');
    }
  },
  _updateExplanation: function(type) {
    var variableName = function(id) {
      var value = $(id).children().filter(':selected').html();
      var a = value.indexOf('(');
      if (a >= 0) {
        value = value.substr(0, a).trim();
      }
      return value;
    }
    var abscissa = variableName('#edit-abscissa');
    var ordinate = variableName('#edit-ordinate');

    var text;
    if (type === 'histogram') {
      text = 'This graph shows <b>the number of data items</b> that have a specific <i>' + ordinate + '</i> value for each <i>' + abscissa +
              '</i>.<br />It may reveal patterns in the frequency of value pairs, and thus indicate a relationship between variables <i>' + ordinate + '</i> and <i>' + abscissa + '</i>.';
    } else {
      text = 'This graph shows <b>the numeric values</b> of <i>' + ordinate + '</i> for each <i>' + abscissa +
              '</i>.<br />It may reveal patterns in the distribution of <i>' + ordinate + '</i> values, and thus indicate a relationship between variables <i>' + ordinate + '</i> and <i>' + abscissa + '</i>.';
    }

    $('#pi-result-presentations-explanation').html(text);
  },
  _upateGrahDef: function() {
    var title = $('#edit-title').attr('value');

    var type;
    if (this._ordinateIsNumeric()) {
      type = $("input[value='line-bar']")[0].checked ? 'line-bar' : 'histogram';
    } else {
      type = 'histogram';
    }

    if (type === 'histogram') {
      $("input[value='histogram']").attr('checked', 'checked');
      $("input[value='line-bar']").attr('checked', false);
    } else {
      $("input[value='line-bar']").attr('checked', 'checked');
      $("input[value='histogram']").attr('checked', false);
    }

    var question = $('select[name="key_question_id"]').val();

    this._updateExplanation(type);

    var abscissa = $('#edit-abscissa').attr('value');
    var ordinate = $('#edit-ordinate').attr('value');
    var color = $('#edit-color').attr('value');


    if (abscissa.length > 0 && ordinate.length > 0) {
      $('#pi-result-presentations-preview').show();
      $('#pi-result-presentations-preview-no').hide();
      $('#pi-result-presentations-preview-loading').show();
      var url = this.getBaseURL() + "&title=" + title +
              "&count=" + (this.count++) + "&type=" + type +
              "&abscissa=" + abscissa + "&ordinate=" + ordinate +
              "&color=" + color + "&question=" + question;
      $('#pi-result-presentations-preview').attr('src', url).load(function() {
        $('#pi-result-presentations-preview-loading').hide();
      });

    } else {
      $('#pi-result-presentations-preview').hide();
      $('#pi-result-presentations-preview-loading').hide();
      $('#pi-result-presentations-preview-no').show();
    }
  }
};

$(function() {
  MoonrockModules.register('PiResultPresentations', PiResultPresentations);
});

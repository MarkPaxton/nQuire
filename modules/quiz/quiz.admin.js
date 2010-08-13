// $Id: quiz.admin.js,v 1.1.2.4 2009/06/07 23:05:33 sivaji Exp $

/**
 * Scripts for Quiz administration.
 */

var Quiz = Quiz || {};
Quiz.reg = /^([^\[]*)\[type:([\w -_]+), id:([0-9]+)\]$/;

// Key should be either always or random.
Quiz.addQuestion = function (key) {
  var selector = '#edit-' + key + '-autocomplete';
  var selectedItem = $(selector).val();
  var matches = selectedItem.match(Quiz.reg);
  var statusCode = (key == 'always' ? 1 : 2); // e.g. QUIZ_ALWAYS, QUIZ_RANDOM

  if (!matches || matches.length < 4) {
    alert('Error: The entered question was not found.')
    return;
  }

  var title = Drupal.checkPlain(matches[1]);
  var type = Drupal.checkPlain(matches[2]);
  var nid = parseInt(matches[3]);
  var weight = 0;
  $('.question-order-weight-' + statusCode).each(function () {
    var thisVal = parseInt($(this).val());
    if (thisVal > weight) {
      weight = thisVal;
    }
  });
  ++weight;

  var out = Drupal.theme('addQuestion', nid, type, title, key, weight);
  $('#questions-order-' + statusCode + ' tr:last').after(out);

  var newRow = $('#questions-order-' + statusCode + ' tr:last').get(0);

  var table = Drupal.tableDrag['questions-order-' + statusCode];
  table.makeDraggable(newRow);
  if (table.changed == false) {
    table.changed = true;
    $(Drupal.theme('tableDragChangedWarning')).insertAfter(table.table).hide().fadeIn('slow');
  }
  $(selector).val(''); // Zero out the value
  Drupal.attachBehaviors();
}

Drupal.behaviors.attachRemoveAction = function () {
  $('.rem-link').click(function (e) {
    var $this = $(this);
    var remID = $this.parents('tr').find('.question-order-weight').attr('id');

    var matches = remID.match(/edit-weights-([a-zA-Z]+)-([0-9]+)/);
    if (!matches || matches.length < 3) {
      return false;
    }

    var remItem = matches[1] + '-' + matches[2];

    var statusCode = (matches[1] == 'always') ? 1 : 0;

    var remList = $('#edit-remove-from-quiz');
    var orig = remList.val();
    remList.val(remItem + ',' + orig);

    $this.parents('tr').remove();

    var table = Drupal.tableDrag['questions-order-' + statusCode];
    if (!table.changed) {
      table.changed;
      $(Drupal.theme('tableDragChangedWarning')).insertAfter(table.table).hide().fadeIn('slow');
    }

    e.preventDefault();
    return true;
  })
};

Drupal.theme.prototype.addQuestion = function (nid, type, title, state, weight) {
  var fieldID = state + '-' + nid;
  var pre = (location.search.indexOf('?q=') == 0 ? '?q=' : Drupal.settings.basePath);
  var nodeLink = pre + Drupal.encodeURIComponent('node/' + nid);
  var remLink = pre + Drupal.encodeURIComponent('node/' + nid + '/questions/remove');
  var stateCode = (state == 'always') ? 1: 2;
  var actions = 'View'.link(nodeLink) + ' | ' + $('Remove'.link('#')).addClass('rem-link').parent().html();

  return '<tr class="draggable">' +
    '<td>' + title + '<span class="warning tabledrag-changed">*</span></td><td>' + type + '</td><td>' + actions +
    '</td><td style="display:none">' +
    '<div id="edit-weights-' + fieldID + '-wrapper" class="form-item">' +
    '<input id="edit-weights-' + fieldID + '" class="form-text question-order-weight question-order-weight-' + stateCode + '" ' +
    'type="text" value="' + weight + '" size="3" name="weights[' + fieldID + ']" maxlength="4"/>' +
    '</div></td></tr>';
}


$(document).ready(function () {

  // Stupid hack to get around bug in tableDrag (collapsed tableDrag tables cannot have hidden Weight fields)
  $('fieldset.collapsible:last>legend>a').click();

  // Effectively bind the autocomplete submit handler to the
  // "Add question" button's submit handler (for both autocomplete fields).


  $('#edit-always-autocomplete,#edit-random-autocomplete').keypress(function (e) {
      if (e.which == 13) {

        /* We could do something like this....
        $this = $(this);
        if ($this.val().length > 0) {
          Quix.addQuestion();
        }
        */

        // Kill the 'return' handler before the form gets accidentally submitted.
        e.preventDefault();
        e.stopPropagation();
        return true;
      }
  });

});

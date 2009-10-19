// $Id: collapsiblock.js,v 1.4 2008/12/13 05:42:04 nedjo Exp $

Drupal.Collapsiblock = Drupal.Collapsiblock || {};

Drupal.behaviors.collapsiblock = function (context) {
  var cookieData = Drupal.Collapsiblock.getCookieData();
  var slidetype = Drupal.settings.collapsiblock.slide_type;
  var defaultState = Drupal.settings.collapsiblock.default_state;
  var slidespeed = parseInt(Drupal.settings.collapsiblock.slide_speed);
  $('div.block:not(.collapsiblock-processed)', context).addClass('collapsiblock-processed').each(function () {
    var selector = Drupal.settings.collapsiblock && Drupal.settings.collapsiblock.titleSelector ? Drupal.settings.collapsiblock.titleSelector : 'h2';
    var id = this.id;
    var titleElt = $(selector +':first', this);
    if (titleElt.size()) {
      titleElt = titleElt[0];
      // Status values: 1 = not collapsible, 2 = collapsible and expanded, 3 = collapsible and collapsed, 4 = always collapsed
      var stat = Drupal.settings.collapsiblock.blocks[this.id] ? Drupal.settings.collapsiblock.blocks[this.id] : defaultState;
      if (stat == 1) {
        return;
      }

      titleElt.target = $(this).find('div.content');
      $(titleElt)
        .addClass('collapsiblock')
        .click(function () {
          var st = Drupal.Collapsiblock.getCookieData();
          if ($(this).is('.collapsiblockCollapsed')) {
            $(this).removeClass('collapsiblockCollapsed');
            if (slidetype == 1) {
              $(this.target).slideDown(slidespeed);
            }
            else {
              $(this.target).animate({height:'show', opacity:'show'}, slidespeed);
            }

            // Don't save cookie data if the block is always collapsed.
            if (stat != 4) {
              st[id] = 1;
            }
          } 
          else {
            $(this).addClass('collapsiblockCollapsed');
            if (slidetype == 1) {
              $(this.target).slideUp(slidespeed);
            }
            else {
              $(this.target).animate({height:'hide', opacity:'hide'}, slidespeed);
            }

            // Don't save cookie data if the block is always collapsed.
            if (stat != 4) {
              st[id] = 0;
            }
          }
          // Stringify the object in JSON format for saving in the cookie.
          var cookieString = '{ ';
          var cookieParts = [];
          $.each(st, function (id, setting) {
            cookieParts[cookieParts.length] = ' "' + id + '": ' + setting;
          });
          cookieString += cookieParts.join(', ') + ' }';
          $.cookie('collapsiblock', cookieString, {path: Drupal.settings.basePath});
        });
      // Leave active blocks uncollapsed. If the block is expanded, do nothing.
      if (stat ==  4 || (cookieData[id] == 0 || (stat == 3 && cookieData[id] == 'undefined')) && !$(this).find('a.active').size()) {
        $(titleElt).addClass('collapsiblockCollapsed');
        $(titleElt.target).hide();
      }
    }
  });
};

Drupal.Collapsiblock.getCookieData = function () {
  var cookieString = $.cookie('collapsiblock');
  return cookieString ? Drupal.parseJson(cookieString) : {};
};

  


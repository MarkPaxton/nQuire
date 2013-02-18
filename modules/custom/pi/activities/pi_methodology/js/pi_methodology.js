// $Id: block.js,v 1.2 2007/12/16 10:36:53 goba Exp $

/**
 * Move a block in the blocks table from one region to another via select list.
 *
 * This behavior is dependent on the tableDrag behavior, since it uses the
 * objects initialized in that behavior to update the row.
 */
Drupal.behaviors.blockDrag = function(context) {
  var tables = $('table.measures_sort_table');
  tables.each(function() {
    var table = $(this);
    var tableId = table.attr('id');
    var tableDrag = Drupal.tableDrag[tableId]; // Get the blocks tableDrag object.

    // Add a handler for when a row is swapped, update empty regions.
    tableDrag.row.prototype.onSwap = function() {
      checkRegions();
    };

    // A custom message for the blocks page specifically.
    Drupal.theme.tableDragChangedWarning = function() {
      return '<div class="warning">' + Drupal.theme('tableDragChangedMarker') + ' ' + Drupal.t("The changes to these measures will not be saved until the <em>Save</em> button is clicked.") + '</div>';
    };

    // Add a handler so when a row is dropped, update fields dropped into new regions.
    tableDrag.onDrop = function() {
      dragObject = this;

      //alert($(dragObject.rowObject.element).prev('tr'));

      if ($(dragObject.rowObject.element).prev('tr').is('.region-message')) {
        var regionRow = $(dragObject.rowObject.element).prev('tr').get(0);
        var regionName = regionRow.className.replace(/([^ ]+[ ]+)*region-([^ ]+)-message([ ]+[^ ]+)*/, '$2');
        var regionField = $('select.measures_list-region-select', dragObject.rowObject.element);
        var weightField = $('select.measures_list-weight', dragObject.rowObject.element);
        var oldRegionName = weightField[0].className.replace(/([^ ]+[ ]+)*measures_list-weight-([^ ]+)([ ]+[^ ]+)*/, '$2');

        if (!regionField.is('.measures_list-region-' + regionName)) {
          regionField.removeClass('measures_list-region-' + oldRegionName).addClass('measures_list-region-' + regionName);
          weightField.removeClass('measures_list-weight-' + oldRegionName).addClass('measures_list-weight-' + regionName);
          regionField.val(regionName);
        }
      }
    };

    // Add the behavior to each region select list.
    $('select.measures_list-region-select:not(.blockregionselect-processed)', context).each(function() {
      $(this).change(function(event) {
        // Make our new row and select field.
        var row = $(this).parents('tr:first');
        var select = $(this);
        tableDrag.rowObject = new tableDrag.row(row);

        // Find the correct region and insert the row as the first in the region.
        $('tr.region-message', table).each(function() {
          if ($(this).is('.region-' + select[0].value + '-message')) {
            // Add the new row and remove the old one.
            $(this).after(row);
            // Manually update weights and restripe.
            tableDrag.updateFields(row.get(0));
            tableDrag.rowObject.changed = true;
            if (tableDrag.oldRowElement) {
              $(tableDrag.oldRowElement).removeClass('drag-previous');
            }
            tableDrag.oldRowElement = row.get(0);
            tableDrag.restripeTable();
            tableDrag.rowObject.markChanged();
            tableDrag.oldRowElement = row;
            $(row).addClass('drag-previous');
          }
        });

        // Modify empty regions with added or removed fields.
        checkEmptyRegions(table, row);
        // Remove focus from selectbox.
        select.get(0).blur();
      });
      $(this).addClass('blockregionselect-processed');
    });
  });

  var checkRegions = function() {
    var tables = $('table.measures_sort_table');
    tables.each(function() {
      var table = $(this);
      var trs = table.find('tr');

      var empty = true;
      var messageTr = null;
      var regionTr = null;

      function solveCurrentRegion() {
        if (regionTr && messageTr) {
          if (empty) {
            messageTr.removeClass('region-populated').addClass('region-empty');
          } else {
            messageTr.removeClass('region-empty').addClass('region-populated');
          }
          regionTr = messageTr = null;
        }
      }

      trs.each(function() {
        var tr = $(this);
        if (tr.hasClass('region')) {
          solveCurrentRegion();
          regionTr = tr;
          messageTr = null;
          empty = true;
        } else if (tr.hasClass('region-message')) {
          messageTr = tr;
        } else if (tr.hasClass('draggable')) {
          /* FIX (eloy) for some reason couldn't move measures to lower regions */ 
          if (regionTr) {
            var regionName = regionTr.attr('measure_region');
            var regionSelect = tr.find('select.measures_list-region-select');
            
            regionSelect.val(regionName);
          }
          empty = false;
        }
      });

      solveCurrentRegion();
    });
  };
};

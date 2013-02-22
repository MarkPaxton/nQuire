
$(function() {
  nQuireJsSupport.register('PiDataSpreadsheet', {
    init: function(dependencies) {
      var grid = new editgrid.Grid({
        sessionKey: dependencies.PiDataSpreadsheetSessionKey.sessionKey,
        suppressSessionKeyWarning: 1}
      );
      var layout = grid.getStandardLayout();
      layout.doLayout(document.getElementById('pi_data_spreadsheet_grid_container'), grid);
      grid.openBook({path: 'user/evilfer/' + dependencies.PiDataSpreadsheetSessionKey.bookId});
    }
  }, ['PiDataSpreadsheetSessionKey']);
});

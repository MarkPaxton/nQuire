
$(function() {
	nQuireJsSupport.register('PiDataSpreadsheet', {
		loadGrid: function(data) {
			var options = {
				bookAccessToken: data.bookAccessToken
			}; 

			var grid = new editgrid.Grid(options);
			var layout = grid.getStandardLayout();
			layout.doLayout(document.getElementById('pi_data_spreadsheet_grid_container'), grid);
			grid.openBook({path: 'user/evilfer/' + data.bookId});
		},
		init: function(dependencies) {
			this.loadGrid(dependencies.PiDataSpreadsheetSessionKey);
		}
	}, ['PiDataSpreadsheetSessionKey']);
});

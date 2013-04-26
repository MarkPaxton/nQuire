
$(function() {
	nQuireJsSupport.register('PiDataSpreadsheet', {
		loadGrid: function(data) {
			var grid = new editgrid.Grid({
				sessionKey: data.sessionKey,
				suppressSessionKeyWarning: 1
			}
			);
			var layout = grid.getStandardLayout();
			layout.doLayout(document.getElementById('pi_data_spreadsheet_grid_container'), grid);
			grid.openBook({path: 'user/evilfer/' + data.bookId});
		},
		init: function(dependencies) {
			var self = this;
			if (dependencies.PiDataSpreadsheetSessionKey.readonly) {
				var iframe = $('<iframe style="display:none;">').appendTo('body').attr('onload', function() {
					self.loadGrid(dependencies.PiDataSpreadsheetSessionKey);
				}).attr('src', 'http://www.editgrid.com/site/logout');

			} else {
				this.loadGrid(dependencies.PiDataSpreadsheetSessionKey);
			}
		}
	}, ['PiDataSpreadsheetSessionKey']);
});



$(function() {
	nQuireJsSupport.register('InquiryStructureShareContentTable', {
		options: {
			defaultColumn: 'Share with',
			canRename: false,
			freeTextColumn: false,
			allowCreate: false,
			allowEmpty: false,
			values: {
				individual_no: 'Just me',
				individual_group: 'Group members',
				individual_all: 'Inquiry participants',
				group_no: 'Just group members',
				group_all: 'Other groups',
				all_no: 'Inquiry participants'
			},
			_valueSubset: function(keys) {
				var values = {};
				for(var i in keys) {
					values[keys[i]] = this.values[keys[i]];
				}
				return values;
			},
			defaultValueForItem: function(row) {
				switch (row.attr('collaboration')) {
					case 'individual':
						return row.attr('use_groups') === '1' ? 'individual_group' : 'individual_no';
					case 'group':
						return 'group_no';
					default:
						return 'all_no';
				}
			},
			validValuesForItem: function(row) {
				var keys = null;
				switch (row.attr('collaboration')) {
					case 'individual':
						keys = row.attr('use_groups') === '1' ? ['individual_no', 'individual_group', 'individual_all'] : ['individual_no', 'individual_all'];
						break;
					case 'group':
						keys = ['group_no', 'group_all'];
						break;
					default:
						keys = ['all_no'];
						break;
				}
				
				return this._valueSubset(keys);
			},
			forItemType: 'activity'
		},
		init: function() {
			var self = this;
			$("div[nquire-widget='inquiry-sharecontent']").each(function() {
				$(this).structureFeatureTable(self.options);
			});
		}
	});
});


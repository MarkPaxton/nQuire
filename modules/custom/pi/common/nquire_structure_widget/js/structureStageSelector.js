
$(function() {
	nQuireJsSupport.register('InquiryStageSelector', {
		init: function(dependencies) {
			$("div[nquire-widget='inquiry-stageselector']").each(function() {
				$(this).structureFeatureSelector({
					columnValues: dependencies.InquiryStagePhaseValues.stages,
					selection: dependencies.InquiryStagePhaseValues.highlightColumn ? 'none' : 'single',
					highlightColumn: dependencies.InquiryStagePhaseValues.highlightColumn,
					labels: {
						hidden: 'Not visible',
						view: 'View',
						edit: 'Edit'
					},
          selectedLabel: "Active stage"
				});
			});
		}
	}, ['InquiryStagePhaseValues']);
});


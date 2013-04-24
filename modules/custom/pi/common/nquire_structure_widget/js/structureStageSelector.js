
$(function() {
	nQuireJsSupport.register('InquiryStageSelector', {
		init: function(dependencies) {
			$("div[nquire-widget='inquiry-stageselector']").each(function() {
				$(this).structureFeatureSelector({
					columnValues: dependencies.InquiryStagePhaseValues.stages,
					labels: {
						hidden: 'Not visible',
						view: 'View',
						edit: 'Edit'
					}
				});
			});
		}
	}, ['InquiryStagePhaseValues']);
});


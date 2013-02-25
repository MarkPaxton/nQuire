
$(function() {
  nQuireJsSupport.register('InquiryStructureStagesTable', {
    options: {
      defaultColumn: 'Stage 1',
      freeTextColumn: true,
      defaultValue: 'view',
      allowCreate: true,
      newButtonLabel: 'Add stage',
      newDialogTitle: 'Enter the name of the new stage:',
      deleteDialogTitle: 'Are you sure to delete this stage?',
      renameDialogTitle: 'Changing the stage name:',
      allowEmpty: false,
      values: {
        hidden: 'Not visible',
        view: 'Visible',
        edit: 'Editable'
      },
      forItemType: 'phase'
    },
    init: function() {
      var self = this;
      $("div[nquire-widget='inquiry-stages']").each(function() {
        $(this).structureFeatureTable(self.options);
      });
    }
  });
});


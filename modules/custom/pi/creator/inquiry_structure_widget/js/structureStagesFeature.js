


var nQuireStructureStageTable = {
  options: {
    defaultColumn: 'Stage 1',
    freeTextColumn: true,
    defaultValue: 'view',
    allowCreate: true,
    newButtonLabel: 'Add stage',
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
};

$(function() {
  nQuireJavascriptModules.register('nQuireStructureStageTable', nQuireStructureStageTable);
});


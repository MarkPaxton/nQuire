


var nQuireStructureStageTable = {
  options: {
    defaultColumn: 'New Role',
    freeTextColumn: true,
    defaultValue: 'noaccess',
    allowCreate: true,
    newButtonLabel: 'Add role',
    allowEmpty: true,
    values: {
      noaccess: 'No access',
      view: 'View',
      comment: 'Comment',
      edit: 'Edit'
    },
    forItemType: 'activity'
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


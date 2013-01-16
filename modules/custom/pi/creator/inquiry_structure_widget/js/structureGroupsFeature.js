


var nQuireStructureGroupTable = {
  options: {
    defaultColumn: 'Grouping',
    canRename: false,
    freeTextColumn: false,
    defaultValue: 'individual',
    allowCreate: false,
    allowEmpty: false,
    values: {
      individual: 'Individual',
      group: 'In groups',
      communitary: 'Communitary'
    },
    forItemType: 'phase'
  },
  init: function() {
    var self = this;
    $("div[nquire-widget='inquiry-groups']").each(function() {
      $(this).structureFeatureTable(self.options);
    });
  }
};

$(function() {
  nQuireJavascriptModules.register('nQuireStructureGroupTable', nQuireStructureGroupTable);
});


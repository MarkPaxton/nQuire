
$(function() {
  nQuireJsSupport.register('InquiryStructureRolesTable', {
    options: {
      defaultColumn: 'New Role',
      freeTextColumn: true,
      defaultValue: 'noaccess',
      allowCreate: true,
      newButtonLabel: 'Add role',
      newDialogTitle: 'Enter the name of the new role:',
      deleteDialogTitle: 'Are you sure to delete this role?',
      renameDialogTitle: 'Changing the role name:',
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
      $("div[nquire-widget='inquiry-roles']").each(function() {
        $(this).structureFeatureTable(self.options);
      });
    }
  });
});


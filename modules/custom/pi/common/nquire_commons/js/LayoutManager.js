

$(function() {
  nQuireJsSupport.register('LayoutManager', {
    init: function() {
      var self = this;

      $(window).resize(function() {
        self.resizeRoots();
      });

      self.resizeRoots();
    },
    resizeRoots: function() {
      var self = this;
      $('div.nquire-layout-root').each(function() {
        self.resizeRoot($(this));
      });
    },
    resizeRoot: function(element) {
      var offset = element.offset();
      var availableH = $(window).height() - offset.top - 30;
      var availableW = $(window).width() - offset.left - 20;

      element.css({
        width: availableW,
        height: availableH
      });
      this.resizeChildren(element);
      return;
    },
    resizeChildren: function(parent) {
      var self = this;

      var direction = parent.attr('nquire-layout-direction');
      var dirSizeAttr, crossSizeAttr, dirSize, crossSize, dirPosAttr, crossPosAttr;
      if (direction === 'vertical') {
        dirSizeAttr = 'height';
        crossSizeAttr = 'width';
        dirPosAttr = 'top';
        crossPosAttr = 'left';
      } else {
        dirSizeAttr = 'width';
        crossSizeAttr = 'height';
        dirPosAttr = 'left';
        crossPosAttr = 'top';
      }

      dirSize = parent[dirSizeAttr]();
      crossSize = parent[crossSizeAttr]();

      var requiredSize = 0;
      var flexChildrenCount = 0;
      parent.children('.nquire-layout').each(function() {
        var child = $(this);
        if (child.attr('nquire-layout-flex') === '1') {
          flexChildrenCount++;
        } else {
          if (child.attr('size')) {
            child.css(dirSizeAttr, child.attr('size'));
          }
          requiredSize += child[dirSizeAttr]();
        }
      });

      var flexSize = flexChildrenCount > 0 ? Math.max(dirSize - requiredSize, 0) / flexChildrenCount : 0;
      var currentDirPos = 0;
      parent.children('.nquire-layout').each(function() {
        var child = $(this);
        child.css(crossSizeAttr, crossSize);
        child.css(crossPosAttr, 0);
        if (child.attr('nquire-layout-flex') === '1') {
          child.css(dirSizeAttr, flexSize);
        }

        child.css(dirPosAttr, currentDirPos);
        currentDirPos += child[dirSizeAttr]();

        if (!child.hasClass('nquire-layout-leaf')) {
          self.resizeChildren(child);
        }
      });
    }
  });
});



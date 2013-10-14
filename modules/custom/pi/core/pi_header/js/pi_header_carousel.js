

$(function() {
  nQuireJsSupport.register('HeaderCarousel', {
    currentItem: 1,
    config: null,
    task: null,
    init: function(dependencies) {
      var self = this;
      this.config = dependencies.HeaderCarouselPath;
      this.highlightDot(this.currentItem);
      this.schedule();
      $('.pi_header_carousel_item').click(function() {
        self.setItem(parseInt($(this).attr('item')), false);
        self.schedule();
      });

      $('#pi_header_image_top').css('opacity', 0);
    },
    schedule: function() {
      var self = this;
      clearInterval(this.task);
      this.task = setInterval(function() {
        self.setItem(self.currentItem === 4 ? 1 : self.currentItem + 1, true);
      }, 10000);
    },
    setItem: function(index, animate) {
      if (index !== this.currentItem) {
        if (animate) {
          $('#pi_header_image_top').css('opacity', 0);
          $('#pi_header_image_top').attr('src', this.config.path + 'home_' + this.currentItem + '.png');
          $('#pi_header_image_top').css('opacity', 1);
          this.currentItem = index;
          $('#pi_header_image').attr('src', this.config.path + 'home_' + this.currentItem + '.png');
          $('#pi_header_image_top').animate({opacity: 0}, 1000);
        } else {
          $('#pi_header_image_top').css('opacity', 0);
          this.currentItem = index;
          $('#pi_header_image').attr('src', this.config.path + 'home_' + this.currentItem + '.png');
        }
        this.highlightDot(index);
      }
    },
    highlightDot: function(index) {
      $('.pi_header_carousel_item[item=' + index + ']').addClass('pi_header_carousel_active');
      $('.pi_header_carousel_item[item!=' + index + ']').removeClass('pi_header_carousel_active');
    }
  }, ['HeaderCarouselPath']);
});
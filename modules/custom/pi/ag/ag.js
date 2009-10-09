/*
 * jQuery ag stuff
 *
 * Date: 2009-10-02
 */
 
var inquiry_count;
var prev_inquiry;
var current_inquiry;
var next_inquiry;
var current_stage;
var pulloutopen = 0;

$(document).ready(function() {

  // toggles the slickbox on click the noted link
  $('a#inquiry-toggle').click(function() {
    $('#ag_pullout').toggle(400);
    if (pulloutopen == 1) {
      $('img#ag_handle').attr({src:"images/unit-navigator-open.png"});
      pulloutopen = 0;
    }
    else {
      $('img#ag_handle').attr({src:"images/unit-navigator-close.png"});
      pulloutopen = 1;
    };
    return false;
  });
  
  // tabs show on mouseover
  $(function() {
  	$(".tabs").tabs({
  		event: 'mouseover'
  	});
  });
  
  // tabs reset on mouseout
  $('div.ag_inquiry').mouseout(
    function(){
      set_stages();
    }
  );
  
  $('a#ag_prev').click(function (){
    inquiry_rotate_up();
  });
  
  $('a#ag_next').click(function (){
    inquiry_rotate_down();
  });
  
  
  function inquiry_rotate_up() {
    prev_inquiry = current_inquiry;
    next_inquiry = (current_inquiry + 1) % inquiry_count;
    $("div.ag_inquiry:eq(" + next_inquiry + ")").css('top','80px')
    $("div.ag_inquiry:eq(" + current_inquiry + ")").animate({top: -70},"slow");
    $("div.ag_inquiry:eq(" + next_inquiry + ")").show().animate({top: 5},"slow");
    current_inquiry = next_inquiry;
  };
  
  function inquiry_rotate_down() {
    prev_inquiry = current_inquiry - 1;
    if (prev_inquiry < 0) {
      prev_inquiry = inquiry_count - 1;
      prev_inquiry = prev_inquiry % inquiry_count;
    };
    next_inquiry = current_inquiry;
    $("div.ag_inquiry:eq(" + prev_inquiry + ")").css('top','-70px')
    $("div.ag_inquiry:eq(" + current_inquiry + ")").animate({top: 80},"slow");
    $("div.ag_inquiry:eq(" + prev_inquiry + ")").show().animate({top: 5},"slow");
    current_inquiry = prev_inquiry;
  };
  
  function set_stages() {
    var j = 0;
    $("div.ag_inquiry").each(
      function(){
        current_stage = $("div.ag_inquiry:eq("+j+")").attr("stage");
        $("div.ag_inquiry:eq("+j+")").find(".tabs").tabs('select', current_stage);
        j = j + 1;
      }
    );
  };
  
  // hides the slickbox as soon as the DOM is ready
  // (a little sooner than page load)
  $('#ag_pullout').hide();
  
  // set tabs to select currentstage tab
  set_stages();
  
  // calculate number of inquiries and put current inquiry at the top
  inquiry_count = $("div.ag_inquiry").size();
  current_inquiry = $("#ag_currentinquiry").text() - 1;
  $("div.ag_inquiry:eq("+current_inquiry+")").css('top','5px');

});

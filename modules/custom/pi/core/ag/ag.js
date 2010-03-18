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
var ag_width = 465;

function pausecomp(millis)
{
    var date = new Date();
    var curDate = null;

    do {
        curDate = new Date();
    }
    while(curDate-date < millis);
}

$(document).ready(function() {

    // toggles the slickbox on click the noted link
    $('#ag_guide a#inquiry-toggle').click(function() {
        $('#ag_guide  #ag_pullout').toggle(400, resize_ag);
        if (pulloutopen == 1) {
            $('#ag_guide img#ag_handle').attr({
                src:"images/unit-navigator-open.png"
            });
            pulloutopen = 0;
        }
        else {
            $("#ag_guide").css('width','550px');
            $('#ag_guide img#ag_handle').attr({
                src:"images/unit-navigator-close.png"
            });
            pulloutopen = 1;
        }
        return false;
    });

    function resize_ag () {
        if (pulloutopen == 0) {
            $("#ag_guide").css('width', '200px');
        }
    }

    // tabs reset on mouseout
    $('#ag_guide div.ag_inquiry').mouseout(function(){
        reset_stages();
    });
    
    $('#ag_guide a#ag_prev').click(function (){
        inquiry_rotate_up();
    });
  
    $('#ag_guide a#ag_next').click(function (){
        inquiry_rotate_down();
    });
  
    // tabs show on mouseover
    $('#ag_guide .tabs').tabs({
        event: 'mouseover'
    });
    
    function inquiry_rotate_up() {
        prev_inquiry = current_inquiry;
        next_inquiry = (current_inquiry + 1) % inquiry_count;
        $("#ag_guide div.ag_inquiry:eq(" + next_inquiry + ")").css('top','80px');
        $("#ag_guide div.ag_inquiry:eq(" + current_inquiry + ")").animate({
            top: -70
        },"slow");
        $("#ag_guide div.ag_inquiry:eq(" + next_inquiry + ")").show().animate({
            top: 5
        },"slow");
        current_inquiry = next_inquiry;
    };
  
    function inquiry_rotate_down() {
        prev_inquiry = current_inquiry - 1;
        if (prev_inquiry < 0) {
            prev_inquiry = inquiry_count - 1;
            prev_inquiry = prev_inquiry % inquiry_count;
        };
        next_inquiry = current_inquiry;
        $("#ag_guide div.ag_inquiry:eq(" + prev_inquiry + ")").css('top','-70px');
        $("#ag_guide div.ag_inquiry:eq(" + current_inquiry + ")").animate({
            top: 80
        },"slow");
        $("#ag_guide div.ag_inquiry:eq(" + prev_inquiry + ")").show().animate({
            top: 5
        },"slow");
        current_inquiry = prev_inquiry;
    };
  
    function reset_stages() {
        var j = 0;
        $("#ag_guide div.ag_inquiry").each(
            function(){
                current_stage = $("#ag_guide div.ag_inquiry:eq("+j+")").attr("stage");
                $("#ag_guide div.ag_inquiry:eq("+j+")").find(".tabs").tabs('select', current_stage);
                j = j + 1;
            }
            );
    };
  
    // hides the slickbox as soon as the DOM is ready
    // (a little sooner than page load)
    $('#ag_guide #ag_pullout').hide();
    $('#sidebar-right div.block-pi_selected_measures div.block-inner').hide();
  
    // set tabs to select currentstage tab
    reset_stages();
  
    // calculate number of inquiries and put current inquiry at the top
    inquiry_count = $("#ag_guide div.ag_inquiry").size();
    current_inquiry = $("#ag_guide #ag_currentinquiry").text() - 1;
    $("#ag_guide div.ag_inquiry:eq("+current_inquiry+")").css('top','5px');
  
});

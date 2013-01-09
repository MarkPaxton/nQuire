var ajaxLoading = false;
Drupal.behaviors.ajaxLinksApi = function (context) {
  $("a.ajax_link").click(function(e) {
    e.preventDefault();
	if(!ajaxLoading) {
      ajaxLoading = true;
      var url = $(this).attr("href");
      var id = $(this).attr("rel");
      $(id).html('<div class="loading"></div>');
      ajaxLink(id,url);
	}
  });
};
function  ajaxLink(id, url) {
  $.ajax({
    url: url,
    type: "GET",
    data: "ajax=1",
    success: function (data, textStatus, xhr) {
      $(id).html(data);
	  var title = data.match("<title>(.*?)</title>")[1];
	  Drupal.attachBehaviors(id);
	  
	  // you can uncomment this in case you want title and url change in browser window.
	  /*
	  window.history.pushState( {} , document.title, window.location.href ); // change title and url	
	  window.history.replaceState( {} , title, url ); // Change url.
	  document.title = title; // Since title is not changing with window.history.replaceState(), manually change title. Possible bug.
	  */	  
    },
    error: function (xhr, textStatus, errorThrown) {
      $(id).html("Access denied or page not found");
    }
  });
}

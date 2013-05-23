// $Id: suckerfish.js,v 1.2 2010/07/12 22:13:12 aross Exp $ 

sfHover = function() {
	var sfEls = document.getElementById("navigation").getElementsByTagName("li");
	for (var i=0; i<sfEls.length; i++) {
		sfEls[i].onmouseover=function() {
			this.className+=" sfhover";
		}
		sfEls[i].onmouseout=function() {
			this.className=this.className.replace(new RegExp(" sfhover\\b"), "");
		}
	}
}
if (window.attachEvent) window.attachEvent("onload", sfHover);

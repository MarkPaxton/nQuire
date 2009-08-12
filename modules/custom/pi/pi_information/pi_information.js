var currentItem='item1';

function hideme(id) {
  document.getElementById(id).style.display = 'none';
}
		
function showme(id) {
  document.getElementById(id).style.display = 'block';
}
		
function show(id) {
  hideme(currentItem);
  currentItem = id;
  showme(id);
}


var currentTab='tab1';

function setActiveTab(tabID) { 
  
  var prevTabElem = document.getElementById(currentTab); 
  var currTabElem = document.getElementById(tabID); 
  
  prevTabElem.setAttribute('class', 'inactive'); 
  prevTabElem.setAttribute('className', 'inactive'); 
  
  currTabElem.setAttribute('class', 'active'); 
  currTabElem.setAttribute('className', 'active'); 
  
  currentTab = tabID;
  
  return;
} 



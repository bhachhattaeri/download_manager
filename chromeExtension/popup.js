// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// This extension demonstrates using chrome.downloads.download() to
// download URLs.
var allLinks = [];
var visibleLinks = [];
var directories = [];
var fileTypes = [];
var FILE_TYPES;

// NOT USED PLEASE DELETE
function showLinks() {
  var linksTable = document.getElementById('links');
  while (linksTable.children.length > 1) {
    linksTable.removeChild(linksTable.children[linksTable.children.length - 1])
  }
  for (var i = 0; i < visibleLinks.length; ++i) {
    var row = document.createElement('tr');
    var col0 = document.createElement('td');
    var col1 = document.createElement('td');
    var checkbox = document.createElement('input');
    checkbox.checked = true;
    checkbox.type = 'checkbox';
    checkbox.id = 'check' + i;
    col0.appendChild(checkbox);
    col1.innerText = visibleLinks[i];
    col1.style.whiteSpace = 'nowrap';
    col1.onclick = function() {
      checkbox.checked = !checkbox.checked;
    }
    row.appendChild(col0);
    row.appendChild(col1);
    linksTable.appendChild(row);
  }
}
// Toggle the checked state of all visible links.
function toggleAll() {
  var checked = document.getElementById('toggle_all').checked;
  for (var i = 0; i < visibleLinks.length; ++i) {
    document.getElementById('check' + i).checked = checked;
  }
}
// Download all visible checked links.
function downloadCheckedLinks() {
  var ws = new WebSocket("ws://localhost:1234/");
  var checkedNumber = 0;
  for (var i = 0; i < visibleLinks.length; ++i) {
    if (document.getElementById('check' + i).checked) {
        checkedNumber++;
    }
  }
  var sendingString = "" + checkedNumber + "\t";
  for (var i = 0; i < visibleLinks.length; ++i) {
    if (document.getElementById('check' + i).checked) {
      var url = visibleLinks[i];
      sendingString += (url + "\n");
      sendingString += (getDirectoryFromUrl(url) + "\n");
      sendingString += "\n";
      sendingString += "\t"
    }
  }
    
  if(checkedNumber > 0) {
      ws.onopen = function()
               {
                  // Web Socket is connected, send data using send()
                  ws.send(sendingString);
               };
  }

  ws.onclose = function(){};
    alert(sendingString);
}

function getDirectoryFromUrl(url) {
  fileExtensionLocation = url.lastIndexOf(".");  
  if(fileExtensionLocation === -1) {
    return directories[FILE_TYPES]; // default
  }

  fileExtensionLocation++;
  for(var i = 0; i < FILE_TYPES; i++) {
    if(url.substring(fileExtensionLocation)== fileTypes[i]) {
      return directories[i];
    }
  }
  return directories[FILE_TYPES];
}

function addToLinks() {
    var linksTable = document.getElementById('links');
    
    var urlObject = document.getElementById('url');
    visibleLinks.push(urlObject.value);
    
    var row = document.createElement('tr');
    var col0 = document.createElement('td');
    var col1 = document.createElement('td');
    var checkbox = document.createElement('input');
    checkbox.checked = true;
    checkbox.type = 'checkbox';
    i = visibleLinks.length - 1; 
    checkbox.id = 'check' + i;
    col0.appendChild(checkbox);
    col1.innerText = visibleLinks[i];
    col1.style.whiteSpace = 'nowrap';
    col1.onclick = function() {
      checkbox.checked = !checkbox.checked;
    }
    row.appendChild(col0);
    row.appendChild(col1);
    linksTable.appendChild(row);
    
}
// NOT USED PLEASE DELETE
// Re-filter allLinks into visibleLinks and reshow visibleLinks.
function filterLinks() {
  var filterValue = document.getElementById('filter').value;
  if (document.getElementById('regex').checked) {
    visibleLinks = allLinks.filter(function(link) {
      return link.match(filterValue);
    });
  } else {
    var terms = filterValue.split(' ');
    visibleLinks = allLinks.filter(function(link) {
      for (var termI = 0; termI < terms.length; ++termI) {
        var term = terms[termI];
        if (term.length != 0) {
          var expected = (term[0] != '-');
          if (!expected) {
            term = term.substr(1);
            if (term.length == 0) {
              continue;
            }
          }
          var found = (-1 !== link.indexOf(term));
          if (found != expected) {
            return false;
          }
        }
      }
      return true;
    });
  }
  showLinks();
}
// NOT USED PLEASE DELETE
chrome.extension.onRequest.addListener(function(links) {
  for (var index in links) {
    allLinks.push(links[index]);
  }
  allLinks.sort();
  visibleLinks = allLinks;
  showLinks();
});
// Set up event handlers and inject send_links.js into all frames in the active
// tab.
window.onload = function() {
  document.getElementById('download1').onclick = downloadCheckedLinks;
  document.getElementById('add').onclick = addToLinks;
      
  fileTypes[0] = "jpg";
  directories[0] = "/Downloads/Images/";
    
  fileTypes[1] = "mov";
  directories[1] = "/Downloads/Movies/";
    
  fileTypes[2] = "doc";
  directories[2] = "/Downloads/Documents/";
    
  fileTypes[3] = "default";
  directories[3] = "/Desktop/";
    
  FILE_TYPES = 3;
    
};
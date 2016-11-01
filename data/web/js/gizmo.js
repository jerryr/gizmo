function getConfig() {
  var config = getConfigFromServer();
  if(config && config.updateUrl) {
    var textbox = document.getElementById("update-url");
    textbox.value = config.updateUrl;
  }
}

function getConfigFromServer() {
  var path = "/api/bootstrap";
  var xhr = new XMLHttpRequest();
  var config = {};
  xhr.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      config = JSON.parse(this.responseText);
    }
  };
  xhr.open("GET", path, false);
  xhr.send();
  return config;
}

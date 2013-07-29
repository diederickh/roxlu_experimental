function el(name) {
  return document.getElementById(name);
}

function popup(url, width, height, title) {
  var left = (screen.width/2)-(width/2);
  var top = (screen.height/2)-(height/2);
  return window.open(url, title, 'toolbar=no, location=no, directories=no, status=no, menubar=no, scrollbars=no, resizable=no, copyhistory=no, width='+width+', height='+height+', top='+top+', left='+left);
}

function youtube_init() {
  var next0 = el('next0');
  var client_el = el('client_id');

  next0.onclick = function() {
    var youtube_url = "https://accounts.google.com/o/oauth2/auth?client_id=" +client_el.value +"&redirect_uri=urn:ietf:wg:oauth:2.0:oob&response_type=code&scope=https://www.googleapis.com/auth/youtube.upload";
    popup(youtube_url, 700,500, "Get access token");
  };

}

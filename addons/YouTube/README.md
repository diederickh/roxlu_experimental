# YouTube

Implementation of the YouTube data API which allows you to upload videos directly to YouTube.

To use this addon you need to create an YouTube application:

- Create a Google Application:
  - Go to [Google API Console](https://code.google.com/apis/console) and create a new project. [See this page](https://developers.google.com/youtube/registering_an_application) for more information on how to do that (just 2 steps)
  - Make sure that you enable the **YouTube Data API v3** in the servics pane.
  - Click on the **API Access** menu item, then on that big blue button **Create an oAuth client ID...**, create an `installed application`
- Go to the datapath of your application copy `youtube_example.cfg` to `youtube.cfg` (see the contents of this `youtube_example.cfg` below in case you can't find this file.
- After you've created a YouTube application, open the `youtube.cfg` file and copy the `client ID` and `client SECRET` into this file. Next step is to authorize this application by an specifc google account. 
- Open `roxlu\addons\YouTube\html\index.html` in Chrome, paste the `client ID` into the field and press `GO`. Accept the permission popup, and copy the `auth-code` you get into `youtube.cfg`.

_youtube_example.cfg_
````json
{
   "client_id": "YOUR_APPLICATION_CLIENT_ID_HERE",
   "client_secret": "YOUR_APPLICATION_SECRET_HERE",
   "auth_code": "YOUR_AUTHORIZATION_CODE_HERE"
}
````

_Pseudo code that shows how to use this addon_

````c++
YouTube yt;

// Set the client-ID and client-Secret (see the documentation, you get these from the API Console)
if(!yt.setup(client_id, client_secret)) {
  RX_ERROR("Cannot setup the youtube API wrapper.");
  return false;
}

// `auth_code` is assumed to be an authorized access token.
if(!yt.hasAccessToken()) {
  RX_VERBOSE("Fetching access token");
  if(!yt.exchangeAuthCode(auth_code)) {
    RX_ERROR("Error while trying to exchagne the auth code for an access token");
    return false;
  }
}

// add a video to the uplaod queue
YouTubeVideo video;
video.filename = rx_to_data_path("test.mov");
video.datapath = false;
video.title = "automated";
yt_client.addVideoToUploadQueue(video);

// and perform all actions (async)
while(true) {
   yt.update();
}

````
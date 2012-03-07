Twitter
==================================

Creating a twitter application
--------------------------------------------------------------------------------

Before you can use Twitter in your (openFrameworks) application, make sure to
register it at `Twitter developers <http://dev.twitter.com/>`_. Once you created 
an application you'll need the application consumer key and consumer secret.

Authorizing your application
--------------------------------------------------------------------------------

In your application include the "Twitter.h" file and create the neccesary
objects. I you want to do status updates, get the timeline, etc.. you need
a :cpp:class:`Twitter` object. When you want to connect to the streaming server, 
you also need the :cpp:class:`Stream` object.  Then in your application setup 
code you set the correct authorization information as shown in the code example
below. Next thing you need to do, is adding some code to authorize your 
application once your application is authorized we will receive a oauth key and
oauth secret. These values are used to sign your requests so Twitter knows you're
they one who allowed the application to i.e. posts tweets on your behalf. 

When you run your application for the first time we will open a browser
window which opens an authorization page. There you see a code which  you need
enter into the console of XCode.


*In your .h file:*

.. code-block:: c++

	rt::Twitter twitter;

*In your .cpp file.*	
	
.. code-block:: c++

	void testApp::testApp() {
		twitter.setConsumerKey("kyw8bCAWKbk6e1HMMdAvw");
		twitter.setConsumerSecret("PwVuyjLedVZbi4ER6yRAo0byF55AIureauV6UhLRw");
		
		string tokens_file = ofToDataPath("twitter.txt",true);
		if(!twitter.loadTokens(tokens_file)) {
			string auth_url;
			twitter.requestToken(auth_url);
			twitter.handlePin(auth_url);
			twitter.accessToken();
			twitter.saveTokens(tokens_file);
		}	
	}
	

Connecting to the twitter stream
--------------------------------------------------------------------------------

Twitter has a streaming server which allows you to receive status update from 
people you're interested in. There are two kinds of streams: 

#. General streaming server
#. User streaming server

The user streaming server is probably the most interesting to use. It allows
you to receive tweets in realtime from people you're following. **This is an
important thing:** you need to be aware of: when you authorize the application,
you need to login to twitter, the user stream you'll be receiving is from this
user.

To receive and do something with new twitter messages, you need te implement
the rt:IEventListener interface.

.. code-block:: c++

	class IEventListener {
	public:
		virtual void onStatusUpdate(const rtt::Tweet& tweet) = 0;
		virtual void onStatusDestroy(const rtt::StatusDestroy& destroy) = 0;
		virtual void onStreamEvent(const rtt::StreamEvent& event) = 0;
	};
	
Twitter streaming server example
--------------------------------------------------------------------------------

*testApp.h*

.. code-block:: c++

	#pragma once
	
	#include "ofMain.h"
	#include "Twitter.h"
	
	// example listener:
	class TwitterListener : public rt::IEventListener {
	public:
	
		virtual void onStatusUpdate(const rtt::Tweet& tweet) {
			printf("> %s\n", tweet.getText().c_str());
		}
		
		virtual void onStatusDestroy(const rtt::StatusDestroy& destroy) {
		}
		
		virtual void onStreamEvent(const rtt::StreamEvent& event) {
	
		}
	};
	
	
	class testApp : public ofBaseApp{
	
		public:
			testApp();
			void setup();
			void update();
			void draw();
	
			void keyPressed  (int key);
			void keyReleased(int key);
			void mouseMoved(int x, int y );
			void mouseDragged(int x, int y, int button);
			void mousePressed(int x, int y, int button);
			void mouseReleased(int x, int y, int button);
			void windowResized(int w, int h);
			void dragEvent(ofDragInfo dragInfo);
			void gotMessage(ofMessage msg);
			
			rt::Twitter twitter;
			rt::Stream stream;		
			TwitterListener listener;
	};

*testApp.cpp*

.. code-block:: c++

	#include "testApp.h"
	
	testApp::testApp()
		:stream(twitter)
	{
	}
	
	void testApp::setup(){
		ofSetFrameRate(60);
		ofSetVerticalSync(true);
		ofBackground(33);
		
		twitter.setConsumerKey("kyw8bCAWKbP6e1MMdAvw");
		twitter.setConsumerSecret("PwVuyjLeUVZi4ER6yRAo0byF55AIureauV6UhLRw");
		
		string tokens_file = ofToDataPath("twitter.txt",true);
		if(!twitter.loadTokens(tokens_file)) {
			string auth_url;
			twitter.requestToken(auth_url);
			twitter.handlePin(auth_url);
			twitter.accessToken();
			twitter.saveTokens(tokens_file);
		}
		
		stream.track("love");
		stream.connect(URL_STREAM_USER);
		
		twitter.addEventListener(listener);
	}
	
	void testApp::update(){
		stream.update();
	}


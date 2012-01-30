Twitter
==================================

Authorizing a connection
------------------------

In your application include the "Twitter.h" file and create the neccesary
objects. I you want to do status updates, get the timeline, etc.. you need
a Twitter object. When you want to connect to the streaming server, you 
use the Stream object.

.. code-block:: c++

	rt::Twitter twitter;
	
Before you continue you need to register your application at Twitter then 
use the correct twitter username, password, consumer key and secret to make a 
connection to the Twitter servers. See the 
`Twitter developers <http://dev.twitter.com/>`_ page and login to create a new
application..

Then in your applications setup code you set the correct authorization information
as shown below:

.. code-block:: c++

	twitter.setTwitterUsername("********");
	twitter.setTwitterPassword("**********");
	twitter.setConsumerKey("***********");
	twitter.setConsumerSecret("***********");
	
Once all authorization information is set, you're ready to go to retrieve the 
authorization tokens (see oauth for more information. We store the authorization
tokens. We store the authorization tokens in a simple text file because you only
need to get these once. 

.. code-block:: c++

	// get authorized tokens.
	string token_file = "twitter.txt";
	if(!twitter.loadTokens(token_file)) {
		string auth_url;
		twitter.requestToken(auth_url);
		twitter.handlePin(auth_url);
		twitter.accessToken();
		twitter.saveTokens(token_file);
	}
	
	

Mac OpenGL Application
----------------------
This document is work in progress. It describes how to create a 
opengl application for Mac OSX. We use a Cocoa window and avoid
using GLUT or other 3rd party, cross platform lib. Note, that this
is Mac OSX Specific with all pros and cons.

Log:
----
	- add opengl framework
	- add quartz core framework (for displaylink)
	- create a class, extends from NSOpenGLView
	- add a "Custom View" object (not a opengl view) <-- UPDATE: you can use a NSOpenGLView, 
	but make sure to enable double buffering and setting up the other buffers.
	- set your custom class to the class for the custom view (MacOpenGLView)
	
Structure
----------
	- There is one cross OS class: "Application". It's functions get
	called from a OS-specific wrapper.
	- For mac we have:
		- MacApplication: this is the custom window class
		- MacOpenGLView: this is the custom NSOpenGLView class 

Todo:
-----
	- Implement a stable time update:
		- http://www.idevgames.com/forums/thread-677.html?highlight=displaylink+game+loop
		- http://sacredsoftware.net/tutorials/Animation/TimeBasedAnimation.xhtml
		- http://gafferongames.com/game-physics/fix-your-timestep/

References:
-----------

	Synchronize with screen refresh rate:
	------------------------------------
	When programming animations, make sure to use double 
	buffering and synchorize with the refresh rate. 
	
	Info:
	- http://developer.apple.com/library/mac/#documentation/graphicsimaging/conceptual/OpenGL-MacProgGuide/opengl_designstrategies/opengl_designstrategies.html#//apple_ref/doc/uid/TP40001987-CH2-SW4
	- Someone else trying using a displaylink and info on fixing your timestep:
		* http://www.idevgames.com/forums/thread-677.html
		* http://sacredsoftware.net/tutorials/Animation/TimeBasedAnimation.xhtml (time step)
		* http://gafferongames.com/game-physics/fix-your-timestep/ (time step)
	

SETUP ROXLU FOR OPENFRAMEWORKS
==============================

A) **Add the configuration settings to your target:**
------------------------------------------------------
1) Drag and drop this file into your xcode project
2) Select your project in the Project Navigator
3) Select a the project in the right pane (with the blue icon, e.g. "emptyExample")
4) Select the "Info" panel
5) For each of the configurations, select the a target (with the red icon)
6) Select "roxlu" from the drop down.


B) **Add the ROXLU_LIBS and ROXLU_INCLUDES** 
--------------------------------------------
1) Go to the Build Settings of your project
2) search for "Other Linker Flags" and add a row with: $(ROXLU_LIBS)
3) Search for "Header Search Paths" and add a row with: $(ROXLU_INCLUDES)


C) **SET THE PATH TO THE ROXLU LIB**
------------------------------------
1) Select your project in the "Project Navigator"
2) Select your target in the right pane and select "Build Settings"
3) Scroll down to "User-defined" 
4) Drag & Drop the 'root' path to the roxlu lib onto ROXLU_BASE_DIR


D) **TELL ROXLU LIB WHAT FRAMEWORK YOU'RE USING**
--------------------------------------------------
1) Select your project in the "Project Navigator"
2) Select "Build Settings"
3) Select your target (e.g "emptyExample")
4) Add this define: ROXLU_GL_WRAPPER=ROXLU_OPENFRAMEWORKS

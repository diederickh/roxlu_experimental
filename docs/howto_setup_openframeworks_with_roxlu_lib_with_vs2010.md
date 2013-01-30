SETUP ROXLU FOR OPENFRAMEWORKS WITH VISUAL STUDIO
==================================================

A) **Add the roxlu project to your solution**

1. Right click your solution: "Add > Existing project" and browse to
   `roxlu\build\vs2010\roxlu\`, select `roxlu.vcxproj`
2. Open the `Property Manager` (make sure you have expert settings toggled on, 
   `Tools > Settings > Expert Settings`)
3. - Select the `roxlu` project in the `Property Manager`.
   - Right click and select `Properties`. 
   - Select `Common properties > User Macros`
   - Set the full path to the root of the roxlu lib for `ROXLU_DIR`
   - Set the full path to OF for `ROXLU_OF_DIR`



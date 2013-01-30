SETUP WITH VISUAL STUDIO
========================

A) **Add the roxlu project to your solution and compile**

1. Right click your solution: `Add > Existing project` and browse to
   `roxlu\build\vs2010\roxlu\`, select `roxlu.vcxproj`
2. Open the `Property Manager` (make sure you have expert settings toggled on, 
   `Tools > Settings > Expert Settings`)
3.  _Setup the paths used by roxlu lib:_
    - Select the `roxlu` project in the `Property Manager`.
    - Right click and select `Properties`. 
    - Select `Common properties > User Macros`
    - Set the full path to the root of the roxlu lib for `ROXLU_DIR`
    - Set the full path to OF for `ROXLU_OF_DIR`
4. _Compile the roxlu lib:_
   - Go back to your `Solution Explorer` (`View > Solution Explorer`)
   - Right click the `roxlu` project, then select `Build`


B) **Add the include paths and libraries for roxlu lib**

1. Go to the `Property Manager`, `View > Property Manager` (at the bottom)
2. _Setup your `Debug | Win 32` project:_
   - Select your project and expand it. Select `Debug | Win 32`
   - Right click `Add Existing Property Sheet...`, select: `roxlu\lib\roxlu_openframeworks_debug.props` 
3. _Setup your `Release | Win 32` project:_
   - Select your project and expand it. Select `Release | Win 32`
   - Right click `Add Existing Property Sheet...`, select: `roxlu\lib\roxlu_openframeworks_release.props` 
   
   

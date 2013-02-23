Buttons
=======
Buttons is a simple GUI addon to tweak your parameters!
- with client <> server feature to tweak from another PC 
- tweak floats, ints, colors, button handlers, radios, vectors, 2D pad

Using buttons with openFrameworks
=================================
- First you need to add the roxlu-lib project to your project as described here: https://github.com/roxlu/roxlu/blob/master/README.md
- `File > Add Files to "emptyExample" ...` and add the project file `roxlu/addons/Buttons/build/xcode/buttons.xcodeproj`.
- `View > Navigators > Project Navigator` and client on the blue `emptyExample` project. Click on the `emptyExample` target.
   - Select `Build Phases > Link Binary with Libraries`. Press the `+` button and select `libbuttons.a`
   - Select `Target Dependencies`. Press the `+` button and select the `buttons` target.
   - Select the `Build Settings` tab, search for `Header Search Paths` and double click to add a new entry. Drag & drop the directory:
      - `/roxlu/addons/Buttons/include/`
  

<p align="center">
<img src="http://upload.roxlu.com/server/php/files/Screen%20shot%202012-07-18%20at%208.38.09%20PM.png" alt="buttons"><br>
<img src="http://upload.roxlu.com/server/php/files/Screen%20shot%202012-07-18%20at%202.23.24%20PM.png" alt="buttons"><br>
<img src="http://upload.roxlu.com/server/php/files/Screen%20shot%202012-10-04%20at%202.39.23%20PM.png" alt='buttons server">
</p>

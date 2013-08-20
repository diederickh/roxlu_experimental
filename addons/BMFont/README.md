Bitmap Font
============

These files implement a simple and highly customizable solution for Bitmap fonts, 
which are generated with the [Bitmap font generator](http://www.angelcode.com/products/bmfont/).
We had to templatize this library to allow users to use custom vertex types. By default
we use the `BMFVertex` which has a couple of standard attributes like `color`, `position`
and `texcoord`. 


BMFont Settings
---------------
Getting to know BMFont to get high quality bitmap fonts which appeal to you might take
some time. These are some hints which suits my need (I'll add some more settings in the
future).

In general:
- Enabling [x] ClearType, results in smooth, thin readable characters
- Supersampling, makes the font smooth but also kindof smudgy/blurry.

_Nice sharp text_

![](http://farm8.staticflickr.com/7321/9476045298_4b7db071ba_o.png)


````
Size px: 14               [x] Match char height
Height: 100%
[ ] Output invalid char glyph
[ ] Do not include kerning pairs

[ ] Render from TrueType outline
[.] TrueType Hinting (disabled)
[x] Font smoothing        [x] Clear type
[ ] SuperSampling         level [..] (disabled)
````

BMFShader
---------

The BMFShader class is used to setup a VAO and shader for the 
used vertex type. Note that all the BMF* types are temlated on 
the vertex type because this allows you to use the BMFont type 
for other kind of vertices if you would need that.

By default, when you don't explicitly pass the shader to `BMFFont::setup()`
we will allocate this BMFShader() for you and handle all memory management.
The default `BMFShader` can be used for both GL 2.x and 3.x


BMFLoader
---------

The BMFLoader is a pure BMFont parser. It loads the XML file and generates
vertices for a given string. This class doesn't do any rendering. For 
rendering see BMFRenderer

The BMF* types are templated on the `vertex` type. See BMFTypes.h for an example
how you vertex should look like, if  you want to use your own vertex types.



Limitations
-----------

- We only support BMFonts with one page image.

- You must export the .fnt file from `BMFont` in XML format


Code snippets
-------------

- Make sure you enable blending:

<pre>
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  font.draw();
</pre>


Using BMFont with XCode
------------------------
- Include paths:
  - `addons/BMFont/include/`

- Source files
  - `addons/BMFont/include/bmfont/*.h`
  - `addons/BMFont/src/bmfont/*.cpp`


TODO
-----
- The projection matrix is uploaded every draw; we could simple set it once and then reuse it.


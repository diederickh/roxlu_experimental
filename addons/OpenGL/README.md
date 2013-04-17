OpenGL
------

Addon which makes it easy work with openGL in the roxlu library. It will 
support features as, loading 3D objects, loading images (using the image addon),
creating vertex buffers, shaders, vao, pixel buffers etc...
These classes are meant to be used for sketch like applications. I recommend that
you use pure GL and custom vertex buffers etc.. when working on production installations.

_Super quick intro_

````c++

void setup() {
     // always call glr_init() after you've created an opengl context.
     glr_init();

     // load an image
     Texture tex;
     tex.load("someimage.png", true);
}

void draw() {
     tex.draw(10,10,320,240);

     glr_nofill();
     glr_draw_circle(10,10,100);

     glr_fill();
     glr_draw_cirlce(10,10,80);
     
}
````


Standards
----------

Because we make it easy for you to draw things with openGL, we're wrapping some of the 
functionality and therefore we must define a fixed set of vertex types that we support.
All the classes of this addon are build around these vertex types.  You can use custom,
vertex types as long as you follow the standards that the addons needs:

In the shader we assume these standards for the `VertexTypes`:

 - The vertex position is stored in an attribute called `vec3 a_pos` and is bound to the location `0`.
 - The texture coord is stored in an attribute called `vec2 a_tex` and is bound to the location `1`.
 - The color is stored in an attribute called `vec4 a_col` and is bound to the location `2`.
 - The normal is stored in an attribute called `vec4 a_norm` and is bound to the location `3`.

Besides the `VertexTypes` we assume the shader has these uniforms (not all have to be used)

 - The projection matrix: `uniform Mat4 u_pm`  
 - The view matrix: `uniform Mat4 u_vm`  
 - The model matrix: `uniform Mat4 u_mm`  
 - The texture unit 0 (`GL_TEXTURE0`): `uniform sampler2D u_tex0`  
 - The texture unit 1 (`GL_TEXTURE1`): `uniform sampler2D u_tex1`  
 - The texture unit 2 (`GL_TEXTURE2`): `uniform sampler2D u_tex2`  
 - The texture unit 3 (`GL_TEXTURE3`): `uniform sampler2D u_tex3`  


Types
-----

### Shader ###

Used to create a shader program based on the types you pass into it. It takes care of setting the
vertex attribute locations. 


### Vertices ###

CPU based buffer, which mimics the API of a std::vector and adds a couple of simple functions to 
make it nicely work together with the GPU reprentation, namely the `VBO`. See this as a plain 
array for vertices.


### VAO ###

Wrapper around a `Vertex Array`. Based on the given type, it will enable the vertex attributes
that apply to the type. We assume the attribute locations and sizes are described above in _Standards_.
The standard VAO implementation can set the correct vertex attributes for all of the vertex types as 
found in `Vertex.h`. If you want to use a custom vertex types, see the section `Customizing the vertex attributes`
below.


### VBO ###

GPU based buffer, coupled with the `Vertices` type. It wraps around the `Vertices` CPU buffer and is
templates. So e.g. when you want to create a `VBO` for `VertexP` vertices, you would do: `VBO<VertexP> my_vbo`.
For convenience we've wrapped some of the functions you often use, like: `push_back(T)`, `size()`.


### Mesh ###

A `Mesh` contains a `VBO` and `VAO` and makes sure they're both setup correctly and that the vertices
get uploaded to the gpu. The mesh is the base type for most drawing operations used in the `Drawing` class.
The `Mesh` class templates by the vertex type to.


### Texture ###

A basic wrapper around openGL textures. It has a simple feature to load a texture from file or update 
the texture data from a pixel array. For now only `PNG` and `TGA` images are supported and you **need
to add the Image addon**.

### Drawer ###

The `Drawer` class is the **master** of all other types. It creates a couple of default `Shader` 
instances for the different vertex types (`VertexP`, `VertexPT`, `VertexPTN` etc..). If you want 
to draw meshes or textures you need an instance of this class. 

### Font ###

All in one class to draw basic bitmap texts, from the amazing [nothings](http://www.nothings.org). 


Simple wrapper functions
------------------------

Although it's not the fastest solution, sometimes it's easy to draw in immediate mode, therefore we 
added a couple of functions which let you use immediate mode that mimics old school openGL. These functions
are:

- `glr_begin([GL_POINTS, GL_TRIANGLES, ....])`  aka `glBegin()`
- `glr_color([Vec3(), Vec4()])` aka `glColor#f()`
- `glr_vertex(Vec3())`, aka `glVertex#f#()`
- `glr_end()` aka `glEnd()`

Drawing shapes:

- `glr_draw_circle(x, y, radius)`
- `glr_draw_rectangle(x, y, w, h)`
- `glr_fill()`
- `glr_nofill()`


Customizing the vertex attributes
----------------------------------

Although we're stuck to the vertex attribute positions you can still make sure of 
all the features the openGL addon provides such as VBO and VAO handling. Because 
most of the types are templated you can easily extend it with custom vertex types.

The only thing you need to do when you want to make use of the `VAO`/`VBO`/`Mesh`/etc.. 
classes is to create a function that will setup the VAO attributes, and bind the 
attributes in your custom shader to the standard positions.

_Bind attributes with custom shader_

````c++

// Shader setup: binds the attribute location
// See the `a_my_own_attrib` which is used for somethin custom
shader.create(STROKE_VS, STROKE_FS);
shader.bindAttribLocation("a_pos", 0);
shader.bindAttribLocation("a_col", 3);
shader.bindAttribLocation("a_tex", 1);
shader.bindAttribLocation("a_my_own_attrib", 10);
 shader.link();

````

The `VAO<T>` object, which indirectly used by the `Mesh<T>` type, will call a function, 
called: `glr_vao_enable_attributes(VBO<T>& vbo)`. When called you can assume that the 
VBO is already bound, so you only need to setup the attributes.



_Setup VAO_

````c++
  inline void glr_vao_enable_attributes(VBO<StrokeVertex>& vbo) {
    glEnableVertexAttribArray(0); // pos
    glEnableVertexAttribArray(1); // tex
    glEnableVertexAttribArray(3); // color
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(StrokeVertex), (GLvoid*)0); // pos
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(StrokeVertex), (GLvoid*)8); // tex
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(StrokeVertex), (GLvoid*)16); // col
  }

````


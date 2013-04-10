OpenGL
------

Addon which makes it easy work with openGL in the roxlu library. It will 
support features as, loading 3D objects, loading images (using the image addon),
creating vertex buffers, shaders, vao, pixel buffers etc...
These classes are meant to be used for sketch like applications. I recommend that
you use pure GL and custom vertex buffers etc.. when working on production installations.


Standards
----------

Because we make it easy for you to draw things with openGL, we're wrapping some of the 
functionality and therefore we must define a fixed set of vertex types that we support.
All the classes of this addon are build around these vertex types.  You can use custom,
vertex types as long as you follow the standards that the addons needs:

In the shader we assume these standards for the `VertexTypes`:

 - The vertex position is stored in an attribute called `vec3 a_pos` and is bound to the location `0`
 - The texture coord is stored in an attribute called `vec2 a_tex` and is bound to the location `1`.

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
The VAO is not coupled with any other object, but you **need to** make sure that a **VBO** is currently
bound.


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



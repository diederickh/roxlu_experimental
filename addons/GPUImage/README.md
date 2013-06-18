
# GPU IMAGE

This is a small library for handling different kind of pixel formats 
with openGL. We implemented a couple of common pixels formats like,
YUV420P, YUY422, UYVY422 using the most optimized opengl formats/data types.

The `GPUImage` class hides the necessary internals of the specific pixel
format implementations. Because the color space transforms are done on 
the GPU, each implementation (YUV420P, YUYV422, etc..) has it's own shader.
This has a small limitation, that you are bound to use the vertex type that
we handle in the shader, which is: gl::VertexPT. This still means you can 
create any kind of shape you like; and we might add other vertex attributes at
some point (like normals).


## How to use

First of all, the `GPUImage` helps you with a couple of things:

 - it creates a shader that will do the color transform.
 - it gives all the necessary information to create a texture using the 
   best formats for the OS and GL version. 
 - it has helper functions which can create textures for you.
 - each format implementation has support for a projection matrix and 
   model matrix to position the texture.



_Pseudo code:_

````c++
  GLuint tex;
  GPUImage img;
  GPUDrawer drawer;

  void setup() {

    if(!img.setup(AV_PIX_FMT_UYVY422)) {
      ::exit(EXIT_FAILURE);                    
    }
    
    // we UYVY422 uses just one texture; YUV420P uses 3
    img.genTextures(&tex, 1280, 720);

    // setup the drawer (creates vbo/vao)
    if(!drawer.setup()) {
      ::exit(EXIT_FAILURE);
    }

    drawer.setPosition(640, 360);
    drawer.setSize(1280, 720);
  }

  void draw() {
     drawer.draw(img, &tex);
  }

````

## Handle the drawing yourself 

When you want to use a custom draw path; e.g. when you want to define 
your own `gl::VertexPT` positions/texcoords. Or you want to use PBOs, 
you can still do that but you need to be aware of what functions you need 
to call and in what order. You can always look at `GPUDrawer::draw()` which 
does all this:

 - Make sure to set the projectionMatrix on the gpu image so it can calculate the position in the shader use `setProjectionMatrix()`
 - Before calling `setProjectionMatrix()`, make sure to enable the shader that supports the color conversion by calling, `useProgram()`.
 - Use `setModelMatrix()` to position the vertices that you want to use to draw. You can pass an idenity matrix.
 - Call `bindTextures(ids)` to bind the separate textures there are generated with `genTextures()`. Make sure the that array you pass into this function has the correct number of textures. Use `getNumTextures()` to retrieve the number of textures that the colorspace conversion shaders wants. 

_Pseudo code, using your own draw path_

````c++

GPUImage img;
GLuint tex_id;        // the tex_id, see GPUImage.genTextures()
GLuint vao;           // vao
GLuint vbo;           // your vbo that contains gl::VertexPT

void draw() {
  img.useProgram();
  img.setProjectionMatrix(your_ortho_matrix);

  Mat4 model_matrix;
  model_matrix.setPosition(10,10,0);                           // draw at x = 10, y = 10
  model_matrix.setScale(640,480,1);                            // width = 640, height = 480
  img.setModelMatrix(your_model_matrix_to_scale_and_position);

  img.bindTextures(&tex_id);  // let the GPUImage bind the textures are the correct units.

  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, 0, 32); 
  
}

````
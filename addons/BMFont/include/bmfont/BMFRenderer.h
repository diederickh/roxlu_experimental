
/*

  BMFRenderer
  -----------
  This class takes care of rendering fonts for a BMFLoader. We create a 
  texture and VBO. This type works together with a BMFShader type. You can
  pass your own BMFShader type to setup() if you want to use your own shader
  e.g. if you have custom properties.
  
 */

#if BMFONT_WITH_UV
extern "C" {
#include <uv.h>
}
#endif

#ifndef ROXLU_BMFONT_RENDERER_H
#define ROXLU_BMFONT_RENDERER_H

#include <vector>
#include <iterator>
#include <sstream>
#include <roxlu/Roxlu.h>
#include <image/Image.h>
#include <bmfont/BMFTypes.h>
#include <bmfont/BMFLoader.h>
#include <bmfont/BMFShader.h>
#include <bmfont/BMFRenderer.h>

#define BMF_ERR_NOT_SETUP "You didn't call setup so we cant render the BMFont"
#define BMF_ERR_NOT_ALLOC "We havent allocated any data for the GPU yet, did you call update() and added vertices?"
#define BMF_ERR_NO_IMAGE "The given BMFLoader object couldnt find the font image. Dit you call BMFLoader::load() before calling BMFRenderer::setup()"
#define BMF_ERR_NO_IMAGE_FOUND "Cannot find the image file: %s"
#define BMF_ERR_WRONG_NCOMPONENTS "Wrong number of image channels in image file."
#define BMF_ERR_IMAGE_SIZE "The loaded image width/height is not the same as defined in the font"
#define BMF_ERR_BIND_NOT_ALLOCATED "We cannot bind because no bytes were allocated yet. Did you add any text?"
#define BMF_ERR_BIND_SIZE "We cannot bind because the text doesn't have any vertices"
#define BMF_ERR_ALLOC_ALREADY_VERTICES "You cannot allocate manually when you've added some text/vertices already. Makes sure to allocate before adding text"
#define BMF_ERR_ALLOC_ALREADY_ALLOCATED "You cannot allocate manually when you've already allocated some bytes"

template<class T> class 
BMFLoader;

template<class T>
class BMFRenderer {
 public:
  BMFRenderer(BMFLoader<T>& font);
  ~BMFRenderer();
  void setup(int windowW,                                                 /* setup the renderer; we need to the windowW/H for the ortho graphic projection matrix */
             int windowH,  
             size_t pageSize = 1024,                                      /* we will allocate this number of bytes each time the buffer needs to grow (or a multiple of this) */
             BMFShader* shader = NULL);      
  size_t allocate(int numPages);                                            /* allocate `numPages * page_size` number of bytes already; we will the data with 0, make sure to only call this after setup and not when you have already uploaded some vertices! We return the number of bytes allocated or 0 on error */
  void update();                                                          /* updates the VBO if needed */
  size_t addVertices(std::vector<T>& vertices);                           /* add vertices to the VBO and returns the index into multi_counts and multi_firsts. See glMultiDrawArrays for info on these members */
  void removeVertices(size_t index);                                      /* remove vertices for the index which was returned by `addVertices()` */
  void replaceVertices(size_t index, std::vector<T>& vertices);           /* replace vertices for the given index, you can only use this when the number of vertices in the given vector is the same as the one you want to replace */
  void draw();                                                            /* render all strings */
  void debugDraw();
  void drawText(size_t index);                                            /* draw only a specific entry. pass a value you got from addVertices().. also make sure you call bind() before drawing single instances of vertices */
  void drawText(size_t index, const float* modelMatrix);                  /* draw only a specific entry + custom model matrix. pass a value you got from addVertices().. also make sure you call bind() before drawing single instances of vertices */
  void setModelMatrix(const float* mm);
  void setAlpha(float a);
  void reset();                                                           /* reset the VBO, call this when you are updating the text repeatedly */
  size_t size();                                                          /* number of vertices */
  bool bind();                                                            /* bind the specific GL objects we use to render the text. only call this when you are using drawText(). Call bind() once per frame. */
  void flagChanged();                                                     /* when called we make sure that the generated vertices will be updated the next time you call update(); when vertices change this will be set for you. */
  void onResize(int w, int h);                                            /* whenever the viewport changes make sure to call this function so we can recalculate the orth matrix */
 protected:
  void clear();                                                           /* deallocates everything and resets the complete state; */

 protected:
  bool is_changed;                                                        /* set in `flagChanged()` and is used to keep track when we need to update the vbo */
  bool is_setup;
  bool must_replace; /* is set to true when we must replace vertices, see member `to_be_replaced` */
  bool allocated_shader;
  BMFLoader<T>& font;
  BMFShader* shader;                                                      /* the `BMFShader` object which setups up a VAO/Shader which is specific for the used vertex */

  GLuint tex;                                                             /* the font texture */
  GLuint vbo;                                                             /* the BMFRenderer takes care of all buffer handling */
  
 public:
  float projection_matrix[16];
  float model_matrix[16];
  std::vector<GLint> multi_firsts;
  std::vector<GLsizei> multi_counts;
  std::vector<size_t> to_be_replaced;
  std::vector<T> vertices;
  size_t bytes_allocated;
  size_t prev_num_vertices;                                               /* we keep track of the number of vertices in the update() function so we only need to update the sub-buffer data, with new vertices */
  size_t page_size;                                                       /* number of bytes to allocate when the buffer needs to grow */
};

// -----------------------------------------------

template<class T>
BMFRenderer<T>::BMFRenderer(BMFLoader<T>& font)
  :font(font)
  ,is_setup(false)
  ,shader(NULL)
  ,allocated_shader(false)
  ,prev_num_vertices(0) 
  ,page_size(1024) 
  ,must_replace(false)
  ,bytes_allocated(0)
  ,vbo(0)
  ,tex(0)
{
  clear();

  glGenTextures(1, &tex);
#if defined(ROXLU_GL_CORE3)
  glBindTexture(GL_TEXTURE_RECTANGLE, tex);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#else
  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex);
  glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#endif
  
  float* mm = model_matrix;
  memset(mm, 0x00, sizeof(float) * 16);
  mm[0] = mm[5] = mm[10] = mm[15] = 1.0f;
}


template<class T>
BMFRenderer<T>::~BMFRenderer() {
  if(vbo) {
    glDeleteBuffers(1, &vbo);
  }
  
  if(tex) {
    glDeleteTextures(1, &tex);
  }

  if(allocated_shader) {
    delete shader;
    shader = NULL;
    allocated_shader = false;
  }
    
  clear();
}

template<class T>
void BMFRenderer<T>::clear() {
  vbo = 0;
  tex = 0;
  bytes_allocated = 0;
  is_setup = false;

  memset(projection_matrix, 0x00, sizeof(projection_matrix));
 
  multi_firsts.clear();
  multi_counts.clear();
  vertices.clear();
}

template<class T>
void BMFRenderer<T>::reset() {
  vertices.clear();
  multi_firsts.clear();
  multi_counts.clear();
  to_be_replaced.clear();
  prev_num_vertices = 0;
}

template<class T>
void BMFRenderer<T>::setup(int windowW, int windowH, size_t pageSize, BMFShader* useShader) {

  page_size = pageSize;
  rx_ortho(0, windowW, windowH, 0.0, -500.0, 500.0, projection_matrix);

  if(!font.getImageWidth() || !font.getImageHeight() || !font.getImagePath().size()) {
    RX_ERROR(BMF_ERR_NO_IMAGE);
    ::exit(EXIT_FAILURE);
  }

  Image img;
  if(!img.load(font.getImagePath())) {
    RX_ERROR(BMF_ERR_NO_IMAGE_FOUND, font.getImagePath().c_str());
    ::exit(EXIT_FAILURE);
  }

  if(img.getPixelFormat() != RX_FMT_GRAY8) {
    RX_ERROR(BMF_ERR_WRONG_NCOMPONENTS);
    ::exit(EXIT_FAILURE);
  }

  if(img.getWidth() != font.getImageWidth() || img.getHeight() != font.getImageHeight()) {
    RX_ERROR(BMF_ERR_IMAGE_SIZE);
    ::exit(EXIT_FAILURE);
  }

  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
#if defined(ROXLU_GL_CORE3)
  glBindTexture(GL_TEXTURE_RECTANGLE, tex);
  glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_R8, img.getWidth(), img.getHeight(), 0, GL_RED, GL_UNSIGNED_BYTE, img.getPixels());
#else
  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tex);
  glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_R8, img.getWidth(), img.getHeight(), 0, GL_RED, GL_UNSIGNED_BYTE, img.getPixels());
#endif
  is_setup = true;

  glGenBuffers(1, &vbo);

  if(!useShader) {
    allocated_shader = true;
    shader = new BMFShader();
  }

  shader->setup(vbo, tex);

}

template<class T>
void BMFRenderer<T>::onResize(int winW, int winH) {
  rx_ortho(0, winW, winH, 0.0, -500.0, 500.0, projection_matrix);
}


template<class T>
size_t BMFRenderer<T>::allocate(int numPages) {

  if(vertices.size()) {
    RX_ERROR(BMF_ERR_ALLOC_ALREADY_VERTICES);
    return 0;
  }

  if(bytes_allocated > 0) {
    RX_ERROR(BMF_ERR_ALLOC_ALREADY_ALLOCATED);
    return 0;
  }

  if(!vbo) {
    RX_ERROR(BMF_ERR_NOT_SETUP);
    return 0;
  }

  bytes_allocated = numPages * page_size;
  RX_VERBOSE("PAGESIZE: %ld, numPages: %ld", page_size, numPages);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, bytes_allocated, NULL, GL_STREAM_DRAW);

  RX_VERBOSE("ALLOCATED: %ld", bytes_allocated);
  return bytes_allocated;
}

template<class T>
size_t BMFRenderer<T>::addVertices(std::vector<T>& in) {
  size_t index = multi_firsts.size();
  multi_firsts.push_back(vertices.size());
  multi_counts.push_back(in.size());

  std::copy(in.begin(), in.end(), std::back_inserter(vertices));
  return index;
}

template<class T>
void BMFRenderer<T>::removeVertices(size_t dx) {
  assert(dx < multi_firsts.size());
  size_t start = multi_firsts[dx];
  size_t count = multi_counts[dx];
  //RX_VERBOSE("REMOVE FROM: %ld and %ld vertices", start, count);

  vertices.erase(vertices.begin() + start, vertices.begin() + start + count);
  multi_firsts.erase(multi_firsts.begin() + dx);
  multi_counts.erase(multi_counts.begin() + dx);
}

template<class T>
void BMFRenderer<T>::replaceVertices(size_t dx, std::vector<T>& in) {
  assert(dx < multi_firsts.size());
  to_be_replaced.push_back(dx);

  size_t start = multi_firsts[dx];
  size_t count = multi_counts[dx];
  //  RX_VERBOSE("REPLACE FROM: %ld and %ld vertices", start, count);

  //std::replace(vertices.begin() + start, vertices.begin() + start + count, in.begin(), in.end());
  std::copy(in.begin(), in.end(), vertices.begin() + start) ; // vertices.begin() + start, vertices.begin() + start + count, in.begin(), in.end());
  must_replace = true;
  
}

template<class T>
void BMFRenderer<T>::update() {

  if(!vertices.size()) {
    return;
  }

  if(!is_changed && !must_replace) {
    return;
  }

  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  size_t bytes_needed = vertices.size() * sizeof(T);

#if BMFONT_WITH_UV
  uint64_t start = uv_hrtime();
#endif

  if(bytes_needed > bytes_allocated) {
    while(bytes_allocated < bytes_needed) {
      bytes_allocated = std::max<size_t>(bytes_allocated * 2, page_size);
    }
    glBufferData(GL_ARRAY_BUFFER, bytes_allocated, NULL, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, bytes_needed, vertices[0].getPtr());
  }
  else if(vertices.size() != prev_num_vertices) {
    size_t new_bytes = (vertices.size() - prev_num_vertices) * sizeof(T);
    size_t prev_offset = prev_num_vertices * sizeof(T);
    glBufferSubData(GL_ARRAY_BUFFER, prev_offset, new_bytes, vertices[prev_num_vertices].getPtr());
    //RX_ERROR("UPLOADED!, prev_num_vertices: %ld, %ld, %ld", prev_num_vertices, prev_offset, new_bytes);
  }

  if(must_replace) {
    RX_ERROR("MUST REPLACE!: %ld", to_be_replaced.size());
    for(std::vector<size_t>::iterator it = to_be_replaced.begin(); it != to_be_replaced.end(); ++it) {
      size_t dx = *it;
      size_t start = multi_firsts[dx];
      size_t count = multi_counts[dx];
      glBufferSubData(GL_ARRAY_BUFFER, start * sizeof(T), sizeof(T) * count, vertices[start].getPtr());
    }
    to_be_replaced.clear();
    
  }
  //glBufferSubData(GL_ARRAY_BUFFER, 0, bytes_needed, vertices[0].getPtr());
#if BMFONT_WITH_UV
  uint64_t d = uv_hrtime() - start;
  uint64_t md = d / 1000000;
  std::stringstream ss;
  ss << d << " ns " << md << " millis";
  RX_WARNING("UPDATING FONT VERTICES TOOK: %s ", ss.str().c_str());
#endif

  prev_num_vertices = vertices.size(); 
  //RX_VERBOSE("NUM VERTICES: %ld", vertices.size());
  is_changed = false;
  must_replace = false;
}

template<class T>
inline bool BMFRenderer<T>::bind() {

  if(!is_setup) {
    RX_ERROR(BMF_ERR_NOT_SETUP);
    return false;
  }

  if(!bytes_allocated) { 
    RX_ERROR(BMF_ERR_BIND_NOT_ALLOCATED);
    return false;
  }

  if(!vertices.size()) {
    RX_ERROR(BMF_ERR_BIND_SIZE);
    return false;
  }

  shader->bind();
  shader->setProjectMatrix(projection_matrix);
  shader->setModelMatrix(model_matrix);

  return true;
}

template<class T>
void BMFRenderer<T>::draw() {

  if(!multi_counts.size()) {
    return;
  }

  bind();

#if defined(ROXLU_GL_CORE3)
  glMultiDrawArrays(GL_TRIANGLES, &multi_firsts[0], &multi_counts[0], multi_counts.size());
#else 
  for(size_t i = 0; i < multi_firsts.size(); ++i) {
    glDrawArrays(GL_TRIANGLES, multi_firsts[i], multi_counts[i]);
  }
#endif
}

template<class T>
void BMFRenderer<T>::debugDraw() {
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  draw();
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


template<class T>
inline void BMFRenderer<T>::drawText(size_t dx) {
  glDrawArrays(GL_TRIANGLES, multi_firsts[dx], multi_counts[dx]);
}

template<class T>
inline void BMFRenderer<T>::drawText(size_t dx, const float* modelMatrix) {
  assert(dx < multi_counts.size());
  assert(multi_counts[dx] > 0);
  assert(dx < multi_firsts.size());
  assert(multi_firsts[dx] >= 0);

  shader->setModelMatrix(modelMatrix);
  glDrawArrays(GL_TRIANGLES, multi_firsts[dx], multi_counts[dx]);
}

template<class T>
inline void BMFRenderer<T>::setModelMatrix(const float* modelMatrix) {
  memcpy(model_matrix, modelMatrix, sizeof(float) * 16);
}

template<class T>
inline void BMFRenderer<T>::setAlpha(float a) {
  shader->setAlpha(a);
}

template<class T>
inline size_t BMFRenderer<T>::size() {
  return vertices.size();
}

template<class T>
inline void BMFRenderer<T>::flagChanged() {
  is_changed = true;
}



#endif

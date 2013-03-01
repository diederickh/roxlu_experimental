#ifdef ROXLU_GL_WRAPPER

#include <roxlu/3d/shapes/Axis.h>

namespace roxlu {

  Axis::Axis() 
  {
  }

  void Axis::setup(int size) {

    // VERTICES
    // ---------
    VerticesPC verts;
    int mod = size % 2;
    size += mod;
    float spacing = (float)size*0.05;
    float half_size = size * 0.5 * spacing;
    float full_size = half_size * 2;
    int middle = size * 0.5;
    Vec3 col_gray(0.5,0.5,0.5);
    Vec3 col_white(1,1,1);
    Vec3 col_x(0.85,0.26,0.29);
    Vec3 col_y(0.56,0.78,0.42);
    Vec3 col_z(0.11,0.64,0.79);
    Vec3* curr_col; 

    VertexPC vpc;
    for(int i = 0; i <= size; ++i) {
      // z-direction
      curr_col = (i == middle) ? &col_z : &col_gray;
      if(i == 0 || i == size) {
        curr_col = &col_white;
      }
      
      vpc.setCol(*curr_col);
      vpc.setPos(Vec3(i * spacing - half_size, 0, half_size));
      verts.add(vpc);

      vpc.setPos(i * spacing - half_size, 0, -half_size);
      verts.add(vpc);
		
      // x-direction
      curr_col = (i == middle) ? &col_x : &col_gray;
      if(i == 0 || i == size) {
        curr_col = &col_white;
      }
      vpc.setCol(*curr_col);
      vpc.setPos(half_size, 0, i * spacing - half_size);
      verts.add(vpc);
      
      vpc.setPos(-half_size, 0, i * spacing - half_size);
      verts.add(vpc);
    }

    // draw y 
    vpc.setCol(col_y);
    vpc.setPos(0.0f, 0.0f, 0.0f);
    verts.add(vpc);

    vpc.setPos(0,full_size, 0);
    verts.add(vpc);

    nvertices = verts.size();

    // SHADER, VAO & VBO
    shader.create(AXIS_VS, AXIS_FS);
    shader.a("a_pos", 0).a("a_col", 1);
    shader.link();
    shader.u("u_projection_matrix").u("u_view_matrix");
    
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    // vao.bind();
    
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verts.numBytes(), verts.getPtr(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // pos
    glEnableVertexAttribArray(1); // col
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPC), (GLvoid*)offsetof(VertexPC, pos));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPC), (GLvoid*)offsetof(VertexPC, col));
  }

  void Axis::draw(const float* pm, const float* vm) {
    glBindVertexArray(vao);
    shader.enable();
    shader.uniformMat4fv("u_projection_matrix", pm);
    shader.uniformMat4fv("u_view_matrix", vm);
    glLineWidth(2.0f);
    glDrawArrays(GL_LINES, 0, nvertices);
  }

} // roxlu

#endif // ROXLU_GL_WRAPPER

#ifndef ROXLU_RTYPESH
#define ROXLU_RTYPESH

#include <roxlu/opengl/GL.h>
#include <roxlu/math/Vec2.h>
#include <iterator>
#include <vector>
#include <cstring> // memcpy

#define BMOUSE_INSIDE(mx,my,ex,ey,w,h) 	(mx>=ex && mx<=(ex+w) && my >= ey && my <= (ey+h))
#define BINSIDE_ELEMENT(ptr, mx, my)  BMOUSE_INSIDE(mx, my, ptr->x, ptr->y, ptr->w, ptr->h)
#define BINSIDE_HEADER(ptr, mx, my)  BMOUSE_INSIDE(mx, my, ptr->x, ptr->y, ptr->w, ptr->header_h)
#define BSET_COLOR(col, r, g, b, a) col[0] = r; col[1] = g; col[2] = b; col[3] = a;
#define BSET_MAT_POS(mat, x, y, z) mat[11] = x; mat[12] = y; mat[13] = z;
#define BLIMIT_FLOAT(v,minv, maxv) std::max<float>((minv), std::min<float>((maxv), (v)))

using std::vector;
using roxlu::Vec2;

namespace buttons {

  class Element;
  class Buttons; 

  enum ElementTypes {
    BTYPE_SLIDER = 0
    ,BTYPE_TOGGLE = 1
    ,BTYPE_BUTTON = 2
    ,BTYPE_RADIO = 3
    ,BTYPE_SPLINE = 4
    ,BTYPE_COLOR = 5
    ,BTYPE_PAD = 6
    ,BTYPE_RECTANGLE = 7
    ,BTYPE_VECTOR = 8
    ,BTYPE_SLIDER_RANGE = 9
  };

  enum ElementStates {
    BSTATE_NONE
    ,BSTATE_ENTER
    ,BSTATE_LEAVE
  };

  enum ElementValueTypes {
    BVALUE_NONE
    ,BVALUE_INT
    ,BVALUE_FLOAT
  };


  enum ButtonsEventType {
    BEVENT_BUTTONS_REDRAW = 0 // fired when color, size, value etc.. changes.
    ,BEVENT_VALUE_CHANGED = 1  // fired when a value of a widged is changed
  };

  // Used by client/server to specify how a remote gui looks like
  // The scheme also uses the ElementTypes to describe the elements 
  // of the gui.
  enum ButtonsScheme {
     BSCHEME_GUI   // element is a gui
    ,BSCHEME_PANEL  // element is a panel 
  };

  class ButtonsListener {
  public:
    virtual void onEvent(ButtonsEventType event, const Buttons& buttons, const Element* target, void* targetData) = 0;
  };

  class ButtonVertex {

  public:

    float pos[2];
    float col[4];
	
    ButtonVertex() {
      pos[0] = pos[1] = 0.0f;
      col[0] = col[1] = col[2] = col[3] = 0.0f;
    }
	
    ButtonVertex(const float x, const float y, const float* col4) {
      set(x,y,col4);
    }
	
	
    void setPos(const float x, const float y) {
      pos[0] = x;
      pos[1] = y;
    }
	
    void setCol(const float* c) {
      setCol(c[0], c[1], c[2], c[3]);
    }
	
    void setCol(const float r, const float g, const float b, const float a) {
      col[0] = r;
      col[1] = g;
      col[2] = b;
      col[3] = a;
    }
	
    void set(const float x, const float y, const float* col4) {
      pos[0] = x;
      pos[1] = y;
      col[0] = col4[0];
      col[1] = col4[1];
      col[2] = col4[2];
      col[3] = col4[3];
    }
  };

  struct ButtonDrawArray {
  ButtonDrawArray(int m, int s, int c):mode(m),start(s),count(c){}
    int mode;
    int start;
    int count;
  };

  class ButtonVertices {
  public:
    ButtonVertices() {
    }
	
    size_t size() {
      return verts.size();
    }
	
    float* getPtr() {
      return &verts[0].pos[0];
    }
	
    void clear() {
      draw_arrays.clear();
      verts.clear();
    }
	
    void add(vector<ButtonVertex>& newVertices, int drawMode) {
      int start = size();
      std::copy(newVertices.begin(), newVertices.end(), std::back_inserter(verts));
      draw_arrays.push_back(ButtonDrawArray(drawMode, start, newVertices.size()));
    }
	
    ButtonVertex& operator[](const int& i) {
      return verts[i];
    }
	
    vector<ButtonVertex> verts;
    vector<ButtonDrawArray> draw_arrays;
  };

  static int createRect(ButtonVertices& vd, int x, int y, int w, int h, float* topCol, float* botCol) {
    vector<ButtonVertex> new_verts(6);
    new_verts[0].set(x,y,topCol);
    new_verts[1].set(x+w,y,topCol);
    new_verts[2].set(x+w,y+h,botCol);
	
    new_verts[3].set(x+w,y+h,botCol);
    new_verts[4].set(x,y+h,botCol);
    new_verts[5].set(x,y,topCol);
    vd.add(new_verts, GL_TRIANGLES);
	
    return 6; // number of vertices
  }

  static int createCircle(ButtonVertices& vd, const float x, const float y, const float r, float* col) {
    static int num = 20;
    static vector<ButtonVertex> res;
    if(!res.size()) {
      float def_col[4] = {1.0f, 1.0f, 1.0f, 1.0f};
      float pip = 6.283185307 / float(num);
      for(int i = 0; i <= num; ++i) {
        float a = pip * i;
        res.push_back(ButtonVertex(cos(a),sin(a), def_col));
      }
    }
	
    vector<ButtonVertex> new_verts(res.size());
    for(int i = 0; i <= num; ++i) {
      new_verts[i].set(x + res[i].pos[0] * r, y + res[i].pos[1] * r, col);
    }
    vd.add(new_verts, GL_TRIANGLE_FAN);
	
    return new_verts.size();
  }

  // Hash function used by client/server
  inline unsigned int buttons_hash(const char *key, size_t len) {
    unsigned int hash, i;
    for(hash = i = 0; i < len; ++i) {
      hash += key[i];
      hash += (hash << 10);
      hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
  }

  // @todo use this everywhere, instead of Buttons::createCleanName
  inline std::string buttons_create_clean_name(const std::string& ugly) {
    std::string clean_name;
    for(int i = 0; i < ugly.size(); ++i) {
      if(isalnum(ugly[i])) {
        clean_name.push_back(tolower(ugly[i]));
      }
      else if(isspace(ugly[i]) || ugly[i] == '.' || ugly[i] == '_' || ugly[i] == '-') {
        clean_name.push_back('_');
      }
    }
    return clean_name;
  }

  // Super simple buffer used by client/server
  struct ButtonsBuffer {
    ButtonsBuffer(){
    }

    void clear() {	
      data.clear();
    }

    size_t size() { 
      return data.size();
    }

    size_t getNumBytes() { 
      return size() * sizeof(char); 
    }

    char* getPtr() { 
      return &data[0]; 
    }

    void addByte(char b) { 
      data.push_back(b);	
    }

    void addI32(int num) {
      addBytes((char*)&num, sizeof(num));
    }
	
    void addUI32(unsigned int num) {	
      addBytes((char*)&num, 4);
    }

    void addUI16(unsigned short num) { 
      addBytes((char*)&num, 2); 
    }

    void addFloat(float num) {
      addBytes((char*)&num, sizeof(num));
    }

    void addBytes(char* buffer, int num) { 
      for(int i = 0; i < num; ++i) { 
        data.push_back(buffer[i]);
      } 
    }

    void addString(std::string str) {
      unsigned short size = str.size();
      addUI16(size);
      addBytes((char*)str.c_str(), size);
    }

    void rewrite(int start, int num, char* buffer) { 
      for(int i = start, j = 0; i < (start+num); ++i, ++j) { 
        data[i] = buffer[j]; 
      } 
    }

    char consumeByte() {
      char c = data[0];
      flush(1);
      return c;
    }

    std::string consumeString() {
      unsigned short size = consumeUI16();
      std::string str;
      for(int i = 0; i < size; ++i) {
        str.push_back((char)data[i]);
      }
      flush(size);
      return str;
    }
	
    unsigned short int consumeUI16() {
      unsigned short int v = getUI16();
      flush(2);
      return v;
    }

    unsigned int consumeUI32() {
      unsigned int v = getUI32();
      flush(sizeof(v));
      return v;
    }

    int consumeI32() {
      int v = getI32();
      flush(sizeof(v));
      return v;
    }
	
    float consumeFloat() {
      float f = getFloat();
      flush(sizeof(float));
      return f;
    }

    unsigned short int getUI16(int dx = 0) {
      unsigned short int v;
      memcpy((char*)&v, (char*)&data[dx], sizeof(v));
      return v;
    } 

    unsigned int getUI32(int dx = 0) { 
      unsigned int v; 
      memcpy((char*)&v, (char*)&data[dx], sizeof(v));
      return v;
    }

    float getFloat() {
      float f = 0.0f;
      memcpy((char*)&f, (char*)&data[0], sizeof(float));
      return f;
    }

    int getI32(int dx = 0) {
      int v = 0;
      memcpy((char*)&v, (char*)&data[dx], sizeof(int));
      return v;
    }

    void flush() {
      flush(size());
    }

    void flush(int numBytes) {
      data.erase(data.begin(), data.begin() + numBytes);
    }

    char& operator[](const unsigned int dx) { return data[dx]; } 

    std::vector<char> data;
  };

} // namespace buttons


#endif

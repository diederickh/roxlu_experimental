#ifndef ROXLU_ROXH
#define ROXLU_ROXH

#include <sstream>
#include <map>
#include <string>
#include <fstream>

#include <roxlu/3d/Vertexdata.h>
//#include "VertexData.h"
using namespace std;

namespace roxlu {


/**

Almost identical format to OBJ for loading vertices, faces, normals
and texture coordinates. I created a simple "ROX" exporter for blender, which
are just a couple of lines you can include in the scripting pane in 
blender. 

We follow the .OBJ standard but use these value differently:

g [name] [indices]:		When you define a vertex group in blender, this will
						contain a list of vertex indices of this group separated
						by a space. 
						
						Examples:
						---------
						g hand.L 67 68 77 78
						g hand.R 62 63 72 73
						
						
Python code:
---------------
import bpy
from pprint import pprint

obj = bpy.context.active_object
me = obj.data
ofs = open("grid.rox", "w")

data = "o %s\n" % obj.name
ofs.write(data)

# export vertices
for v in me.vertices:
    data = "v %.6f %.6f %.6f\n" % v.co[:]
    ofs.write(data)

# export normals
for polygon in obj.data.polygons:
    data = "vn %.6f %.6f %.6f\n" % polygon.normal[:]
    ofs.write(data)


# export texcoords
uvcoords = me.uv_layers.active.data
for uv in uvcoords[:]:
    data = "vt %.6f %.6f\n" % uv.uv[:]
    ofs.write(data)
    

# export faces
for polygon in obj.data.polygons:
   if len(polygon.loop_indices) == 4:
       va = me.loops[polygon.loop_indices[0]].vertex_index + 1
       vb = me.loops[polygon.loop_indices[1]].vertex_index + 1
       vc = me.loops[polygon.loop_indices[2]].vertex_index + 1
       ta = polygon.loop_indices[0] + 1
       tb = polygon.loop_indices[1] + 1
       tc = polygon.loop_indices[2] + 1
       data = "f %d/%d/ %d/%d/ %d/%d/\n" % (va, ta, vb, tb, vc, tc)
       ofs.write(data)
       
       va = me.loops[polygon.loop_indices[2]].vertex_index + 1
       vb = me.loops[polygon.loop_indices[3]].vertex_index + 1
       vc = me.loops[polygon.loop_indices[0]].vertex_index + 1
       ta = polygon.loop_indices[2] + 1
       tb = polygon.loop_indices[3] + 1
       tc = polygon.loop_indices[0] + 1
       data = "f %d/%d/ %d/%d/ %d/%d/\n" % (va, ta, vb, tb, vc, tc)
       ofs.write(data)
   elif len(polygon.loop_indices) == 3:
       va = me.loops[polygon.loop_indices[0]].vertex_index + 1
       vb = me.loops[polygon.loop_indices[1]].vertex_index + 1
       vc = me.loops[polygon.loop_indices[2]].vertex_index + 1
       ta = polygon.loop_indices[0] + 1
       tb = polygon.loop_indices[1] + 1
       tc = polygon.loop_indices[2] + 1
       data = "f %d/%d/ %d/%d/ %d/%d/\n" % (va, ta, vb, tb, vc, tc)
       ofs.write(data)
   else:
       print(len(polygon.loop_indices))
       
# export vertex groups
for vertex_group in obj.vertex_groups:
    vertex_list = []
    print("Vertex group: ", vertex_group.name)
    for vertex in me.vertices:
        for vgroup in vertex.groups:
            if vgroup.group == vertex_group.index:
                vertex_list.append(vertex.index)

    #data = " ".join(v["%s" % el fro el in vertex_list])                    
    data = "g " +vertex_group.name +" " +" ".join(["%s" % el for el in vertex_list]) +"\n"
    print(vertex_list)
    ofs.write(data)
ofs.close()						
*/


class ROX {
public:
	
	struct Object {
		VertexData vd;
		string name;
	};	
	
	bool import(const string& filepath);
	bool extractFace(string info, int& vertexIndex, int& normalIndex, int& texcoordIndex);
	void print();
	bool contains(const string& objectName);
	
	std::map<string, Object> objects;
	VertexData operator[](const string& name);
};

inline VertexData ROX::operator[](const string& name) {
	map<string,Object>::iterator it = objects.find(name);
	if(it == objects.end()) {
		VertexData vd;
		return vd;	
	}
	return it->second.vd;
}

inline bool ROX::contains(const string& objectName) {
	map<string,Object>::iterator it = objects.find(objectName);
	if(it == objects.end()) {
		return false;
	}
	return true;
}

};

#endif
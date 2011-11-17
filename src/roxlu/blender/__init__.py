import bpy
import struct
import mathutils
# Good info: http://wiki.blender.org/index.php/Dev:2.5/Py/Scripts/Cookbook

#imports for file selector
from bpy_extras.io_utils import ImportHelper
from bpy.props import StringProperty, BoolProperty, EnumProperty

R3F_VERTEX_DATAS = 0x01
R3F_VERTICES = 0x02
R3F_FACES = 0x03
R3F_FACE_QUAD = 0x04
R3F_SCENE_ITEMS = 0x05

bl_info = {
    "name": "RoxluLib",
    "description": "Imports 3D scenes exported using roxlu lib.",
    "author": "Diederick Huijbers <diederick@apollomedia.nl>",
    "version": (0,1),
    "blender": (2, 5, 3),
    "api": 31236,
    "location": "File > Import-Export",
    "warning": "", # used for warning icon and text in addons panel
    "wiki_url": "http://www.roxlu.com",
    "tracker_url": "http://www.roxlu.com",
    "category": "Import-Export"}    


def readUI32(file):
	tmp = file.read(4)
	data = struct.unpack("i", tmp)
	return data[0]

def readUI8(file):
	tmp = file.read(1);
	data = struct.unpack("b", tmp)
	return data[0]

def readUI16(file):
	tmp = file.read(2)
	data = struct.unpack("h",tmp)
	return data[0]
	
def readFloat(file):
	tmp = file.read(4)
	data = struct.unpack("f", tmp)
	return data[0]
	
def readFloat3(file):
	tmp = file.read(12)
	(x,y,z) = struct.unpack("fff", tmp)
	return (x,y,z) 
	
def readFloat4(file):
	tmp = file.read(16)
	(x,y,z,w) = struct.unpack("ffff", tmp)
	return (x,y,z,w) 

def readQuad(file):
	tmp = file.read(16)
	q = Quad()
	(q.a, q.b, q.c, q.d) = struct.unpack("iiii", tmp)
	return q

def readString(file):
	length = readUI16(file)
	tmp = file.read(length)
	n = str(tmp, "utf-8", "replace")
	return n

   
class SceneItem:
	def __init__(self):
		self.vertex_data_id = 0
		self.name = "no_name"
		self.origin = []
		self.orientation = 0
		self.scale = []
	
	def setVertexDataID(self, id):
		self.vertex_data_id = id
	
	def getVertexDataID(self):
		return self.vertex_data_id
		
	def setName(self, n):
		self.name = n
		
	def getName(self):
		return self.name
		
	def setOrigin(self, o):
		self.origin = (o[0], o[2], o[1])
		
	def getOrigin(self):
		return self.origin
		
	def setOrientation(self, o):
		self.orientation = mathutils.Quaternion((o[0], o[2], o[1]), -o[3]).to_euler() 

	def getOrientation(self):
		return self.orientation
		
	def setScale(self, s):
		self.scale = (s[0], s[2], s[1])
		
	def getScale(self):
		return self.scale
		
class VertexData:
	def __init__(self):
		self.vertices = []
		self.quads = []
		self.triangles = []
		self.faces = []
			
	def addVertex(self, vert):
		self.vertices.append((vert[0], vert[2], vert[1]))
		
	def addQuad(self, quad):
		self.quads.append(quad)
		
	def addTriangle(self, triangle):
		self.triangles.append(triangle)
	
	# creates faces from quads and triangles
	def getFaces(self):
		for quad in self.quads:
			self.faces.append((quad.a, quad.b, quad.c, quad.d))
		return self.faces;
	
	def getVertices(self):
		return self.vertices;
	
	def setID(self, id):
		self.id = id
		
	def getID(self):
		return self.id 

class Triangle:
	def __init__(self):
		self.a = 0;
		self.b = 0;
		self.c = 0;
		
class Quad:
	def __init__(self):
		self.a = 0
		self.b = 0
		self.c = 0
		self.d = 0
	
	def set(self, d):
		self.a = d[0]
		self.b = d[1]
		self.c = d[2]
		self.d = d[3]

class Scene:
	
	
	def __init__(self):
		self.vertex_datas = {}
		self.scene_items = []
		self.meshes = {}
		
	def addVertexData(self, vd):
		self.vertex_datas[vd.getID()] = vd;
		
	def addSceneItem(self, si):
		self.scene_items.append(si)
	
	def createMeshes(self):
		print("Creating meshes")
		
		# Create meshes (vertex data representations)
		for vertex_data_id in self.vertex_datas:
			print("ID: ", vertex_data_id)
			vd = self.vertex_datas[vertex_data_id]			
			m = bpy.data.meshes.new("mesh_" +str(vertex_data_id))
			self.addMesh(vertex_data_id, m)
			m.from_pydata(vd.getVertices(), [], vd.getFaces())
			m.update()			
		
		# Create instances (scene items)
		for si in self.scene_items:
			m = self.getMeshForSceneItem(si)
			ob = bpy.data.objects.new(si.getName(), m)
			ob.location = si.getOrigin()
			ob.rotation_euler = si.getOrientation()
			ob.scale = si.getScale()
			bpy.context.scene.objects.link(ob)
		
	def getMeshForSceneItem(self, si):
		return self.meshes["mesh" +str(si.getVertexDataID())]
		
	def addMesh(self, vertexDataID, mesh):
		self.meshes["mesh" +str(vertexDataID)] = mesh
		
	def getVertexData(self, vertexDataID):
		return self.vertex_datas[vertexDataID]
	
def parseCommand(cmd, dataFile, Container):
	if cmd == R3F_VERTEX_DATAS: 
		num_vertex_datas = readUI32(dataFile)
		print("Parse vertex datas: ", num_vertex_datas)
		for i in range(0, num_vertex_datas):
			vertex_data = VertexData()
			vertex_data_id = readUI32(dataFile)
			vertex_data.setID(vertex_data_id)
			print("Vertex data id: ",vertex_data_id)
			Container.addVertexData(vertex_data)
			cmd = readUI8(dataFile)
			parseCommand(cmd, dataFile, vertex_data)
		cmd = readUI8(dataFile)
		parseCommand(cmd, dataFile, Container)
		
	elif cmd == R3F_VERTICES:
		num_vertices = readUI32(dataFile)
		print("Parse vertices: ", num_vertices)
		for i in range(0,num_vertices):
			v = readFloat3(dataFile)
			Container.addVertex(v)
		cmd = readUI8(dataFile)
		parseCommand(cmd, dataFile, Container)

	elif cmd == R3F_FACES:
		num_faces = readUI32(dataFile)
		print("Parse faces: ", num_faces)
		for i in range(0, num_faces):
			face_type = readUI8(dataFile)
			if face_type == R3F_FACE_QUAD:
				quad = readQuad(dataFile)
				Container.addQuad(quad)
			else:
				print("Unhandled face type: ", face_type)

	elif cmd == R3F_SCENE_ITEMS:
		num_scene_items = readUI32(dataFile)
		print("Number of scene items: ", num_scene_items)		
		for i in range(0, num_scene_items):
			vertex_data_id = readUI32(dataFile)
			scene_item_name = readString(dataFile)
			origin = readFloat3(dataFile)
			scale = readFloat3(dataFile)
			orientation = readFloat4(dataFile)
			si = SceneItem()
			si.setVertexDataID(vertex_data_id)
			si.setName(scene_item_name)
			si.setOrigin(origin)
			si.setOrientation(orientation)
			si.setScale(scale)
			Container.addSceneItem(si)

			print("--------")			
			print("Scene item:")
			print("vertex data: ", vertex_data_id)
			print("origin: ", origin)
			print("scale: ", scale)
			print("orientation: ", orientation)
			print("name:", scene_item_name)
			print("--------")

	
def importR3F(filepath):
	data_file = open(filepath, "rb");
	scene = Scene()
	cmd = readUI8(data_file)
	parseCommand(cmd, data_file, scene)
	scene.createMeshes()
	return

# the ExportHelper gives us the file popup :)
class RoxluLib(bpy.types.Operator, ImportHelper):
	'''Tooltip'''
	bl_idname = "roxlulib.import"
	bl_label = "Import Roxlu file"
	filename_ext = ".r3f"
	filter_glob = StringProperty(default="*.r3f", options={'HIDDEN'})
	
	def execute(self, context):
		importR3F(self.filepath)
		return {'FINISHED'}
		
def menu_func_import(self, context):
	self.layout.operator("roxlulib.import", text="Roxlu 3D Format (*.r3f)")

def register():
	bpy.utils.register_module(__name__)
	bpy.types.INFO_MT_file_import.append(menu_func_import)
	#bpy.utils.register_class(RoxluLib)
	
	
def unregister():
	bpy.utils.unregister_class(RoxluLib)
		
if __name__ == "__main__":
	register()

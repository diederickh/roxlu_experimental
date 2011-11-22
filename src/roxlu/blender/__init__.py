import bpy
import struct
import mathutils
import os
# Good info: http://wiki.blender.org/index.php/Dev:2.5/Py/Scripts/Cookbook

#imports for file selector
from bpy_extras.io_utils import ImportHelper
from bpy.props import StringProperty, BoolProperty, EnumProperty

R3F_VERTEX_DATAS = 0x01
R3F_SCENE_ITEMS = 0x02
R3F_MATERIALS = 0x03

R3F_MAT_NONE			= 0
R3F_MAT_DIFFUSE		= (1 << 0)
R3F_MAT_SPECULAR		= (1 << 1)
R3F_MAT_AMBIENT		= (1 << 2)
R3F_MAT_HEIGHT		= (1 << 3)
R3F_MAT_NORMAL		= (1 << 4)
R3F_MAT_SHININESS	= (1 << 5)
R3F_MAT_OPACITY		= (1 << 6)
R3F_MAT_DISPLACEMENT	= (1 << 7)
R3F_MAT_LIGHT		= (1 << 8)
R3F_MAT_REFLECTION	= (1 << 9)
R3F_MAT_CUBEMAP		= (1 << 10)

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
	
def readFloat2(file):
	tmp = file.read(8)
	(x,y) = struct.unpack("ff", tmp)
	return (x,y) 

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
	
class Texture: 
	def __init__(self):
		self.type = R3F_MAT_NONE
		self.file = ""
	
	def setup(self, type, file):
		self.type = type
		self.file = file

		
	def getFile(self):
		return self.file
		
	def createBlenderImage(self):
		img = bpy.data.images.load(self.file)
		return img
		
	

class Material:
	def __init__(self):
		self.textures = dict()
	
	def addTexture(self, type, tex):
		self.textures[type] = tex
		
	def getTextures(self):
		return self.textures
   
class SceneItem:
	def __init__(self):
		self.vertex_data_id = 0
		self.name = "no_name"
		self.origin = []
		self.orientation = 0
		self.scale = []
		self.material_name = ""
	
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
	
	def hasMaterial(self):
		return self.material_name != ""
		
	def getMaterialName(self):
		return self.material_name
		
class VertexData:
	def __init__(self):
		self.vertices = []
		self.quads = []
		self.triangles = []
		self.faces = []
		self.texcoords = []
			
	def addVertex(self, vert):
		self.vertices.append((vert[0], vert[2], vert[1]))
		
	def addQuad(self, quad):
		self.quads.append(quad)
		
	def addTriangle(self, triangle):
		self.triangles.append(triangle)
	
	def addTexCoord(self, tc):
		self.texcoords.append(tc)
		
	# creates faces from quads and triangles
	def getFaces(self):
		for quad in self.quads:
			self.faces.append((quad.a, quad.b, quad.c, quad.d))
		return self.faces;
	
	def getVertices(self):
		return self.vertices;
	
	def getTexCoords(self):
		return self.texcoords
		
	def hasTexCoords(self):
		return len(self.texcoords) > 0
	
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
		self.materials = {}
		self.blender_materials = {}
		
	def addVertexData(self, vd):
		self.vertex_datas[vd.getID()] = vd;
		
	def addSceneItem(self, si):
		self.scene_items.append(si)
		
	def addMaterial(self, name, mat):
		self.materials[name] = mat
	
	def addBlenderMaterial(self, name, mat):
		self.blender_materials[name] = mat
	
	def getBlenderMaterial(self, name):
		return self.blender_materials[name]
		
	def getMaterial(self, name):
		return self.materials[name]
		
	def createMeshes(self):
		print("Creating meshes")
		
		# Create materials (using nodes)
		# ----------------------------------
		bpy.context.scene.use_nodes = True
		tree = bpy.context.scene.node_tree
		links = tree.links
			
		# clear default nodes
		for n in tree.nodes:
			tree.nodes.remove(n)
		
		rl = tree.nodes.new("R_LAYERS")
		
		for material_name in self.materials:
			mat = self.materials[material_name]
			textures = mat.getTextures()
			blender_mat = bpy.data.materials.new(material_name)
			print("Textures: ", textures)
			for texture_type in textures:
				tex = textures[texture_type]
				img = tex.createBlenderImage()
				
				if texture_type == R3F_MAT_DIFFUSE:
					blender_tex = bpy.data.textures.new(material_name +"_diffuse", type = 'IMAGE')
					blender_tex.image = img
					blender_tex_slot = blender_mat.texture_slots.add();
					blender_tex_slot.texture = blender_tex;
					blender_tex_slot.texture_coords = 'UV'
					
				elif texture_type == R3F_MAT_NORMAL:
					blender_tex = bpy.data.textures.new(material_name +"_normal", type = 'IMAGE')
					blender_tex.image = img
					blender_tex_slot = blender_mat.texture_slots.add();
					blender_tex_slot.texture = blender_tex;
					blender_tex_slot.texture_coords = 'UV'
					
					print("Texture: normal")
			print("Creating material: ", material_name)
		
		# Create meshes (vertex data representations)
		# ----------------------------------		
		for vertex_data_id in self.vertex_datas:
			print("ID: ", vertex_data_id)
			vd = self.vertex_datas[vertex_data_id]			
			m = bpy.data.meshes.new("mesh_" +str(vertex_data_id))
			self.addMesh(vertex_data_id, m)
			faces = vd.getFaces()
			m.from_pydata(vd.getVertices(), [], faces)
			m.update()			
			
			# create UV layer
			if vd.hasTexCoords():
				texcoords = vd.getTexCoords()
				uv_main = m.uv_textures.new()
				uv_main.name = "uv_" +str(vertex_data_id)
				uv_faces = m.uv_textures.active.data[:]
				
				for i,f in enumerate(uv_main.data):
					face = faces[i]
					uv_num = 0
					if len(face) == 4:
						f.uv1 = texcoords[face[0]]
						f.uv2 = texcoords[face[1]]
						f.uv3 = texcoords[face[2]]
						f.uv4 = texcoords[face[3]]
		
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
		
		#parse vertex datas
		print("Parse vertex datas: ", num_vertex_datas)
		for i in range(0, num_vertex_datas):
			vertex_data = VertexData()
			vertex_data_id = readUI32(dataFile)
			vertex_data.setID(vertex_data_id)
			Container.addVertexData(vertex_data)
			print("Vertex data id: ",vertex_data_id)
			
			#parse vertices
			num_vertices = readUI32(dataFile)
			print("Parse vertices: ", num_vertices)
			for i in range(0,num_vertices):
				v = readFloat3(dataFile)
				vertex_data.addVertex(v)
				
			#parse texcoords
			num_texcoords = readUI32(dataFile)
			print("Parsse texcoords: ",num_texcoords)
			for i in range(0, num_texcoords):
				v = readFloat2(dataFile)
				vertex_data.addTexCoord(v)
				
			#parse normals
			num_normals = readUI32(dataFile)
			print("Parsse normals: ",num_normals)
			for i in range(0, num_normals):
				v = readFloat3(dataFile)
				vertex_data.addTexCoord(v)
				
			# parse quads
			num_quads = readUI32(dataFile)
			for i in range(0, num_quads):
				quad = readQuad(dataFile)
				vertex_data.addQuad(quad)
				
			# and continue...
			cmd = readUI8(dataFile)
			parseCommand(cmd, dataFile, Container)

	elif cmd == R3F_SCENE_ITEMS:
		num_scene_items = readUI32(dataFile)
		print("Number of scene items: ", num_scene_items)		
		for i in range(0, num_scene_items):
			vertex_data_id = readUI32(dataFile)
			scene_item_name = readString(dataFile)
			origin = readFloat3(dataFile)
			scale = readFloat3(dataFile)
			orientation = readFloat4(dataFile)
			has_material = readUI8(dataFile)
			material_name = ""
			
			if has_material:
				material_name = readString(dataFile)
		
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
			print("material: ", material_name)
			print("--------")
		cmd = readUI8(dataFile)
		parseCommand(cmd, dataFile, Container)
		
	elif cmd == R3F_MATERIALS:
		num_materials = readUI32(dataFile)
		print("Num materials: ", num_materials)
		for i in range(0, num_materials):
			material_name = readString(dataFile) 
			num_textures = readUI8(dataFile)
			mat = Material()
			Container.addMaterial(material_name,mat)
			print("Material name: ", material_name)
			print("Material num textures: ", num_textures)
			for j in range(0, num_textures):
				print("Texture: ", j)
				texture_type = readUI8(dataFile)
				texture_file = readString(dataFile)
				tex = Texture()
				tex.setup(texture_type, texture_file)
				mat.addTexture(texture_type, tex)
				print("Texture file: ", texture_file)
				print("Texture type: ", texture_type)
			
			
		

	
def importR3F(filepath):
	data_file = open(filepath, "rb");
	scene = Scene()
	cmd = readUI8(data_file)
	parseCommand(cmd, data_file, scene)
	scene.createMeshes()
	return

# the ExportHelper gives us the file popup :)
# class RoxluLib(bpy.types.Operator, ImportHelper):
class RoxluLib(bpy.types.Operator):
	'''Tooltip'''
	bl_idname = "roxlulib.import"
	bl_label = "Import Roxlu file"
	filename_ext = ".r3f"
	filter_glob = StringProperty(default="*.r3f", options={'HIDDEN'})
	
	def execute(self, context):
		importR3F("/Users/diederickhuijbers/Documents/programming/c++/of/of_preRelease_v007_osx/apps/diederick-007/015_roxlu_r3f_test/bin/data/test.r3f")
		#importR3F(self.filepath)
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
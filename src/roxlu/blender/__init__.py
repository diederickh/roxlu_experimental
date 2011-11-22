import bpy
#import bpy_extras
import shutil
import struct
import mathutils
import os

# Good info: http://wiki.blender.org/index.php/Dev:2.5/Py/Scripts/Cookbook

#imports for file selector
from bpy_extras.io_utils import ImportHelper, ExportHelper
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
	
def writeUI8(file, value):
	data = struct.pack("B", value)
	file.write(data)
	
def writeUI16(file, value):
	data = struct.pack("h", value)
	file.write(data)

def writeUI32(file, value):
	data = struct.pack("i", value)
	file.write(data)
	
def writeString(file, value):
	data = struct.pack(">h", len(value))
	file.write(data)
	file.write(bytes(value, 'utf-8'))

def writeFloat2(file, value):	
	data = struct.pack("ff",value[0], value[1])
	file.write(data)
	
def writeFloat3(file, value):
	data = struct.pack("fff",value[0], value[1], value[2])
	file.write(data)

def writeFloat4(file, value):
	data = struct.pack("ffff",value[0], value[1], value[2], value[3])
	file.write(data)
	
def writeBool(file, value):
	writeUI8(file, value)
	
class Texture: 
	def __init__(self):
		self.type = R3F_MAT_NONE
		self.file = ""
	
	def setup(self, type, file):
		self.type = type
		self.file = file

	# get the filepath	
	def getFile(self):
		return self.file
		
	def getFileName(self):
		return os.path.basename(self.getFile())
		
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
		
	def getNumTextures(self):
		return len(self.textures)
		
	def hasTexture(self, type):
		return self.textures[type] 
		
	def getTexture(self, type):
		return self.textures[type]
		
class SceneItem:
	def __init__(self):
		self.vertex_data_id = 0
		self.name = "no_name"
		self.origin = []
		self.orientation = 0
		self.scale = []
		self.material_name = ""
		self.vertex_data_name = ""
	
	# not used anymore, use vertex data name
	def setVertexDataID(self, id):
		print("SETVERTEXDATAID IS NOT USED ANYMORE, USE VERTEXDATANAME");
		self.vertex_data_id = id
	
	def getVertexDataID(self):
		print("GETVERTEXDATAID IS NOT USED ANYMORE, USE VERTEXDATANAME");
		return self.vertex_data_id
		
	def setVertexDataName(self, n):
		self.vertex_data_name = n
	
	def getVertexDataName(self):
		return self.vertex_data_name;
		
	def setName(self, n):
		self.name = n
		
	def getName(self):
		return self.name
	
	# @todo pass in a correct object (use G2B3: openGl to Blender Vec3)	
	def setOrigin(self, o):
		self.origin = (o[0], o[2], o[1])
		
	def getOrigin(self):
		return self.origin
		
	def setOrientation(self, o):
		self.orientation = o;
		
	def getOrientationAsEuler(self):
		return mathutils.Quaternion((self.orientation[0], self.orientation[2],self.orientation[1]), -self.orientation[3]).to_euler()
	
	def getOrientation(self):
		return self.orientation
	
	# @todo pass in a correct object (use B2G3 function)	
	def setScale(self, s):
		self.scale = (s[0], s[2], s[1])
		
	def getScale(self):
		return self.scale
	
	def hasMaterial(self):
		return self.material_name != ""
		
	def setMaterialName(self, n):
		self.material_name = n
		
	def getMaterialName(self):
		return self.material_name

# blender 2 opengl conversion
def B2G3(o):
	return (o[0], -o[1], -o[2]);

def B2G4(o):
	return (o[0], -o[1], -o[2], o[3]);
# opengl 2 blender conversion
def G2B33(o):
	return (o[0], o[2], o[1])
	
class VertexData:
	def __init__(self):
		self.vertices = []
		self.quads = []
		self.triangles = []
		self.normals = []
		self.faces = []
		self.texcoords = []
		self.name = "none"
			
	def addVertex(self, vert):
		# from blender to opengl: self.vertices.append((vert[0], -vert[1], -vert[2]))
		#self.vertices.append((vert[0], -vert[1], -vert[2]))
		self.vertices.append(vert)
		
	def addQuad(self, quad):
		self.quads.append(quad)
		
	def addTriangle(self, triangle):
		self.triangles.append(triangle)
	
	def addTexCoord(self, tc):
		self.texcoords.append(tc)
		
	def addNormal(self, norm):
		self.normals.append((norm[0], norm[1], norm[2]))
		#self.normals.append((norm[0], -norm[1], -norm[2]))
		
	def getNormal(self, dx):
		return self.normals[dx]
	
	def getNumNormals(self):
		return len(self.normals)
		
	# creates faces from quads and triangles
	def getFaces(self):
		for quad in self.quads:
			self.faces.append((quad.a, quad.b, quad.c, quad.d))
		return self.faces;
	
	def getVertices(self):
		return self.vertices;
	
	def getNumVertices(self):
		return len(self.vertices)
	
	def getVertex(self, i):
		return self.vertices[i]
	
	def getTexCoords(self):
		return self.texcoords
		
	def getNumTexCoords(self):
		return len(self.texcoords)
		
	def getTexCoord(self, i):
		return self.texcoords[i]
		
	def hasTexCoords(self):
		return len(self.texcoords) > 0
	
		
	def setName(self, n):
		self.name = n
		
	def getName(self):
		return self.name
	
	# Used by export 	
	def addQuadVerticesAsTriangles(self, a, b, c, d):
		self.addVertex(a)
		self.addVertex(b)
		self.addVertex(c)
		self.addVertex(c)
		self.addVertex(d)
		self.addVertex(a)
	
	def addQuadNormalsAsTriangles(self, a, b, c, d):
		self.addNormal(a)
		self.addNormal(b)
		self.addNormal(c)
		self.addNormal(c)
		self.addNormal(d)
		self.addNormal(a)
		
	def addQuadTexCoordsAsTriangles(self, a, b, c, d):
		self.addTexCoord(a)
		self.addTexCoord(b)
		self.addTexCoord(c)
		self.addTexCoord(c)
		self.addTexCoord(d)
		self.addTexCoord(a)
		
	def getNumQuads(self):
		return len(self.quads)

	def getQuad(self, i):
		return self.quads[i]

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
		self.vertex_datas[vd.getName()] = vd;

	def getNumVertexDatas(self):
		return len(self.vertex_datas)
	
	def getNumSceneItems(self):
		return len(self.scene_items)
		
	def getNumMaterials(self):
		return len(self.materials)
	
	def addSceneItem(self, si):
		self.scene_items.append(si)
		
	def getSceneItem(self, dx):
		return self.scene_items[dx]
		
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
		for vertex_data_name in self.vertex_datas:
			print("ID: ", vertex_data_name)
			vd = self.vertex_datas[vertex_data_name]			
			m = bpy.data.meshes.new("mesh_" +vertex_data_name)
			self.addMesh(vertex_data_name, m)
			faces = vd.getFaces()
			m.from_pydata(vd.getVertices(), [], faces)
			m.update()			
			
			# create UV layer
			if vd.hasTexCoords():
				texcoords = vd.getTexCoords()
				uv_main = m.uv_textures.new()
				uv_main.name = "uv_" +vertex_data_name
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
			print("Scene item: ", si)
			m = self.getMeshForSceneItem(si)
			ob = bpy.data.objects.new(si.getName(), m)
			ob.location = si.getOrigin()
			ob.rotation_euler = si.getOrientationAsEuler()
			ob.scale = si.getScale()
			bpy.context.scene.objects.link(ob)
		
	def getMeshForSceneItem(self, si):
		return self.meshes["mesh" +si.getVertexDataName()]
		
	def addMesh(self, vertexDataID, mesh):
		self.meshes["mesh" +str(vertexDataID)] = mesh
		
	def getVertexData(self, vertexDataID):
		return self.vertex_datas[vertexDataID]
		
	def exportToR3F(self, filePath):
		dest_dir = os.path.dirname(filePath)
		
		print("Export to ", filePath, " path: ", dest_dir)
		#b = bpy_extras.io_utils.path_reference("./textures", dest_dir, dest_dir, 'RELATIVE')
		#print("---------------", b)
		# rel = bpy_extras.io_utils.path_reference(face_img.filepath, source_dir, dest_dir, path_mode, "", copy_set, face_img.library)
		file = open(filePath, "wb")
		writeUI8(file, R3F_VERTEX_DATAS)
		writeUI32(file, self.getNumVertexDatas())
		print("Number of vertex datas: ", self.getNumVertexDatas())
		
		# Store vertex datas
		for vertex_data_id in self.vertex_datas:
			vertex_data = self.vertex_datas[vertex_data_id]
			num_vertices = vertex_data.getNumVertices()
			writeString(file, vertex_data.getName())

			# vertices
			writeUI32(file, num_vertices)
			for i in range(0, num_vertices):
				v = vertex_data.getVertex(i)
				writeFloat3(file, v)
				
			# texcoords
			num_texcoords = vertex_data.getNumTexCoords()
			writeUI32(file, num_texcoords)
			for i in range(0, num_texcoords):
				writeFloat2(file, vertex_data.getTexCoord(i))
				
			# normals
			num_normals = vertex_data.getNumNormals()
			print("Num normals: ", num_normals)
			writeUI32(file, num_normals)
			for i in range(0, num_normals):
				writeFloat3(file, vertex_data.getNormal(i))
				
			# num quads
			num_quads = vertex_data.getNumQuads()
			
			writeUI32(file, num_quads)
			for i in range(0, num_quads):
				q = vertex_data.getQuad(i)
				writeUI32(file, q.a)
				writeUI32(file, q.b)
				writeUI32(file, q.c)
				writeUI32(file, q.d)
			
		
		# Store materials.
		num_materials = self.getNumMaterials()
		writeUI8(file, R3F_MATERIALS)
		writeUI32(file, num_materials)
		
		for mat_name in self.materials:
			m = self.getMaterial(mat_name)
			writeString(file, mat_name)
			
			num_textures = m.getNumTextures()
			writeUI8(file, num_textures)
			
			# only diffuse texture implemented for now
			for type in m.textures:
				texture = m.getTexture(type)
				# @todo we only need to create this path once
				if not os.path.exists(dest_dir +"/textures/"):
					os.makedirs(dest_dir +"/textures/")
				dest_file = dest_dir +"/textures/" +texture.getFileName()
				shutil.copyfile(texture.getFile(), dest_file)
				writeUI8(file, type)
				writeString(file, dest_file)
				
		
		# Store scene items
		num_scene_items = self.getNumSceneItems()
		writeUI8(file, R3F_SCENE_ITEMS)
		writeUI32(file, num_scene_items)
		print("Number of scene items: ", num_scene_items)
		for si in self.scene_items:
			writeString(file, si.getVertexDataName())
			writeString(file, si.getName())
			writeFloat3(file, si.getOrigin())
			writeFloat3(file, si.getScale())
			writeFloat4(file, si.getOrientation())
			writeBool(file, si.hasMaterial())
			if si.hasMaterial():
				writeString(file, si.getMaterialName())
		
		
		file.close()
	
def parseCommand(cmd, dataFile, Container):
	if cmd == R3F_VERTEX_DATAS: 
		num_vertex_datas = readUI32(dataFile)
		
		#parse vertex datas
		print("Parse vertex datas: ", num_vertex_datas)
		for i in range(0, num_vertex_datas):
			vertex_data = VertexData()
			vertex_data_name = readString(dataFile)
			vertex_data.setName(vertex_data_name)
			Container.addVertexData(vertex_data)
			print("Vertex data id: ",vertex_data_name)
			
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
		print("VERTEX_DATA: Next type is: ", cmd)
		parseCommand(cmd, dataFile, Container)

	elif cmd == R3F_SCENE_ITEMS:
		num_scene_items = readUI32(dataFile)
		print("Number of scene items: ", num_scene_items)		
		for i in range(0, num_scene_items):
			vertex_data_name = readString(dataFile)
			scene_item_name = readString(dataFile)
			origin = readFloat3(dataFile)
			scale = readFloat3(dataFile)
			orientation = readFloat4(dataFile)
			has_material = readUI8(dataFile)
			material_name = ""
			
			if has_material:
				material_name = readString(dataFile)
		
			si = SceneItem()
			si.setVertexDataName(vertex_data_name)
			si.setName(scene_item_name)
			si.setOrigin(origin)
			si.setOrientation(orientation)
			si.setScale(scale)
			Container.addSceneItem(si)

			print("--------")			
			print("Scene item:", i)
			print("vertex data: ", vertex_data_name)
			print("origin: ", origin)
			print("scale: ", scale)
			print("orientation: ", orientation)
			print("name:", scene_item_name)
			print("material: ", material_name)
			print("--------")
		# scene items are the last entries in a r3f format, so we stop here
		
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
			
		cmd = readUI8(dataFile)
		print("R3F_Materials: next: " ,cmd)
		parseCommand(cmd, dataFile, Container)	
		

	
def importR3F(filepath):
	data_file = open(filepath, "rb");
	scene = Scene()
	cmd = readUI8(data_file)
	parseCommand(cmd, data_file, scene)
	scene.createMeshes()
	return

# Export functions 
# ------------------------------------------------------------------------------

def rvec3d(v):
	return round(v[0], 6), round(v[1], 6), round(v[2], 6)

def rvec2d(v):
	return round(v[0], 6), round(v[1], 6)

def meshVertexToList(v):
	return (v.co.x, v.co.y, v.co.z)
	
# exports current scene (check io_mesh_ply addon for reference)
# handy: print(repr(some_var)) or, print(dir(some_var))
# tested on 2011.11.22 and the export + import both works.
def exportR3F(context, filepath):
	objects = context.selected_objects
	scene = Scene()
		
	#vertex_data_id = 0
	for obj in objects:
		vertex_data = VertexData()
		vertex_data.setName(obj.name +"_vertex_data")
		scene_item = SceneItem()
		scene_item.setName(obj.name)
		
		scene_item.setVertexDataName(vertex_data.getName())
		scene_item.setOrigin(B2G3(obj.location))
		#print("location: ", obj.location)
		
		#quat = mathutils.Euler(obj.rotation_euler[0], obj.rotation_euler[1], obj.rotation_euler[2])
		obj.rotation_mode='QUATERNION'
		#print(obj.rotation_quaternion)
		scene_item.setOrientation(B2G4(obj.rotation_quaternion))
		scene_item.setScale(B2G3(obj.scale))
	
		scene.addVertexData(vertex_data)
		scene.addSceneItem(scene_item)
		
		# will be an option... 
		use_uv_coords = True
		
		mesh = obj.to_mesh(context.scene, True, 'PREVIEW')
		if not mesh:
			raise Exception("Error, could not get mesh data from active object")
		
		# create material
		mat = None
		tex = None
		if len(mesh.materials) > 0:
			mat = mesh.materials[0]
			tex = bpy.data.textures[mat.active_texture_index]
			if mat and tex.image:
				t = Texture()
				t.setup(R3F_MAT_DIFFUSE, tex.image.filepath)
				m = Material()
				m.addTexture(R3F_MAT_DIFFUSE, t)
				scene.addMaterial(mat.name, m)
				scene_item.setMaterialName(mat.name)
		
		has_uv = (len(mesh.uv_textures) > 0)
		has_uv_vertex = (len(mesh.sticky) > 0)
		has_vcol = (len(mesh.vertex_colors) > 0)
				
		if (not has_uv) and (not has_uv_vertex):
			use_uv_coords = False
			
		if not use_uv_coords:
			has_uv = has_uv_vertex = False
		
		if has_uv:
			active_uv_layer = mesh.uv_textures.active
			if not active_uv_layer:
				use_uv_coords = False
				hash_uv = False
			else:
				active_uv_layer = active_uv_layer.data
				
		uv = None
		mesh_verts = mesh.vertices;
		texcoord = texcoord_key = normal = normal_key = None
		
		for i, f in enumerate(mesh.faces):
			smooth = f.use_smooth
			if not smooth:
				normal = tuple(f.normal)
				normal_key = rvec3d(normal)
			
			if has_uv:
				uv = active_uv_layer[i]
				#uv = uv.uv1, uv.uv2, uv.uv3, uv.uv4
			
			face_verts = f.vertices
			
			if len(face_verts) == 4:
				vertex_data.addQuadVerticesAsTriangles( 						\
						 B2G3(meshVertexToList(mesh_verts[face_verts[0]])) 	\
						,B2G3(meshVertexToList(mesh_verts[face_verts[1]]))		\
						,B2G3(meshVertexToList(mesh_verts[face_verts[2]])) 	\
						,B2G3(meshVertexToList(mesh_verts[face_verts[3]])) 	\
				)
							
				vertex_data.addQuadNormalsAsTriangles( 				\
						 B2G3(mesh_verts[face_verts[0]].normal)		\
						,B2G3(mesh_verts[face_verts[1]].normal)		\
						,B2G3(mesh_verts[face_verts[2]].normal)		\
						,B2G3(mesh_verts[face_verts[3]].normal)		\
				)
				
				if has_uv:
					vertex_data.addQuadTexCoordsAsTriangles(uv.uv1, uv.uv2, uv.uv3, uv.uv4)
					
			elif len(face_verts) == 3:
				vertex_data.addVertex(B2G3(meshVertexToList(mesh_verts[face_verts[0]])))
				vertex_data.addVertex(B2G3(meshVertexToList(mesh_verts[face_verts[1]])))
				vertex_data.addVertex(B2G3(meshVertexToList(mesh_verts[face_verts[2]])))
				
				vertex_data.addNormal(B2G3(mesh_verts[face_verts[0]].normal))
				vertex_data.addNormal(B2G3(mesh_verts[face_verts[1]].normal))
				vertex_data.addNormal(B2G3(mesh_verts[face_verts[2]].normal))
				
				if has_uv:
					vertex_data.addTexCoord(uv.uv1)
					vertex_data.addTexCoord(uv.uv2)
					vertex_data.addTexCoord(uv.uv3)
		
	scene.exportToR3F(filepath)
		
		
		

#class RoxluExport(bpy.types.Operator, ExportHelper):
class RoxluExport(bpy.types.Operator):
	bl_idname = "roxlulib.export"
	bl_label = "Export Roxlu file (r3f)"
	filename_ext = ".r3f"
	filter_glob = StringProperty(default="*.r3f", options={'HIDDEN'})
	
	def execute(self, context):
		exportR3F(context, "/Users/diederickhuijbers/Downloads/test.r3f")
		#exportR3F(context, self.filepath)
		return {'FINISHED'}

# ------------------------- end export functions -------------------------------

# the ExportHelper gives us the file popup :)
# class RoxluLib(bpy.types.Operator, ImportHelper):
class RoxluImport(bpy.types.Operator, ImportHelper):
	'''Tooltip'''
	bl_idname = "roxlulib.import"
	bl_label = "Import Roxlu file (r3f)"
	filename_ext = ".r3f"
	filter_glob = StringProperty(default="*.r3f", options={'HIDDEN'})
	
	def execute(self, context):
		#importR3F("/Users/diederickhuijbers/Documents/programming/c++/of/of_preRelease_v007_osx/apps/diederick-007/015_roxlu_r3f_test/bin/data/test.r3f")
		importR3F(self.filepath)
		return {'FINISHED'}
		
def menu_func_import(self, context):
	self.layout.operator(RoxluImport.bl_idname, text="Roxlu 3D Format (*.r3f)")
	

def menu_func_export(self, context):
    self.layout.operator(RoxluExport.bl_idname, text="Roxlu 3D Format (*.r3f)")

def register():
	bpy.utils.register_module(__name__)
	bpy.types.INFO_MT_file_import.append(menu_func_import)
	bpy.types.INFO_MT_file_export.append(menu_func_export)
	#bpy.utils.register_class(RoxluLib)
	
	
def unregister():
	bpy.utils.unregister_class(RoxluLib)
		
if __name__ == "__main__":
	register()
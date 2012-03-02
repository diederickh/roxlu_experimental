openGL
================================================================================

Vertex Data
--------------------------------------------------------------------------------
Use the :cpp:class:`VertexData` class as a container for your vertices, texture
coordinates, colors, normals, tangents and binormals. The VertexData class can
convert the stored information to any of the generic vertex types as defined in 
``VertexTypes``

.. code-block:: c++

	VertexData vd;
	
	// create a quad
	float s = 0.5;
	vd.addVertex(-s, s, 0);
	vd.addVertex(s, s, 0);
	vd.addVertex(s,-s, 0);
	vd.addVertex(-s,-s, 0);
	
	// with texture coordinates
	vd.addTexCoord(0,0);
	vd.addTexCoord(1,0);
	vd.addTexCoord(1,1);
	vd.addTexCoord(0,1);
	
	// We must delete the vertices after using.
	VertexP* vertices = vd.getVertexP();
	
	// Getting VertexPT
	VertexPT* vertices_pt = vd.getVertexPT();
	

Using shaders
--------------------------------------------------------------------------------

Example loading and setting up a :cpp:class:`Shader`.

.. code-block:: c++

	Shader shader;
	
	// Create two files: rays.vert and rays.frag in your data dir.
	shader.load("rays"); 
	shader.enable();
	
	// Tell the shader what attributes we have
	shader.addAttribute("tex")
			.addAttribute("pos");
	
	// Tell the shader what uniforms we have			
	shader.addUniform("diffuse_texture")
			.addUniform("projection_matrix")
			.addUniform("modelview_matrix")
			.addUniform("exposure")
			.addUniform("decay")
			.addUniform("weight");
			
	// Set some uniform values
	shader.uniform1f("exposure", 1.5f)
			.uniform1f("density", 0.999f)
			.uniform1f("weight", 1.0f);
	

Example vertex shader

.. code-block:: c
	
	attribute vec4 pos;
	attribute vec2 tex;
	
	uniform mat4 projection_matrix;
	uniform mat4 modelview_matrix;
	varying vec2 v_tex;
	
	void main() {
		gl_Position = projection_matrix * modelview_matrix * pos; 
		v_tex = tex;
	}

	
Example fragment shader

.. code-block:: c

	void main() {
		gl_FragColor = vec4(1.0, 0.0, 0.5, 1.0);
	}
	
	
Using a VBO and Shader
--------------------------------------------------------------------------------


.. code-block:: c++

	void testApp::setup(){
		ofBackground(33);
		ofSetFrameRate(60);
		ofSetVerticalSync(true);
		
		
		// Create indices.
		int rows = 10;
		int cols = 10;
		float width = 0.1;
		float height = 0.1;
		for(int i = 0; i < cols; ++i) {
			for(int j = 0; j < rows; ++j) {
				float x = i * width;
				float y = j * height;
				vd.addVertex(x,y,0);
			}
		}
		
		for(int i = 0; i < cols-1; ++i) {
			for(int j = 0; j < rows-1; ++j) {
				int a = DX(i,j, cols);
				int b = DX(i+1, j, cols);
				int c = DX(i+1, j+1, cols);
				int d = DX(i, j+1, cols);
				vd.addQuadIndices(a,b,c,d);
			}
		}
		
		// Make sure VAO keeps state of the VBO
		vao.bind();
		
		// Initialize shader.
		shader.load("shader");
		shader.addUniform("projection_matrix").addUniform("view_matrix");
		shader.addAttribute("pos");
		shader.enableVertexAttribArray("pos");
	
		
		// Fill VBO
		vbo.setIndices(vd.getIndicesPtr(), vd.getNumIndices());
		vbo.setVertexData(vd.getVertexP(), vd.size());
		vbo.setPosVertexAttribPointer(shader.getAttribute("pos"));	
		
		projection_matrix.perspective(45.0f, ofGetWidth()/ofGetHeight(), 0.1, 100);
		view_matrix.translate(-1,0,-3);
	}
	
	
	
	void testApp::draw(){
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		shader.enable();
		shader.uniformMat4fv("projection_matrix", projection_matrix.getPtr());
		shader.uniformMat4fv("view_matrix", view_matrix.getPtr());
		vbo.drawElements(GL_QUADS, vd.size()*4);
	}
	
	
API reference
--------------------------------------------------------------------------------

VBO
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


.. cpp:function:: 	 VBO::VBO()


.. cpp:function:: 	VBO&  VBO::setVertexData(VertexData& data)

	:param VertexData& data: 

.. cpp:function:: 	VBO&  VBO::setVertexData(VertexData* data)

	:param VertexData* data: 

.. cpp:function:: 	VBO&  VBO::setVertexData(VertexP* data, int num)

	:param VertexP* data: 
	:param int num: 

.. cpp:function:: 	VBO&  VBO::setVertexData(VertexPN* data, int num)

	:param VertexPN* data: 
	:param int num: 

.. cpp:function:: 	VBO&  VBO::setVertexData(VertexPT* data, int num)

	:param VertexPT* data: 
	:param int num: 

.. cpp:function:: 	VBO&  VBO::setVertexData(VertexPNC* data, int num)

	:param VertexPNC* data: 
	:param int num: 

.. cpp:function:: 	VBO&  VBO::setVertexData(VertexPTN* data, int num)

	:param VertexPTN* data: 
	:param int num: 

.. cpp:function:: 	VBO&  VBO::setVertexData(VertexPTNT* data, int num)

	:param VertexPTNT* data: 
	:param int num: 

.. cpp:function:: 	VBO&  VBO::setVertexData(VertexPTNC* data, int num)

	:param VertexPTNC* data: 
	:param int num: 

.. cpp:function:: 	VBO&  VBO::setVertexData(VertexPTNTB* data, int num)

	:param VertexPTNTB* data: 
	:param int num: 

.. cpp:function:: 	VBO&  VBO::updateVertexData(VertexData* data)

	:param VertexData* data: 

.. cpp:function:: 	VBO&  VBO::setVertices(const float* vertices, int numCoords, int num, int usage = GL_STATIC_DRAW)

	:param const float* vertices: 
	:param int numCoords: 
	:param int num: 
	:param int usage = GL_STATIC_DRAW: 

.. cpp:function:: 	VBO&  VBO::setTexCoords(const float* texCoords, int num, int usage = GL_STATIC_DRAW)

	:param const float* texCoords: 
	:param int num: 
	:param int usage = GL_STATIC_DRAW: 

.. cpp:function:: 	VBO&  VBO::setIndices(const int* indices, int num, int usage = GL_STATIC_DRAW)

	:param const int* indices: 
	:param int num: 
	:param int usage = GL_STATIC_DRAW: 

.. cpp:function:: 	VBO&  VBO::setColors(const float* colors, int numColors, int usage = GL_STATIC_DRAW)

	:param const float* colors: 
	:param int numColors: 
	:param int usage = GL_STATIC_DRAW: 

.. cpp:function:: 	VBO&  VBO::bind()


.. cpp:function:: 	VBO&  VBO::unbind()



.. cpp:function:: 	VBO&  VBO::drawElements(int drawMode, int total)

	:param int drawMode: 
	:param int total: 


.. cpp:function:: 	void  VBO::setNormVertexAttribPointer(GLuint index)

	:param GLuint index: 

.. cpp:function:: 	void  VBO::setPosVertexAttribPointer(GLuint index)

	:param GLuint index: 

.. cpp:function:: 	void  VBO::setTexVertexAttribPointer(GLuint index)

	:param GLuint index: 


.. cpp:function:: 	bool VBO::hasIndices(); 


.. cpp:function:: 	return  VBO::hasType(VBOType type)

	:param VBOType: 


VertexData
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.. cpp:class:: VertexData
	
	VertexData class
	
.. cpp:function:: 	 VertexData::VertexData()
	
	Default constructor.

.. cpp:function:: 	 VertexData::VertexData(string meshName)

	:param string meshName:	You can give the vertex data a name, which can be
							used by exporters. 

.. cpp:function:: 	int  VertexData::addVertex(const Vec3& vec)

	:param const Vec3& vec:		Add a vertex

.. cpp:function:: 	int  VertexData::addVertex(const float x, const float y, const float z)

	:param const float x: 	X of vertex
	:param const float y: 	Y of vertex
	:param const float z: 	Z of vertex

.. cpp:function:: 	void  VertexData::addTexCoord(const Vec2& vec)

	:param const Vec2& vec: 	Add this texture coordinate

.. cpp:function:: 	void  VertexData::addTexCoord(const float x, const float y)

	Add a texture coordinate:
	
	:param const float x: 	x-value
	:param const float y: 	y-value

.. cpp:function:: 	void  VertexData::addColor(const Vec3& color)

	:param const Vec3& color: 

.. cpp:function:: 	void  VertexData::addColor(const Color4& color)

	:param const Color4& color: 

.. cpp:function:: 	void  VertexData::addColor(const float r, const float g, const float b)
	
	:param const float r: 
	:param const float g: 
	:param const float b: 

.. cpp:function:: 	void  VertexData::addNormal(const float x, const float y, const float z)

	:param const float x: 
	:param const float y: 
	:param const float z: 

.. cpp:function:: 	void  VertexData::addNormal(const Vec3& vec)

	:param const Vec3& vec: 

.. cpp:function:: 	void  VertexData::addIndex(const int& index)

	:param const int& index: 

.. cpp:function:: 	int  VertexData::addTriangle(Triangle t)

	:param Triangle t: 

.. cpp:function:: 	int  VertexData::addTriangle(int a, int b, int c)

	:param int a: 
	:param int b: 
	:param int c: 

.. cpp:function:: 	int  VertexData::addTriangleAndIndices(int a, int b, int c)

	:param int a: 
	:param int b: 
	:param int c: 

.. cpp:function:: 	int  VertexData::addQuad(int a, int b, int c, int d)

	:param int a: 
	:param int b: 
	:param int c: 
	:param int d: 

.. cpp:function:: 	int  VertexData::addQuad(Quad q)

	:param Quad q: 

.. cpp:function:: 	int  VertexData::addQuadAndIndices(int a, int b, int c, int d)

	:param int a: 
	:param int b: 
	:param int c: 
	:param int d: 

.. cpp:function:: 	void  VertexData::addQuadIndices(int a, int b, int c, int d)

	:param int a: 
	:param int b: 
	:param int c: 
	:param int d: 

.. cpp:function:: 	void  VertexData::debugDraw(int drawMode = GL_TRIANGLES)

	:param int drawMode = GL_TRIANGLES: 

.. cpp:function:: 	void  VertexData::debugDrawQuad(int quadNum)

	:param int quadNum: 

.. cpp:function:: 	const float*  VertexData::getVerticesPtr()


.. cpp:function:: 	const float*  VertexData::getTexCoordsPtr()


.. cpp:function:: 	const float*  VertexData::getColorsPtr()


.. cpp:function:: 	const float*  VertexData::getNormalsPtr()


.. cpp:function:: 	const int*  VertexData::getIndicesPtr()


.. cpp:function:: 	Triangle*  VertexData::getTrianglePtr(int triangle)

	:param int triangle: 

.. cpp:function:: 	Quad*  VertexData::getQuadPtr(int quad)

	:param int quad: 

.. cpp:function:: 	int  VertexData::getNumVertices()


.. cpp:function:: 	int  VertexData::getNumTexCoords()


.. cpp:function:: 	int  VertexData::getNumColors()


.. cpp:function:: 	int  VertexData::getNumNormals()


.. cpp:function:: 	int  VertexData::getNumTangents()


.. cpp:function:: 	int  VertexData::getNumIndices()


.. cpp:function:: 	int  VertexData::getNumTriangles()


.. cpp:function:: 	int  VertexData::getNumQuads()


.. cpp:function:: 	Vec2  VertexData::getTexCoord(int index)

	:param int index: 

.. cpp:function:: 	Vec2*  VertexData::getTexCoordPtr(int index)

	:param int index: 

.. cpp:function:: 	Vec3  VertexData::getVertex(int index)

	:param int index: 

.. cpp:function:: 	Vec3*  VertexData::getVertexPtr(int index)

	:param int index: 

.. cpp:function:: 	Vec3&  VertexData::getVertexRef(int index)

	:param int index: 

.. cpp:function:: 	Vec3  VertexData::getNormal(int index)

	:param int index: 

.. cpp:function:: 	Vec3*  VertexData::getNormalPtr(int index)

	:param int index: 

.. cpp:function:: 	Vec3&  VertexData::getNormalRef(int index)

	:param int index: 

.. cpp:function:: 	VertexP*  VertexData::getVertexP()


.. cpp:function:: 	VertexPT*  VertexData::getVertexPT()


.. cpp:function:: 	VertexPN*  VertexData::getVertexPN()


.. cpp:function:: 	VertexPTN*  VertexData::getVertexPTN()


.. cpp:function:: 	VertexPTNT*  VertexData::getVertexPTNT()


.. cpp:function:: 	VertexPNC*  VertexData::getVertexPNC()


.. cpp:function:: 	VertexPTNTB*  VertexData::getVertexPTNTB()


.. cpp:function:: 	void  VertexData::clearAttribs()


.. cpp:function:: 	void  VertexData::enablePositionAttrib()


.. cpp:function:: 	void  VertexData::enableNormalAttrib()


.. cpp:function:: 	void  VertexData::enableColorAttrib()


.. cpp:function:: 	void  VertexData::enableTexCoordAttrib()


.. cpp:function:: 	void  VertexData::disablePositionAttrib()


.. cpp:function:: 	void  VertexData::disableNormalAttrib()


.. cpp:function:: 	void  VertexData::disableColorAttrib()


.. cpp:function:: 	void  VertexData::disableTexCoordAttrib()


.. cpp:function:: 	void  VertexData::clear()


.. cpp:function:: 	void VertexData::setNormal(int index,Vec3 normal)

	:param int dx: 
	:param Vec3 normal: 

.. cpp:function:: 	void VertexData::setVertex(int index, Vec3 vert)

	:param int dx: 
	:param Vec3 position: 

.. cpp:function:: 	void VertexData::setVertex(int dx, float x, float y, float z)

	:param int dx: 
	:param float x: 
	:param float y: 
	:param float z) { vertices[dx].set(x: 
	:param y: 
	:param z: 

.. cpp:function:: 	void  VertexData::setName(string n)

	:param string n: 

.. cpp:function:: 	string  VertexData::getName()


Shader
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	

.. cpp:class:: Shader

	Shader class
	
.. cpp:function::	Shader()

	Create a new Shader object. Don't forget to set the name before you start
	loading and compiling the shader.
	
.. cpp:function::	Shader(const std::string& name)

   Create a new Shader object with the given name. The name is used to load the 
   files from the data directory.
	
.. cpp:function::	Shader& Shader::load(const std::string& name)

	Load the shader source code using the given name.
	
.. cpp:function::	Shader& Shader::load()

	Load the shader source code with the previously set name. 
	
.. cpp:function::	void Shader::create(const std::string& vertexSource, std::string& fragmentSource)
	
	This function does the actual work by creating a vertex and fragment shader
	object using the given source code.
	
.. cpp:function::	Shader& Shader::addAttribute(const std::string& name)

	Uses ``glGetAttribLocation(prog_id, name)`` and stores the location of the 
	attribute as a member. You can use ``getAttribute(std::string name)`` to 
	retrieve the location of the attribute in the compiled shader.

.. cpp:function::	Shader& Shader::addUniform(const std::string& name)

	Retrieves the location of the uniform using ``glGetUniformLocation(prog_id, name)``.
	The location is stored in the ``Shader`` object and you can use 
	``GetUniform(std::string name)`` to retrieve the uniform location.
	
.. cpp:function::	GLint Shader::getUniform(const std::string& name)

	Returns the uniform location for the given uniform name.
	
.. cpp:function::	GLint Shader::getAttribute(const std::string& name)

	Returns the attribute location for the given attribute name.

.. cpp:function::	string Shader::getVertexSource(bool inDataPath = true)

	Get the vertex shader source using the name of the shader object.

.. cpp:function::	string Shader::getFragmentSource(bool inDataPath = true)

	Get the fragment shader source using the name of the shader object.
	
.. cpp:function::	void Shader::enable()

	Start using the shader application. Call ``disable()`` to disable it again.	

.. cpp:function::	void Shader::disable()

	Disable the shader application. Call ``enable()`` to enable it again.
	


attribute vec4 pos;
attribute vec2 tex;
attribute vec3 norm;

uniform mat4 modelview;
uniform mat4 projection;
uniform mat4 modelview_projection;

varying vec2 texcoord;
varying vec3 normal;
varying vec3 light_direction;

void main() {
	gl_Position = modelview_projection * pos ;

	// correct: get direction from light to vertex in the same space (view/cam) ("modelview": model-space and view-space)
	vec4 vertex_position = modelview * pos;
	vec4 light_position = modelview * vec4(-4.0,0.4,0.0,1.0);
	light_direction = vec3(normalize(vertex_position-light_position));
	
	texcoord = tex;
	normal = norm;

}
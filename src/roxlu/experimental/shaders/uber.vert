attribute vec4 pos;
attribute vec2 tex;

uniform mat4 modelview;
uniform mat4 projection;
uniform mat4 modelview_projection;

varying vec2 texcoord;

void main() {
	mat4 mvp = modelview*projection;
	gl_Position = modelview_projection * pos ;
	texcoord = tex;
//	gl_Position = mvp * pos ;
}
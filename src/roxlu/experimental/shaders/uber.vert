attribute vec4 pos;
uniform mat4 modelview;
uniform mat4 projection;
uniform mat4 modelview_projection;


void main() {
	gl_Position = modelview_projection * pos ;
}
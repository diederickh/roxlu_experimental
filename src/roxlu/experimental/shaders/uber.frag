uniform sampler2D diffuse_texture;
varying vec2 texcoord;

void main() {
	vec4 diffuse_color = texture2D(diffuse_texture, texcoord);
	gl_FragColor = vec4(1.0, 0.0, 0.5,1.0);
	gl_FragColor = gl_FragColor + diffuse_color ;
	gl_FragColor = diffuse_color;
}
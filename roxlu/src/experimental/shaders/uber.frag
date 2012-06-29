uniform sampler2D diffuse_texture;

varying vec2 texcoord;
varying vec3 normal;
varying vec3 light_direction;

void main() {
	vec4 diffuse_color = texture2D(diffuse_texture, texcoord);
	gl_FragColor = vec4(1.0, 0.0, 0.5,1.0);
	gl_FragColor = gl_FragColor + diffuse_color ;
	gl_FragColor = diffuse_color *vec4(1.0, 1.0, 1.0, 1.0);
	
	// correct!
	float n_dot_l = max(dot(normal, light_direction), 0.0);
	if(n_dot_l > 0.0) {
		gl_FragColor += n_dot_l * vec4(1.0, 0.0, 0.0, 1.0);
	}

}
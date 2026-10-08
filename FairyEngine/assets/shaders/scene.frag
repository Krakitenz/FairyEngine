#version 330 core
in vec2 vUV;
out vec4 FragColor;
uniform sampler2D uTex;
uniform float uOpacity;
void main() {
	vec4 c = texture(uTex, vUV);
	FragColor = vec4(c.rgb, c.a * uOpacity);
}

#version 330 core
in vec2 vUV;
out vec4 FragColor;
uniform sampler2D uTex;
uniform vec4 uColor;
void main() {
	float alpha = texture(uTex, vUV).r;
	FragColor = vec4(uColor.rgb, uColor.a * alpha);
}

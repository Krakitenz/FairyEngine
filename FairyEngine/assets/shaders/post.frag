#version 330 core
in vec2 vUV;
out vec4 FragColor;

uniform sampler2D uScene;
uniform vec2 uResolution;
uniform vec2 uFocusPoint;       // где чётко (в UV 0..1)
uniform float uDofStrength;     // 0 = выкл
uniform float uSaturation;      // 1.0 = норм
uniform float uBrightness;      // 1.0 = норм
uniform float uContrast;        // 1.0 = норм
uniform vec3 uTint;             // цветовой сдвиг
uniform float uVignette;        // 0 = выкл, 1 = норм
uniform float uFade;             // 0 = обычная сцена, 1 = чёрный экран

void main() {
    vec3 color;

    // === DOF: размытие, зависит от расстояния до точки фокуса ===
    if (uDofStrength > 0.0) {
        vec2 d = (vUV - uFocusPoint) * vec2(uResolution.x / uResolution.y, 1.0);
        float blur = clamp(length(d) * uDofStrength, 0.0, 1.0) * 3.0;

        vec2 texel = blur / uResolution;
        vec3 sum = vec3(0.0);
        float count = 0.0;
        for (int x = -2; x <= 2; ++x) {
            for (int y = -2; y <= 2; ++y) {
                sum += texture(uScene, vUV + vec2(x, y) * texel).rgb;
                count += 1.0;
            }
        }
        color = sum / count;
    } else {
        color = texture(uScene, vUV).rgb;
    }

    // === Цветокоррекция ===
    color *= uTint;
    color *= uBrightness;
    color = (color - 0.5) * uContrast + 0.5;

    float gray = dot(color, vec3(0.299, 0.587, 0.114));
    color = mix(vec3(gray), color, uSaturation);

    // === Виньетка ===
    vec2 vc = vUV - 0.5;
    float vig = 1.0 - dot(vc, vc) * uVignette;
    color *= vig;

    color = mix(color, vec3(0.0), clamp(uFade, 0.0, 1.0));
    FragColor = vec4(color, 1.0);
}
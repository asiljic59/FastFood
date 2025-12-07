#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D tex0;
uniform float cook; // cook progress [0..2]

void main()
{
    vec4 baseColor = texture(tex0, TexCoord);

    // Fully raw → cooked → burned
    vec3 rawColor    = vec3(0.82, 0.22, 0.22);
    vec3 cookedColor = vec3(0.45, 0.24, 0.10);
    vec3 burnedColor = vec3(0.25, 0.18, 0.10);

    vec3 resultColor;

    if (cook < 1.0) {
        float t = cook / 1.0;
        resultColor = mix(rawColor, cookedColor, t);
    } else {
        float t = (cook - 1.0) / 1.0;
        resultColor = mix(cookedColor, burnedColor, t);
    }

    resultColor *= baseColor.rgb;

    FragColor = vec4(resultColor, baseColor.a);
}

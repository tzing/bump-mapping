#version 400

uniform sampler2D textureSampler;
uniform sampler2D normalSampler;

uniform vec4 ambient;
uniform vec4 diffuse;
uniform vec4 specular;
uniform float shininess;

uniform uint use_bump;

in vec2 vUV;
in vec3 vLiDirect;
in vec3 vHalf;

out vec4 display_color;

const vec3 flat_normal = vec3(0, 0, 1);

void main() {
    // basic
    vec4 default_color = vec4(texture(textureSampler, vUV).rbg, 1);
    vec3 normal = (use_bump == 0) ? (texture(normalSampler, vUV).rgb *2 -1) : (flat_normal);

    // ambient
    vec4 Iamb = ambient;

    // diffuse
    float difMap = dot(vLiDirect, normal);
    vec4 Idiff = diffuse * clamp(difMap, 0, 1);

    // specular
    float shine = dot(vHalf, normal);
    vec4 Ispec = specular * pow(shine, shininess);

    // output
    display_color = default_color * clamp(Iamb +Idiff +Ispec, 0, 1);
}

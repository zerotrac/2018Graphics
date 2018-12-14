#version 440

layout (location = 0) in vec3 v;
layout (location = 1) in vec2 vt;
layout (location = 2) in vec3 vn;

out vec3 LightIntensity;

uniform vec3 LightP1;
uniform vec3 LightP2;
uniform vec3 LightP3;
uniform vec3 LightP4;
uniform vec3 LightP5;

uniform vec3 Kd;
uniform vec3 La;

uniform vec3 Ld1;
uniform vec3 Ld2;
uniform vec3 Ld3;
uniform vec3 Ld4;
uniform vec3 Ld5;

uniform vec3 Eye;

uniform float f;

uniform bool Texture;
uniform sampler2D Tex;

uniform mat4 MVP;

vec3 phong(vec3 Light, vec3 Ld)
{
    vec3 phong_s = normalize(Light - v);
    vec3 phong_v = normalize(Eye - v);
    vec3 phong_n = normalize(vn);
    vec3 phong_r = reflect(-phong_s, phong_n);

    vec3 ambient = La;

    float s_dot_n = max(dot(phong_s, phong_n), 0.0);
    vec3 diffuse = Ld * s_dot_n;

    float r_dot_v = max(dot(phong_r, phong_v), 0.0);
    vec3 spec = Ld * pow(r_dot_v, f);

    if (Texture)
    {
        return (ambient + diffuse + spec) * texture2D(Tex, vt).xyz;
    }
    else
    {
        return (ambient + diffuse + spec) * Kd;
    }
}

void main()
{
    LightIntensity = phong(LightP1, Ld1) + phong(LightP2, Ld2) + phong(LightP3, Ld3) + phong(LightP4, Ld4) + phong(LightP5, Ld5);
    // LightIntensity = texture2D(Tex, vt).xyz;
    gl_Position = MVP * vec4(v, 1.0);
}

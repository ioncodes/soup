#version 330 core

#define PI 3.14159

uniform float time;
uniform vec2 resolution;

out vec4 out_color;

float round(float v) {
    if (v - floor(v) >= 0.5)
        return floor(v) + 1.0;
    else
        return floor(v);
}

vec2 round(vec2 v) {
    vec2 ret = vec2(0.0);
    if (v.x - floor(v.x) >= 0.5)
        ret.x = floor(v.x) + 1.0;
    else
        ret.x = floor(v.x);
    if (v.y - floor(v.y) >= 0.5)
        ret.y = floor(v.y) + 1.0;
    else
        ret.y = floor(v.y);
    return ret;
}

float triwave(float x) {
    return 1.0 - 4.0 * abs(0.5 - fract(0.5 * x + 0.25));
    // return 1.0 - 1.0 * abs(0.5 + 1. - fract(0.5 * x + 0.25)); // test
}

float rand(vec2 co) {
    float t = mod(time, 16.0);
    return fract(sin(dot(co.xy, vec2(1.9898, 7.233))) * t * t);
}

float BarrelPower = 1.085;
vec2 Distort(vec2 p) {
    float theta = atan(p.y, p.x);
    float radius = length(p);
    radius = pow(radius, BarrelPower);
    p.x = radius * cos(theta);
    p.y = radius * sin(theta);
    return 0.5 * (p + 1.0);
}


// whats on the screen=============================================
vec3 shader(vec2 p, vec2 resolution) {
    vec2 pos = (p / resolution.xy);

    vec3 color = vec3(0.0);

    float wave = 1. - abs(pos.y - (sin(pos.x * PI * 4. + time) * 0.25 + 1.));

    color = vec3(0., pow(wave, 16.), 0.);
    color += vec3(pow(wave, 32.));

    return vec3(color);
}
//=====================================================================


float pixelsize = 8.0;

void main(void) {
    vec2 position = (gl_FragCoord.xy);

    vec3 color = vec3(0.0);

    vec2 dposition = Distort(position / resolution - 0.5) * (resolution * 2.0);

    vec2 rposition = round(((dposition - (pixelsize / 2.0)) / pixelsize));

    color = vec3(shader(rposition, resolution / pixelsize));

    color = clamp(color, 0.0625, 1.0);

    // color *= (rand(rposition)*0.25+0.75);

    color *= abs(sin(rposition.y * 8.0 + (time * 16.0)) * 0.15 + 0.85);

    color *= vec3(clamp(abs(triwave(dposition.x / pixelsize)) * 3.0, 0.0, 1.0));
    color *= vec3(clamp(abs(triwave(dposition.y / pixelsize)) * 3.0, 0.0, 1.0));

    float darkness = sin((position.x / resolution.x) * PI) *
                   sin((position.y / resolution.y) * PI);

    color *= vec3(clamp(darkness * 4.0, 0.0, 1.0));

    out_color = vec4(color, 1.0);
}

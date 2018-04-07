#version 330 core

uniform float time;
uniform float spectrum;
uniform vec2 resolution;
uniform float hits;
uniform float random;
uniform float move;
uniform float spectrum_pos;

out vec4 out_color;

#define PI 3.14159265

float crispy = max(min(sin(time)*2000.,500.),-500.); // toggle back white

void main(void) {
    vec3 red = vec3(0.96, 0.26, 0.21);
    vec3 pink = vec3(0.91, 0.12, 0.39);
    vec3 purple = vec3(0.61, 0.15, 0.69);

    vec4 color = vec4(0.0, 0.0, 0.0, 1.0);

    vec2 center = vec2(resolution.x / 2.0, resolution.y / 2.0);
    for(int i = 1; i < hits; i++) {
        vec2 loc = gl_FragCoord.xy;
        loc.x += (1. - ((i + 1.) / 3.)) * resolution.x;
        if(move == 1.0) {
            //loc.x *= tan(loc.x) + spectrum_pos;
            loc.x += spectrum_pos * sin(time);
            //loc.y *= atan(time);
            // loc.y *= sin(loc.x);
            // loc.y *= sin(loc.x) * (time * 10.);
            if(i == 1) { loc.y *= (sin(loc.x) + 1.) * (time / 20.) * spectrum_pos; }
            if(i == 3) { loc.y *= (sin(loc.x) + 1.) * (time / 20.) * spectrum_pos; }
        }
        float radius = length(loc - center);
        float condition = 100.0;
        if(move == 1.0 && i == 2) { condition += (condition * spectrum) / 10.; }
        if(move == 1.0 && i == 1) { condition += ((condition * spectrum) / 10.) + time; }
        if(move == 1.0 && i == 3) { condition += ((condition * spectrum) / 10.) + time; }
        //if(i == 2 && move == 1.0) { condition += condition * sin(time); }
        //if((i == 1 || i == 3) && move == 1.0) { condition += condition * cos(time); }
        if(radius < condition) {
            if(i == 1) { color = vec4(red, 1.); }
            else if(i == 2) { color = mix(color, vec4(pink, 1.), 1.); }
            else { color = mix(color, vec4(purple, 1.), 1.); }
        }
    }
    out_color = color;
}
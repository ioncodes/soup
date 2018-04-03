#version 330 core

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;

out vec4 out_color;

#define SEED 0.12345679

#define TRI 128.0
#define SP 0.1
#define COLOR vec3(0.9411764705882353,0.4274509803921569,0.6549019607843137)

#define PI 3.14159265359
#define TWO_PI 6.28318530718

float rand (vec2 p) {
    return fract(sin(dot(p.xy,
                         vec2(6.8245,7.1248)))*
        9.1283);
}

float tri(vec2 uv, vec2 p, float s){
    vec2 v = uv;
    v -= p;
    v /= max(s, 0.01);

	float a = atan(v.x, v.y) + PI;
    float r = TWO_PI / 3.0;

    float t = cos(floor(0.5 + a / r) * r - a) * length(v);

    return smoothstep(0.4, 0.41, t);
}

float yPos(float i){
    vec2 p = vec2(SEED, i);

    float r = rand(p);
    return fract(time * SP + r);
}

float xPos(float i, float t){
    vec2 p = vec2(i, t - time * SP);
    return rand(p) + .375;
}

vec3 triCol(float i, float t){
    vec3 col = COLOR;
    float r = xPos(i + 1.0, t);
    col *= mix(0.9, 1.1, r);
    return col;
}

mat2 rot(float a){
    return mat2(cos(a),-sin(a),
                sin(a),cos(a));
}

float circ(vec2 uv, vec2 p, float s){
    vec2 d = uv - p;
    return 1.-smoothstep(s-(s*0.01),
                         s+(s*0.01),
                         dot(d,d)*4.0);
}

float box(vec2 uv, vec2 s){
    s = vec2(0.0) - s*0.5;
    vec2 _uv = smoothstep(s,
                        s+vec2(0.001),
                        uv);
    _uv *= smoothstep(s,
                    s+vec2(0.001),
                    vec2(0.0)-uv);
    return _uv.x*_uv.y;
}

float line(vec2 uv, vec2 p, float s){
    vec2 v = uv;
    v -= p;
    v /= max(s, 0.01);

    float t = 0.0;

    t += circ(v, vec2(0.0, 1.0), s);
    t += circ(v, vec2(0.0, -1.0), s);
    t += box(v, vec2(s * 1.8 + 0.1, 2.0)); // Randomly found out that the value I used for testing scale was unique so this is just me trying to make it seem like it works with a weird formula


    return t;//smoothstep(0.4, 0.41, t);
}

vec3 lineCol(vec2 uv){
    vec3 col = mix(
        vec3(0.453125, 0.75, 0.828125),
        vec3(0.59375, 0.81640625, 0.828125),
        sin(uv.y * 6.0 + cos(uv.x * 3.6))
    );

    return col;
}

float getLines(vec2 uv){
    float lines = 0.0;

    for (float i = 0.0; i < 64.0; i++){
        float id = i / 64.0;

        mat2 a = rot(0.7853981633974483); // PI / 4
        vec2 v = uv * a;

        float y = yPos(id);
        float x = xPos(id, y);
        float s = xPos(id, y) * 0.2;

        float l = line(v, vec2(-s) + vec2(x, y) * 2.5 - vec2(1.6, 0.0), s);
        lines += max(0.0, l);
    }

    lines = max(0.0, min(1.0, lines));
    return lines;
}

vec3 background(vec2 uv){
    vec3 col = mix(
        mix(
            vec3(0.51953125, 0.52734375, 0.5859375),
            vec3(0.21875, 0.2890625, 0.37890625),
            cos(uv.x * sin(uv.y * 4.0 + time) + uv.y * 2.0)
            ),
        vec3(0.21, 0.4, 0.5),
        sin(uv.y * 7.24 + cos(uv.x * uv.y + time) * sin(time + uv.x * 5.3)) / 2.0 + 1.0
    );

    uv.x *= (resolution.x / resolution.y);

    col = max(vec3(0.0), col - 0.2 * getLines(uv * vec2(1.04, 1.0) - vec2(0.04, 0.0)));

    float l = getLines(uv);
    if (l > 0.0)
    	col = lineCol(uv) * l;

    return col;
}

void main(){
    vec2 uv = gl_FragCoord.xy / resolution.xy;
    uv.x *= resolution.x/resolution.y;

    vec3 col = COLOR;

    // Generate all dem triangles
    for (float i = TRI; i > 0.0; i--){
        float id = i / TRI;
        float y = yPos(id);
        float x = xPos(id, y);
        float s = min(0.89, max(0.071, id * 0.5));
        float shad = tri(
            uv,
            vec2(x, mix(-s, 1.0 + s / 2.0, y)),
            s
        );

        if (shad < 0.1)
        	col = triCol(id, y) * (1.0 - shad);
    }

    // Set background mask
    float dist = distance(uv, vec2(0.5 * resolution.x / resolution.y, 0.5));
    float l = abs(dist - 0.4);
    if (dist > 0.4)
        col = background(vec2(uv.x / (resolution.x / resolution.y), uv.y));

    // Make circle logo shadow
    float dist_shad = distance(uv, vec2(0.5 * resolution.x / resolution.y, 0.49));
    float l_shad = abs(dist_shad - 0.4);
    col *= mix(0.3, 1.0, min(1.0, l_shad * 30.0));

    // Make circle logo
    col = max(col, vec3(smoothstep(0.97, 0.971, 1.0 - l)));

    out_color = vec4(col,1.0);
}

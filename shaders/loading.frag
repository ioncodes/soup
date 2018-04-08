#version 330 core

uniform float time;
uniform vec2 resolution;

out vec4 out_color;

#define RATIO	(resolution.x/resolution.y)
#define FONT_R	(16.)
#define FONT_S	(1./FONT_R)

const float M_PI = 3.1415926535897932384626433832795;

const vec4	YELOW  = vec4(vec3(0xD3, 0xE7, 0x14) / vec3(0xFF), 1.);
const vec4 	RED    = vec4(vec3(0xFF, 0x38, 0x67) / vec3(0xFF), 1.);
const vec4	BLUE   = vec4(vec3(0x46, 0x2D, 0xFE) / vec3(0xFF), 1.);
const vec4	CYAN   = vec4(vec3(0x09, 0xDE, 0xAF) / vec3(0xFF), 1.);
const vec4 	WHITE  = vec4(1.);
const vec4 	BBLACK = vec4(0., 0., 0., 1.);


vec2 rot(vec2 uv, float a)
{
    return (mat2(cos(a), -sin(a), sin(a), cos(a)) * mat2(RATIO, 0., 0., 1.)) * uv;
}

// almost signed dist sq
vec2 sdsq(vec2 uv, vec2 o, vec2 s, float a)
{
    // handle angle & ratio
    uv = rot(o - uv, a);
    return s/2.0 - abs(uv);
}


// almost signed dist sp
float sdsp(vec2 uv, vec2 o, float s)
{
    // handle angle & ratio
    uv = o - uv;
    return length(uv) - s*.5;
}

// sq color
vec4 sq(vec2 uv, vec2 o, vec2 s, vec4 c, float a)
{
    vec2 d = sdsq(uv, o, s, a);
    return mix(vec4(0.0), c, step(0.0,  min(d.x, d.y)));
}

// sp color
vec4 sp(vec2 uv, vec2 o, float s, vec4 c)
{
    float d = sdsp(uv, o, s);
    return mix(c, vec4(0.0), step(0.0, d));
}

float BezierDeg4(float p0, float p1, float p2, float p3, float t)
{
    // bezier formula following bernstein Bez(t) = E(i->n)Bern(t).Pi with t=[0,1]
    return p0*pow(1.-t, 3.) + p1*3.*t*pow(1.-t, 2.) + p2*3.*t*t*(1.-t) + p3*t*t*t;
}

float easingBezier(vec2 r, vec2 p0, vec2 p1, float t)
{
    t = BezierDeg4(0., p0.x, p1.x, 1., t);
    return BezierDeg4(r.x, p0.y, p1.y, r.y, t);
}

float easingInOutBack(float t)
{
    return easingBezier(vec2(0., 1.), vec2(0.265, -0.55), vec2(0.68, 1.55), t);
}

vec4 anim(vec2 uv, float t, vec4 c, float i)
{
    float ta = t + i *0.03 * mix(-1., 1., step(.5, t));
    ta = mix(1.-easingInOutBack(ta*2.),
            easingBezier(vec2(0., 1.), vec2(0.1, 0.75), vec2(0.67, 3.6), (ta-.5)*2.),
            step(.5, ta));
    vec2 s = vec2(ta * .25, .015);
    float e = step(1.8, ta);
    return sq(uv, vec2(s.x*.5, s.y*i*4.)/RATIO, s, mix(WHITE, c, e), .0)
        + e * sp(uv, vec2(.31, s.y*i*2.5), .015, c);
}

vec4 drawsq(vec2 uv, float t, vec4 c)
{
    vec4 ec;
    ec += anim(uv, t, c, 0.);
    ec += anim(uv, t, c, 1.);
    ec += anim(uv, t, c, 2.);
    return ec;
}

void main(void)
{
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = gl_FragCoord.xy/resolution.xy;
    float t = mod(time, 2.)/2.;

    vec4 c;
    vec2 uvfk;

    float r = easingInOutBack(t)+.125;

    const vec2 off  = vec2(.0, .030);
    const vec2 cent = vec2(.5, .55);
    uvfk = rot(uv-cent + off, r *2.* M_PI) - off;
    c += drawsq(uvfk, t, YELOW);
    uvfk = rot(uv-cent + off, (r + 0.25) *2.* M_PI) - off;
    c += drawsq(uvfk, t, CYAN);
    uvfk = rot(uv-cent + off, (r + 0.50) *2.* M_PI) - off;
    c += drawsq(uvfk, t, BLUE);
    uvfk = rot(uv-cent + off, (r + 0.75) *2.* M_PI) - off;
    c += drawsq(uvfk, t, RED);

    out_color = c;
}
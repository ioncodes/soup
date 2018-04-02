//
// Created by Luca Marcelli on 02.04.18.
//

#ifndef SOUP_RENDERER_H
#define SOUP_RENDERER_H


class Renderer {
private:
    struct {
        float r = 0;
        float g = 0;
        float b = 0;
        float a = 1;
    } m_clear_color;
    const char *VERTEX =
            "#version 330 core\n"
            "in vec2 position;\n"
            "void main()\n"
            "{\n"
            "gl_Position = vec4(position, 0.0, 1.0);\n"
            "}\n";
    const char *DEFAULT_FRAGMENT =
            "#version 330 core\n"
            "uniform float time;\n"
            "uniform vec2 resolution;\n"
            "out vec4 outColor;\n"
            "void main()\n"
            "{\n"
            "vec2 r = resolution,\n"
            "o = gl_FragCoord.xy - r/2.;\n"
            "o = vec2(max(abs(o.x)*.866 + o.y*.5, -o.y)*2. / r.y - .3, atan(o.y,o.x));\n"
            "vec4 s = .07*cos(1.5*vec4(0,1,2,3) + time + o.y + time),\n"
            "e = s.yzwx,\n"
            "f = max(o.x-s,e-o.x);\n"
            "outColor = dot(clamp(f*r.y,0.,1.), 72.*(s-e)) * (s-.1) + f;\n}";
public:
    void compile_shader(char* file);
};


#endif //SOUP_RENDERER_H

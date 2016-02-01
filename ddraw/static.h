const char* shaderprogram = \
    "uniform sampler2D texture;                                             \n" \
    "uniform sampler2D colortable;                                          \n" \
    "                                                                       \n" \
    "void main()                                                            \n" \
    "{                                                                      \n" \
    "   vec2 pos = gl_TexCoord[0].xy;                                       \n" \
    "   vec4 color = texture2D(texture, pos);                               \n" \
    "   vec2 index = vec2(color.r, 0);                                      \n" \
    "   vec4 indexedcolor = texture2D(colortable, index);                   \n" \
    "   gl_FragColor = indexedcolor;                                        \n" \
    "}                                                                      \n";

const char* dx_shaders = \
    "";
uniform sampler2D texture;
uniform sampler2D tex_y;
uniform sampler2D tex_u;
uniform sampler2D tex_v;
uniform bool isYuv;
varying mediump vec4 texc;
void main() {
    mediump vec3 yuv;
    mediump vec3 rgb;
    if (isYuv) {
        yuv.x = texture2D(tex_y, texc.st).r;
        yuv.y = texture2D(tex_u, texc.st).r - 0.5;
        yuv.z = texture2D(tex_v, texc.st).r - 0.5;

        rgb = mat3( 1,       1,         1,
                    0,       -0.39465,  2.03211,
                    1.13983, -0.58060,  0) * yuv;

        gl_FragColor = vec4(rgb, 1);
    }
    else {
        rgb  = texture2D(texture, texc.st).rgb;
        gl_FragColor = vec4(rgb, 1);
    }
}

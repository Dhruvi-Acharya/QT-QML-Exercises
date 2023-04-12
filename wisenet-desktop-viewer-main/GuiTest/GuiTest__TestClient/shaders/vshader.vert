attribute highp vec4 vertices;
attribute mediump vec4 texCoord;
varying mediump vec4 texc;
uniform highp mat4 matrix;
void main() {
    gl_Position = matrix * vertices;
    texc = texCoord;
}

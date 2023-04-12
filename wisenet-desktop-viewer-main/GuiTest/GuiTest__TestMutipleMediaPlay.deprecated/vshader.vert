attribute highp vec4 vertices;
attribute mediump vec4 texCoord;
varying mediump vec4 texc;
void main() {
    gl_Position = vertices;
    texc = texCoord;
}

varying vec2 qt_TexCoord0;
uniform highp float width;
uniform highp float height;

uniform highp float cellWidth;
uniform highp float cellHeight;

uniform highp float gridX;
uniform highp float gridY;
uniform highp float gridAlpha;

uniform sampler2D bgLogo;

void main()
{
    lowp vec2 xy = vec2(qt_TexCoord0.x*width, qt_TexCoord0.y*height);
    lowp vec2 offset = xy - vec2(gridX, gridY);
    lowp vec2 cellSize = vec2(cellWidth, cellHeight);
    lowp vec2 offsetOfCell = offset/cellSize;

    // same to mod(offset, cellSize)
    lowp vec2 modUv = offset-(cellSize*floor(offsetOfCell));

    if (int(modUv.x) == 0 || int(modUv.y) == 0) {
        // #232323
        //gl_FragColor = vec4(0.137, 0.137, 0.137, 1.0)*gridAlpha;
        // #313131
        gl_FragColor = vec4(0.192, 0.192, 0.192, 1.0)*gridAlpha;
    }
    else {
        lowp vec2 uv = fract(offsetOfCell);
        gl_FragColor = texture2D(bgLogo, uv)*gridAlpha;;
        //gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
    }
}

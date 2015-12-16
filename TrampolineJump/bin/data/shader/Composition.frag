uniform sampler2DRect u_chromaKey; //クロマキー合成した画像
uniform sampler2DRect u_background;
uniform float u_position;
uniform vec2 u_resolution;

void main() {
    //現在の座標を読み込む
    vec2 pos = gl_FragCoord.xy;
    
    //テクスチャの読み込み
    vec4 color = texture2DRect(u_chromaKey, vec2(pos.x, pos.y - u_resolution.y / 2.0));
    vec4 back = texture2DRect(u_background, vec2(pos.x, pos.y - u_position - u_resolution.y));
    
    if (pos.y - u_resolution.y / 2.0 < 0.0) {
        color = vec4(0.0);
    }
    
    if (color == vec4(0.0)) {
        color = back;
    }
    
    gl_FragColor = color;
}
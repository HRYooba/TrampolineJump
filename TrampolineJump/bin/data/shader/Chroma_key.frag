// クロマキー合成の閾値
//#define THRESH -0.05
// クロマキー合成の閾値
#define THRESH -0.08

uniform sampler2DRect u_nowImage; //現在のカメラ画像

void main() {
    //現在の座標を読み込む
    vec2 pos = gl_FragCoord.xy;
    
    //テクスチャの読み込み
    vec4 col_now = texture2DRect(u_nowImage, pos);
    
    // 値が小さいほど緑に近い
    float dist = (col_now.r + col_now.b) / 2.0 - col_now.g;
    
    // キー信号
    vec4 key = vec4(0.0);
    
    if (dist >= THRESH) {
        key = vec4(1.0);
    }
    
    gl_FragColor = key * col_now;
}
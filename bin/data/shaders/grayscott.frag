#version 150

in struct VertexOutput
{
    vec2 uv; 
} vertex_out;

uniform float screenWidth;
uniform float screenHeight;

uniform sampler2D tSource;
uniform sampler2D tInfluence;

uniform float delta;
uniform float feed;
uniform float kill;
uniform float diffU;
uniform float diffV;
uniform float brushSize;
uniform vec2 brush;

out vec4 out_color;

void main()
{
    float step_x = 1.0/screenWidth;
    float step_y = 1.0/screenHeight;
    
    vec2 uv = texture(tSource, vertex_out.uv).rg;
    vec2 uv0 = texture(tSource, vertex_out.uv+vec2(-step_x, 0.0)).rg;
    vec2 uv1 = texture(tSource, vertex_out.uv+vec2(step_x, 0.0)).rg;
    vec2 uv2 = texture(tSource, vertex_out.uv+vec2(0.0, -step_y)).rg;
    vec2 uv3 = texture(tSource, vertex_out.uv+vec2(0.0, step_y)).rg;

    vec2 lapl = (uv0 + uv1 + uv2 + uv3 - 4.0 * uv);

    float influence = texture( tInfluence, vertex_out.uv ).r;

    float F = feed + influence * 0.015;
    float K = kill + influence * 0.015;

    float u = uv.r;
    float v = uv.g;
    float uvv = u * v * v;
    
    float du = diffU * lapl.r - uvv + F * ( 1.0 - u );
    float dv = diffV * lapl.g + uvv - ( F + K ) * v;

    vec2 dst = uv + delta * vec2( du, dv );

    out_color = vec4( dst.r, dst.g, 0.0, 1.0 );
    //out_color = vec4( influence, influence, influence, 1.0 );
}


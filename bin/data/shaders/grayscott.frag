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

out vec4 out_color;

float kernel[9];
vec2 offset[9];

void main()
{
    vec2 texelSize = vec2( 1.0 / screenWidth, 1.0 / screenHeight );

    kernel[0] = 0.707106781;
    kernel[1] = 1.0;
    kernel[2] = 0.707106781;
    kernel[3] = 1.0;
    kernel[4] = -6.82842712;
    kernel[5] = 1.0;
    kernel[6] = 0.707106781;
    kernel[7] = 1.0;
    kernel[8] = 0.707106781;

    offset[0] = vec2( -1.0, -1.0 ) * texelSize;
    offset[1] = vec2(  0.0, -1.0 ) * texelSize;
    offset[2] = vec2(  1.0, -1.0 ) * texelSize;

    offset[3] = vec2( -1.0, 0.0 ) * texelSize;
    offset[4] = vec2(  0.0, 0.0 ) * texelSize;
    offset[5] = vec2(  1.0, 0.0 ) * texelSize;

    offset[6] = vec2( -1.0, 1.0 ) * texelSize;
    offset[7] = vec2(  0.0, 1.0 ) * texelSize;
    offset[8] = vec2(  1.0, 1.0 ) * texelSize;
    
    vec2 centerTexel = texture( tSource, vertex_out.uv ).rg;
    vec2 lapl = vec2( 0.0, 0.0 );
    
    for ( int i = 0; i < 9; ++i )
    {
        lapl += texture( tSource, vertex_out.uv + offset[ i ] ).rg * kernel[ i ];
    } 

    float influence = texture( tInfluence, vertex_out.uv ).r;

    float F = feed + influence * 0.015;
    float K = kill + influence * 0.015;

    float u = centerTexel.r;
    float v = centerTexel.g;
    float uvv = u * v * v;
    
    float du = diffU * lapl.r - uvv + F * ( 1.0 - u );
    float dv = diffV * lapl.g + uvv - ( F + K ) * v;

    vec2 dst = centerTexel + delta * vec2( du, dv );

    out_color = vec4( dst.r, dst.g, 0.0, 1.0 );
    //out_color = vec4( influence, influence, influence, 1.0 );
}


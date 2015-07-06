#version 150

in struct VertexOutput
{
    vec2 uv; 
} vertex_out;

uniform float screenWidth;
uniform float screenHeight;

uniform sampler2D tSource;

uniform float delta;
uniform float feed;
uniform float kill;
uniform float brushSize;
uniform vec2 brush;

out vec4 out_color;

void main()
{
    vec2 texel = vec2(1.0/screenWidth, 1.0/screenHeight);
    float step_x = 1.0/screenWidth;
    float step_y = 1.0/screenHeight;

    if (brush.x < -5.0)
    {
        out_color = vec4(1.0, 0.0, 0.0, 1.0);
        return;
    }

    //float feed = vertex_out.uv.y * 0.083;
    //float kill = vertex_out.uv.x * 0.073;

    vec2 uv = texture(tSource, vertex_out.uv).rg;
    vec2 uv0 = texture(tSource, vertex_out.uv+vec2(-step_x, 0.0)).rg;
    vec2 uv1 = texture(tSource, vertex_out.uv+vec2(step_x, 0.0)).rg;
    vec2 uv2 = texture(tSource, vertex_out.uv+vec2(0.0, -step_y)).rg;
    vec2 uv3 = texture(tSource, vertex_out.uv+vec2(0.0, step_y)).rg;

    vec2 lapl = (uv0 + uv1 + uv2 + uv3 - 4.0*uv);//10485.76;
    float du = /*0.00002*/0.2097*lapl.r - uv.r*uv.g*uv.g + feed*(1.0 - uv.r);
    float dv = /*0.00001*/0.105*lapl.g + uv.r*uv.g*uv.g - (feed+kill)*uv.g;
    vec2 dst = uv + delta*vec2(du, dv);

    if (brush.x > 0.0)
    {
        vec2 diff = (vertex_out.uv - brush)/texel;
        float dist = dot(diff, diff);

        if (dist < brushSize )
        {
            dst.g = 0.9;
        }
    }

    //out_color = vec4( vertex_out.uv.xy, 0.0, 1.0 );
    out_color = vec4( dst.r, dst.g, 0.0, 1.0 );
}


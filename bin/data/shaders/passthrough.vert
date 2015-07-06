#version 150

in vec3 position;
in vec2 texcoord;

out struct VertexOutput
{
    vec2 uv; 
} vertex_out;

void main()
{
    vertex_out.uv = texcoord;
    gl_Position = vec4( position, 1.0 );
}

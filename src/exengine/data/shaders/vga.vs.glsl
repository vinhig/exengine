#version 330 core

layout (location = 0) in vec2 in_position;
layout (location = 1) in vec2 in_uv;

out vec2 uv;

uniform mat4 u_projection;

void main() 
{
  gl_Position = u_projection * vec4(in_position.xy, 0.0f, 1.0f);
  uv = in_uv;
}

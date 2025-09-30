#version 330 core

in vec2 uv;

out vec4 color;

uniform sampler2D u_texture;

void main()
{
  vec4 sample = texture(u_texture, vec2(uv.x, uv.y)).rgba;
  color = sample;
}

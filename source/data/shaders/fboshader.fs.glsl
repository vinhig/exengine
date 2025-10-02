#version 330 core

in vec2 uv;

out vec4 color;

uniform sampler2D u_texture;
uniform sampler2D u_ssao;

vec3 aces_tonemap(vec3 x) {
  float a = 2.51;
  float b = 0.03;
  float c = 2.43;
  float d = 0.59;
  float e = 0.14;
  return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.0, 1.0);
}

const vec3 u_white_point = vec3(0.75, 0.75, 0.75);

float ambient_occlusion(vec2 uv)
{
    vec2 texelSize = 1.0 / textureSize(u_ssao, 0);
    float result = 0.0;
    float totalWeight = 0.0;

    float kernel[25] = float[](
        1.0,  4.0,  6.0,  4.0, 1.0,
        4.0, 16.0, 24.0, 16.0, 4.0,
        6.0, 24.0, 36.0, 24.0, 6.0,
        4.0, 16.0, 24.0, 16.0, 4.0,
        1.0,  4.0,  6.0,  4.0, 1.0
    );

    for (int y = -2; y <= 2; y++)
    {
      for (int x = -2; x <= 2; x++)
      {
        vec2 sampleUV = uv + vec2(float(x), float(y)) * texelSize;

        if (sampleUV.x >= 0.0 && sampleUV.x <= 1.0 &&
           sampleUV.y >= 0.0 && sampleUV.y <= 1.0)
        {
           int kernelIndex = (y + 2) * 5 + (x + 2);
           float weight = kernel[kernelIndex];
           float sample = texture(u_ssao, sampleUV).r;
           result += sample * weight;
           totalWeight += weight;
        }
      }
    }

    return result / totalWeight;
}

void main()
{
  bool ssao_enabled = true;
  vec3 tex_color = textureLod(u_texture, uv, 0.0).rgb;
  // if (ssao_enabled)
  // {
  //   tex_color *= ambient_occlusion(uv);
  // }
  tex_color = vec3(1.0) - exp(tex_color / u_white_point);
  color = vec4(aces_tonemap(tex_color) * ambient_occlusion(uv), 1.0);
}

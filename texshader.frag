#version 410 core

in vec4 color;
in vec2 UV;

out vec4 fragColor;
uniform sampler2D textureSampler;

void main()
{
    fragColor = texture(textureSampler, UV);
}

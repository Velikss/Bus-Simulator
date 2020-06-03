#version 450 core

layout (location = 0) in vec2 inUV;
layout (location = 1) in vec4 text;

layout (set = 1, binding = 0) uniform sampler2D samplerSprite;

layout (location = 0) out vec4 outFragColor;

void main(void)
{
    // If we're not rendering text, just sample the sprite
    if (text.a == 0)
    {
        outFragColor = vec4(texture(samplerSprite, inUV).rgb, 1);
    }
    else
    {
        // For text, the sprite contains the characters on the red channel
        float value = texture(samplerSprite, inUV).r;

        // If the value is 0, discard this fragment
        if (value == 0)
        {
            discard;
        }
        else
        {
            // Otherwise, use the value as alpha in the final color, and mix it with the text color
            outFragColor = vec4(text.rgb, value);
        }
    }
}

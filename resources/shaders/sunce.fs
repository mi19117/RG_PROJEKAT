#version 330 core

out vec4 FragColor;
in vec2 koordTeksture;

uniform sampler2D tekstura;

void main()
{
    FragColor = texture(tekstura, koordTeksture);
//     FragColor=vec4(1.0f, 1.0f, 0.0f, 1.0f);
}
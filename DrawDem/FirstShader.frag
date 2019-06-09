#version 330 core
out vec4 FragColor;
in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

uniform int type;

void main()
{

	if(type <= 1)
		FragColor = vec4(ourColor,0.5f);
	else if(type == 2)
		FragColor = texture(ourTexture,TexCoord);
	else if(type == 3)
		FragColor = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0);
} 
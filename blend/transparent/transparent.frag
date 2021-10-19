#version 330 core

in vec2 TextCoord;
uniform sampler2D text;


out vec4 color;


void main()
{
	vec4 textColor = texture(text, TextCoord);
	if(textColor.a < 0.1)  // ����alphaֵ ����ƬԪ < 0.1��ʾ�ӽ���ȫ͸��
		discard;
	color = textColor;
}
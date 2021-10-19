#version 330
layout (location = 0) out vec4 FragColor;  // ָ���������һ��color buffer
layout (location = 1) out vec4 BrightColor;// ָ��������ڶ���color buffer

// ��������interface block
in VS_OUT
{
	in vec3 FragPos;
	in vec2 TextCoord;
	in vec3 FragNormal;
}fs_in;

uniform vec3 lightColor;

//out vec4 FragColor;

void main()
{   
	FragColor = vec4(lightColor, 1.0f); // ����ģ��Ĺ�Դֻ�����Դ������ɫ
	// ���ݸ���������ֵ ���������������Ӧ��color buffer
	float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > 1.0f)
		BrightColor = vec4(FragColor.rgb, 1.0f);
}
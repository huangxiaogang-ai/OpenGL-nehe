#version 330

layout (location = 0) out vec3 gPosition;   // ָ���������һ��color buffer
layout (location = 1) out vec3 gNormal; // ָ��������ڶ���color buffer
layout (location = 2) out vec4 gAlbedoAndSpec; // ָ�������������color buffer

// ��������interface block
in VS_OUT
{
	in vec3 FragPos;
	in vec2 TextCoord;
	in vec3 FragNormal;
}fs_in;

uniform sampler2D texture_diffuse0;
uniform sampler2D texture_specular0;

void main()
{
	// ���ƬԪλ��
	gPosition = fs_in.FragPos;
	// ���ƬԪ�ķ�������Ϣ
	gNormal = normalize(fs_in.FragNormal);
	// ���ƬԪ�����������ɫ
	gAlbedoAndSpec.rgb = texture(texture_diffuse0, fs_in.TextCoord).rgb;
	// ��������ǿ�� �洢Ϊalpha�ɷ�
	gAlbedoAndSpec.a = texture(texture_specular0, fs_in.TextCoord).r;
}
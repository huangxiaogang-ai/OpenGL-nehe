#version 330 core

in vec4 FragPos;

uniform vec3 lightPos;
uniform float far_plane;

void main()
{
	// ��ʹ��OpenGL�Զ��������� ����������ϵ�м������
	// gl_FragDepth = gl_FragCoord.z; // ��ʹ��Ĭ�Ϸ�ʽ

	 // ����ƬԪ�͹�Դλ��
    float lightDistance = length(FragPos.xyz - lightPos);
    
    // ������[0,1]��Χ
    lightDistance = lightDistance / far_plane;
    
    // ��Ϊ������
    gl_FragDepth = lightDistance;
}
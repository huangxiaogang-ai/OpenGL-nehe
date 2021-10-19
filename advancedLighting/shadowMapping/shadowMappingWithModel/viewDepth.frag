#version 330 core

in vec2 TextCoord;
uniform sampler2D depthMap;
uniform float nearPlane;
uniform float farPlane;

out vec4 color;

float LinearizeDepth(float depthVal) 
{
    float Zndc = depthVal * 2.0 - 1.0; // ����ndc���� ����Ĭ��glDepthRange(0,1)
    float Zeye = (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - Zndc * (farPlane - nearPlane)); // �����ĸ�����˷�ת
	return (Zeye - nearPlane)/ ( farPlane - nearPlane);
}

void main()
{
	float depthVal = texture(depthMap, TextCoord).r;
	//depthVal = LinearizeDepth(depthVal);
	color = vec4(vec3(depthVal), 1.0);
}
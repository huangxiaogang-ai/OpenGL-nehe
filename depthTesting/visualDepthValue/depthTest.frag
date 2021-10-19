#version 330 core

in vec2 TextCoord;
uniform sampler2D text;


out vec4 color;

// ԭ�����
float asDepth()
{
  return gl_FragCoord.z;
}
// ����������
float near = 1.0f; 
float far  = 100.0f; 
float LinearizeDepth() 
{
    float Zndc = gl_FragCoord.z * 2.0 - 1.0; // ����ndc���� ����Ĭ��glDepthRange(0,1)
    float Zeye = (2.0 * near * far) / (far + near - Zndc * (far - near)); // �����ĸ�����˷�ת
	return (Zeye - near)/ ( far - near);
}
// ���������
float nonLinearDepth()
{
	float Zndc = gl_FragCoord.z * 2.0 - 1.0; // ����ndc���� ����Ĭ��glDepthRange(0,1)
    float Zeye = (2.0 * near * far) / (far + near - Zndc * (far - near)); // �����ĸ�����˷�ת
	return (1.0 / near - 1.0 / Zeye) / (1.0 / near - 1.0 / far);
}
void main()
{
	float depth = LinearizeDepth();
	color = vec4(vec3(depth), 1.0f);
}
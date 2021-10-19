#version 330 core

layout (triangles) in;  // ����������
layout (triangle_strip, max_vertices=18) out;

uniform mat4 shadowMatrices[6];
out vec4 FragPos; // �����������ϵλ��

void main()
{
    // һ��6��cubemap��
	for(int face = 0; face < 6;++face)
	{
		gl_Layer = face;  // ���õ�ǰ�������

		// Ϊÿ������������ζ��� ������Դ��Ӧ��6���������ϵ��λ��
		for(int i=0; i < 3;++i)
		{
			FragPos = gl_in[i].gl_Position;
			gl_Position = shadowMatrices[face] * FragPos; // ����ÿ�������ڹ�ԴΪ�ӵ�Ĳü�����ϵ��λ��
			EmitVertex();
		}
		EndPrimitive();
	}
}
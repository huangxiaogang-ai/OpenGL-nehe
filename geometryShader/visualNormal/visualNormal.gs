#version 330 core

layout(triangles) in ;  // ����������
layout(line_strip, max_vertices = 6) out;  // ���3������������ֱ��

// ��������interface block
in VS_OUT
{
   vec3 normal;
}gs_in[];

float magnitude = 0.1f;

// Ϊָ�������Ķ����������������ֱ��
void generateNormalLine(int index)
{
  gl_Position = gl_in[index].gl_Position;
  EmitVertex();
  vec4 offset = vec4(gs_in[index].normal * magnitude, 0.0f);
  gl_Position = gl_in[index].gl_Position + offset;
  EmitVertex();
  EndPrimitive();
}

// �������������ֱ��
void main()
{
	generateNormalLine(0);
	generateNormalLine(1);
	generateNormalLine(2);
}
#version 330 core

layout(points) in ;
layout(triangle_strip, max_vertices = 5) out; // ע���������

void makeHouse(vec4 position)
{
    gl_Position = position + vec4(-0.2f, -0.2f, 0.0f, 0.0f);  // ���½�
	EmitVertex();
	gl_Position = position + vec4(0.2f, -0.2f, 0.0f, 0.0f);  // ���½�
	EmitVertex();
	gl_Position = position + vec4(-0.2f, 0.2f, 0.0f, 0.0f);  // ���Ͻ�
	EmitVertex();
	gl_Position = position + vec4(0.2f, 0.2f, 0.0f, 0.0f);  // ���Ͻ�
	EmitVertex();
	gl_Position = position + vec4(0.0f, 0.4f, 0.0f, 0.0f);  // ����
	EmitVertex();
	EndPrimitive();
}
// ���������ʽ�����δ�
void main()
{
    gl_PointSize = gl_in[0].gl_PointSize;
	makeHouse(gl_in[0].gl_Position);
}
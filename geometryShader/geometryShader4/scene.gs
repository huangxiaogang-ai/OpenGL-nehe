#version 330 core

layout(points) in ;
layout(triangle_strip, max_vertices = 5) out; // ע���������

// ��������interface block
in VS_OUT
{
   vec3 vertColor;
}gs_in[];

out vec3 fcolor;

void makeHouse(vec4 position)
{
    fcolor = gs_in[0].vertColor;
	gl_PointSize = gl_in[0].gl_PointSize;
    gl_Position = position + vec4(-0.2f, -0.2f, 0.0f, 0.0f);  // ���½�
	EmitVertex();
	gl_Position = position + vec4(0.2f, -0.2f, 0.0f, 0.0f);  // ���½�
	EmitVertex();
	gl_Position = position + vec4(-0.2f, 0.2f, 0.0f, 0.0f);  // ���Ͻ�
	EmitVertex();
	gl_Position = position + vec4(0.2f, 0.2f, 0.0f, 0.0f);  // ���Ͻ�
	EmitVertex();
	gl_Position = position + vec4(0.0f, 0.4f, 0.0f, 0.0f);  // ����
	fcolor = vec3(1.0f, 1.0f, 1.0f); // ����ı䶥����ɫ
	EmitVertex();
	EndPrimitive();
}
// ���������ʽ�����δ�
void main()
{
	makeHouse(gl_in[0].gl_Position);
}
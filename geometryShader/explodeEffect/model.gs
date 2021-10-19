#version 330 core

layout(triangles) in ;  // ����������
layout(triangle_strip, max_vertices = 3) out;

// ��������interface block
in VS_OUT
{
   vec2 TextCoord;
}gs_in[];

out vec2 TextCoord;
uniform float time; // ��ʱ�����explodeЧ��

// �������3������ ���㷨����
vec3 getNormal(vec4 pos0, vec4 pos1, vec4 pos2)
{
  vec3 a = vec3(pos0) - vec3(pos1);
  vec3 b = vec3(pos2) - vec3(pos1);
  return normalize(cross(a, b));
}

// ����ƫ�ƺ�������ζ���
void explode()
{
  vec3 normal = getNormal(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position);
  float magnitude = ((sin(time) + 1) / 2.0f) * 2.0f; // ʹλ��ƫ��������[0, 2.0f]��Χ��
  vec4 offset = vec4(normal * magnitude, 0.0f);
  gl_Position = gl_in[0].gl_Position + offset;
  TextCoord = gs_in[0].TextCoord; // �������������ÿ�����㶼����ͬ
  EmitVertex();
  gl_Position = gl_in[1].gl_Position + offset;
  TextCoord = gs_in[1].TextCoord;
  EmitVertex();
  gl_Position = gl_in[2].gl_Position + offset;
  TextCoord = gs_in[2].TextCoord;
  EmitVertex();
  EndPrimitive();
}

// ����ڷ�����������ƫ�ƵĶ���λ�� ģ�ⱬըЧ��
void main()
{
	explode();
}
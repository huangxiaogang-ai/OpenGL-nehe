#version 330

// ��������interface block
in VS_OUT
{
	in vec3 FragPos;
	in vec2 TextCoord;
	in vec3 FragNormal;
}fs_in;

// ��Դ���Խṹ��
struct LightAttr
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform LightAttr light;
uniform vec3 viewPos;
uniform bool normalMapping;
uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

out vec4 color;

void main()
{   
    vec3 objectColor = texture(diffuseMap,fs_in.TextCoord).rgb;
	// ������ɷ�
	float	ambientStrength = 0.1f;
	vec3	ambient = ambientStrength * light.ambient;

	// �������ɷ� ��ʱ��Ҫ���߷���Ϊָ���Դ
	vec3	lightDir = normalize(light.position - fs_in.FragPos);
	vec3	normal = normalize(fs_in.FragNormal);
	if(normalMapping)  // ʹ��normal mappingʱ ��normal mapȡ��normal vector
	{
		normal = texture(normalMap, fs_in.TextCoord).rgb;
        normal = normalize(normal * 2.0 - 1.0); // ����ɫ��Χ[0,1]��ԭ��������Χ[-1,1]
	}
	float	diffFactor = max(dot(lightDir, normal), 0.0);
	vec3	diffuse = diffFactor * light.diffuse;

	vec3	viewDir = normalize(viewPos - fs_in.FragPos);
	float	specFactor = 0.0;
	vec3 halfDir = normalize(lightDir + viewDir);
	specFactor = pow(max(dot(halfDir, normal), 0.0), 32.0); // 32.0Ϊ����߹�ϵ��
	vec3	specular = specFactor * light.specular;

	vec3	result = (ambient + diffuse + specular ) * objectColor;
	color	= vec4(result , 1.0f);
}
#version 330

// ��������interface block
in VS_OUT
{
	in vec3 FragPos;
	in vec2 TextCoord;
	in vec3 FragNormal;
}fs_in;

// ���Դ���Խṹ��
struct PointLightAttr
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;	// ˥������
	float linear;   // ˥��һ��ϵ��
	float quadratic; // ˥������ϵ��
};

#define LIGHT_NUM 4
uniform PointLightAttr lights[LIGHT_NUM];
uniform vec3 viewPos;
uniform sampler2D text;
uniform bool bGamma;

out vec4 color;

vec3 calcPointLight(PointLightAttr light)
{
	// ������ɷ�
	vec3	ambient = light.ambient * vec3(texture(text, fs_in.TextCoord));

	// �������ɷ� ��ʱ��Ҫ���߷���Ϊָ���Դ
	vec3	lightDir = normalize(light.position - fs_in.FragPos);
	vec3	normal = normalize(fs_in.FragNormal);
	float	diffFactor = max(dot(lightDir, normal), 0.0);
	vec3	diffuse = diffFactor * light.diffuse * vec3(texture(text, fs_in.TextCoord));

	// ���淴��ɷ� ����Blinn-Phongģ��
	float	specularStrength = 0.5f;
	vec3	viewDir = normalize(viewPos - fs_in.FragPos);
	vec3    halfDir = normalize(lightDir + viewDir);
	float	specFactor = pow(max(dot(halfDir, normal), 0.0), 64.0f);
	vec3	specular = specFactor * light.specular * vec3(texture(text, fs_in.TextCoord));

	// ����˥������
	float distance = length(light.position - fs_in.FragPos); // ����������ϵ�м������
	// ������GammaУ��ʱ һ�κ���Ч���� ����ʱ���κ���Ч����
	float attenuation = 1.0f / ( bGamma ? distance * distance : distance);

	vec3	result = (ambient + diffuse + specular) * attenuation;
	return  result;
}

void main()
{
	vec3 result = vec3(0.0);
	for(int i = 0; i < LIGHT_NUM;++i)
	   result += calcPointLight(lights[i]);
	if(bGamma)
	{
		result = pow(result, vec3(1.0 / 2.2));	// ����GammaУ��
	}
	color	= vec4(result , 1.0f);
}
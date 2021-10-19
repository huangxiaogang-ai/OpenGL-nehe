#version 330 core

in vec3 FragPos;
in vec2 TextCoord;
in vec3 FragNormal;

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

#define LIGHT_NUM 2
uniform PointLightAttr lights[LIGHT_NUM];
uniform vec3 viewPos;
uniform sampler2D texture_diffuse0;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular0;
uniform sampler2D texture_specular1;
uniform bool bGamma;

out vec4 color;

vec3 calcPointLight(PointLightAttr light)
{
	// ������ɷ�
	vec3	ambient = light.ambient * vec3(texture(texture_diffuse0, TextCoord));

	// �������ɷ� ��ʱ��Ҫ���߷���Ϊָ���Դ
	vec3	lightDir = normalize(light.position - FragPos);
	vec3	normal = normalize(FragNormal);
	float	diffFactor = max(dot(lightDir, normal), 0.0);
	vec3	diffuse = diffFactor * light.diffuse * vec3(texture(texture_diffuse0, TextCoord));

	// ���淴��ɷ� ����Blinn-Phongģ��
	float	specularStrength = 0.5f;
	vec3	viewDir = normalize(viewPos - FragPos);
	vec3    halfDir = normalize(lightDir + viewDir);
	float	specFactor = pow(max(dot(halfDir, normal), 0.0), 64.0f);
	vec3	specular = specFactor * light.specular * vec3(texture(texture_specular0, TextCoord));

	// ����˥������
	float distance = length(light.position - FragPos); // ����������ϵ�м������
	float attenuation = 1.0f / (light.constant 
			+ light.linear * distance
			+ light.quadratic * distance * distance);

	vec3	result = (ambient + diffuse + specular) * attenuation;
	return result;
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
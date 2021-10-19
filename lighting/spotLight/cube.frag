#version 330

in vec3 FragPos;
in vec2 TextCoord;
in vec3 FragNormal;

out vec4 color;

// �������Խṹ��
struct MaterialAttr
{
	sampler2D diffuseMap;	// ʹ������������ƬԪλ��ȡ��ͬ�Ĳ�������
	sampler2D specularMap;
	float shininess; //����߹�ϵ��
};
// ��Դ���Խṹ��
struct LightAttr
{
	vec3 position;	// �۹�Ƶ�λ��
	vec3 direction; // �۹�Ƶ�spot direction
	float cutoff;	// �۹���Žǵ�cosֵ

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;	// ˥������
	float linear;   // ˥��һ��ϵ��
	float quadratic; // ˥������ϵ��
};

uniform MaterialAttr material;
uniform LightAttr light;
uniform vec3 viewPos;

void main()
{   
	// ������ɷ�
	vec3	ambient = light.ambient * vec3(texture(material.diffuseMap, TextCoord));
	vec3	lightDir = normalize(light.position - FragPos);
	float theta = dot(lightDir, normalize(-light.direction));// ������۹��spotDir�н�����ֵ
	
	if(theta > light.cutoff)	// �ھ۹���ŽǷ�Χ��
	{
		// �������ɷ� ��ʱ��Ҫ���߷���Ϊָ���Դ
		vec3	normal = normalize(FragNormal);
		float	diffFactor = max(dot(lightDir, normal), 0.0);
		vec3	diffuse = diffFactor * light.diffuse * vec3(texture(material.diffuseMap, TextCoord));

		// ���淴��ɷ� ��ʱ��Ҫ���߷���Ϊ�ɹ�Դָ��
		float	specularStrength = 0.5f;
		vec3	reflectDir = normalize(reflect(-lightDir, normal));
		vec3	viewDir = normalize(viewPos - FragPos);
		float	specFactor = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
		vec3	specular = specFactor * light.specular * vec3(texture(material.specularMap, TextCoord));

		// ����˥������
		float distance = length(light.position - FragPos); // ����������ϵ�м������
		float attenuation = 1.0f / (light.constant 
				+ light.linear * distance
				+ light.quadratic * distance * distance);
		vec3	result = (ambient + diffuse + specular) * attenuation;
		color	= vec4(result , 1.0f);
	}
	else
	{
		vec3 result = ambient;	// �����ŽǷ�Χ��ʱֻ�л�����ɷ�
		color	= vec4(result , 1.0f);
	}
}
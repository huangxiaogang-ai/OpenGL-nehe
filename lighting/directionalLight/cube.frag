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
	vec3 direction;	// �����Դ
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform MaterialAttr material;
uniform LightAttr light;
uniform vec3 viewPos;

void main()
{   
	// ������ɷ�
	vec3	ambient = light.ambient * vec3(texture(material.diffuseMap, TextCoord));

	// �������ɷ� ��ʱ��Ҫ���߷���Ϊָ���Դ
	//vec3	lightDir = normalize(light.position - FragPos);

	vec3	lightDir = normalize(-light.direction);	// ��ת�����Դ�ķ���
	vec3	normal = normalize(FragNormal);
	float	diffFactor = max(dot(lightDir, normal), 0.0);
	vec3	diffuse = diffFactor * light.diffuse * vec3(texture(material.diffuseMap, TextCoord));

	// ���淴��ɷ� ��ʱ��Ҫ���߷���Ϊ�ɹ�Դָ��
	float	specularStrength = 0.5f;
	vec3	reflectDir = normalize(reflect(-lightDir, normal));
	vec3	viewDir = normalize(viewPos - FragPos);
	float	specFactor = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
	vec3	specular = specFactor * light.specular * vec3(texture(material.specularMap, TextCoord));

	vec3	result = ambient + diffuse + specular;
	color	= vec4(result , 1.0f);
}
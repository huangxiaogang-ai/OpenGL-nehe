#version 330

// ��������interface block
in VS_OUT
{
	vec3 FragPos;
	vec2 TextCoord;
	vec3 FragNormal;
}fs_in;


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
	vec3 position;
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
	vec3	ambient = light.ambient * vec3(texture(material.diffuseMap, fs_in.TextCoord));

	// �������ɷ� ��ʱ��Ҫ���߷���Ϊָ���Դ
	vec3	lightDir = normalize(light.position - fs_in.FragPos);
	vec3	normal = normalize(fs_in.FragNormal);
	float	diffFactor = max(dot(lightDir, normal), 0.0);
	vec3	diffuse = diffFactor * light.diffuse * vec3(texture(material.diffuseMap, fs_in.TextCoord));

	// ���淴��ɷ� ��ʱ��Ҫ���߷���Ϊ�ɹ�Դָ��
	float	specularStrength = 0.5f;
	vec3	reflectDir = normalize(reflect(-lightDir, normal));
	vec3	viewDir = normalize(viewPos - fs_in.FragPos);
	float	specFactor = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
	vec3	specular = specFactor * light.specular * vec3(texture(material.specularMap, fs_in.TextCoord));

	// ����˥������
	float distance = length(light.position - fs_in.FragPos); // ����������ϵ�м������
	float attenuation = 1.0f / (light.constant 
			+ light.linear * distance
			+ light.quadratic * distance * distance);

	vec3	result = (ambient + diffuse + specular) * attenuation;
	color	= vec4(result , 1.0f);
}
#version 330

in vec3 FragPos;
in vec2 TextCoord;
in vec3 FragNormal;
in vec3 LightPos;

out vec4 color;


uniform vec3 lightColor;
uniform vec3 objectColor;

void main()
{   
	// ������ɷ�
	float	ambientStrength = 0.1f;
	vec3	ambient = ambientStrength * lightColor;

	// �������ɷ� ��ʱ��Ҫ���߷���Ϊָ���Դ
	vec3	lightDir = normalize(LightPos - FragPos);
	vec3	normal = normalize(FragNormal);
	float	diffFactor = max(dot(lightDir, normal), 0.0);
	vec3	diffuse = diffFactor * lightColor;

	// ���淴��ɷ� ��ʱ��Ҫ���߷���Ϊ�ɹ�Դָ��
	float	specularStrength = 0.5f;
	vec3	reflectDir = normalize(reflect(-lightDir, normal));
	vec3	viewDir = normalize(-FragPos); // �������ϵ��viewPosλ��ԭ�� viewPos - FragPos = - FragPos 
	float	specFactor = pow(max(dot(reflectDir, viewDir), 0.0), 32); // 32Ϊ����߹�ϵ��
	vec3	specular = specularStrength * specFactor * lightColor;

	vec3	result = (ambient + diffuse + specular ) * objectColor;
	color	= vec4(result , 1.0f);
}
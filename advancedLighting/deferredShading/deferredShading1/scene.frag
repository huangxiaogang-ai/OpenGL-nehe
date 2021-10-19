#version 330

in vec2 TextCoord;

out vec4 FragColor;

// ��Դ���Խṹ��
struct LightAttr
{
	vec3 position;
	vec3 diffuse;
	vec3 specular;
	float linear;   // һ��˥��ϵ��
	float quadratic; // ����˥��ϵ��
};
#define NR_LIGHTS 32
uniform LightAttr lights[NR_LIGHTS];
uniform vec3 viewPos;

uniform sampler2D gPositonText;
uniform sampler2D gNormalText;
uniform sampler2D gAlbedoSpecText;

uniform int drawMode;


void main()
{
	// ��GBuffer�и�������ȡ��ƬԪ����Ϣ
	vec3 FragPos = texture(gPositonText, TextCoord).rgb;
	vec3 FragNormal = texture(gNormalText, TextCoord).rgb;
	vec3 DiffuseColor = texture(gAlbedoSpecText, TextCoord).rgb;
	float SpecStrength = texture(gAlbedoSpecText,TextCoord).a;
	
	// ������ɷ�
	vec3	ambientColor = DiffuseColor;
	float	ambientStrength = 0.1f;
	vec3	ambient = ambientStrength * ambientColor;
	vec3	lightSum = ambient;
	vec3	viewDir = normalize(viewPos - FragPos);// �ᵽѭ������ �����ظ�����
	for(int i = 0 ; i < NR_LIGHTS; ++i)
	{
		// �������ɷ� ��ʱ��Ҫ���߷���Ϊָ���Դ
		vec3	lightDir = normalize(lights[i].position - FragPos);
		vec3	normal = normalize(FragNormal);
		float	diffFactor = max(dot(lightDir, normal), 0.0);
		vec3	diffuse = diffFactor * lights[i].diffuse * DiffuseColor;

		// �����ɷ� Bling-Phongģ��
		vec3	halfDir = normalize(lightDir + viewDir);
		float	specFactor = pow(max(dot(halfDir, normal), 0.0), 32.0); // 32.0Ϊ����߹�ϵ��
		vec3	specular = SpecStrength * specFactor * lights[i].specular;
		
		// ˥������
		float distance = length(FragPos - lights[i].position);
		float attenuation = 1.0 / (1.0 + lights[i].linear * distance + lights[i].quadratic * distance * distance);
		lightSum += (diffuse + specular) * attenuation;
	}
	if (drawMode == 1) 
		FragColor = vec4(lightSum, 1.0);
	else if(drawMode == 2)
	    FragColor = vec4(FragPos, 1.0);
	else if(drawMode == 3)
		FragColor = vec4(FragNormal, 1.0);
	else if(drawMode == 4)
		FragColor = vec4(DiffuseColor,1.0);
	else
	   FragColor = vec4(vec3(SpecStrength), 1.0);
}
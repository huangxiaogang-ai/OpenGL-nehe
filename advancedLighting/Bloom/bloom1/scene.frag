#version 330
layout (location = 0) out vec4 FragColor;  // ָ���������һ��color buffer
layout (location = 1) out vec4 BrightColor;// ָ��������ڶ���color buffer

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
	vec3 diffuse;
	vec3 specular;
};
uniform LightAttr lights[4];
uniform vec3 viewPos;
uniform sampler2D text;

//out vec4 FragColor;

void main()
{   
    vec3 objectColor = texture(text,fs_in.TextCoord).rgb;
	// ������ɷ�
	vec3	ambitentStrength = vec3(0.1,0.1,0.1);
	float	ambientStrength = 0.1f;
	vec3	ambient = ambientStrength * ambitentStrength;
	vec3	lightSum = vec3(0.0f);
	for(int i = 0 ; i < 4; ++i)
	{
		
		// �������ɷ� ��ʱ��Ҫ���߷���Ϊָ���Դ
		vec3	lightDir = normalize(lights[i].position - fs_in.FragPos);
		vec3	normal = normalize(fs_in.FragNormal);
		float	diffFactor = max(dot(lightDir, normal), 0.0);
		vec3	diffuse = diffFactor * lights[i].diffuse;

		// �����ɷ� Bling-Phongģ��
		vec3	viewDir = normalize(viewPos - fs_in.FragPos);
		vec3	halfDir = normalize(lightDir + viewDir);
		float	specFactor = pow(max(dot(halfDir, normal), 0.0), 32.0); // 32.0Ϊ����߹�ϵ��
		vec3	specular = specFactor * lights[i].specular;
		
		// ˥������
		float distance = length(fs_in.FragPos - lights[i].position);
		float attenuation = 1.0 / (distance * distance);
		lightSum += (diffuse + specular) * attenuation;
	}
	vec3	result = (ambient + lightSum) * objectColor;
	FragColor = vec4(objectColor , 1.0f);
	// ���ݸ���������ֵ ���������������Ӧ��color buffer
	float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > 1.0f)
		BrightColor = vec4(result, 1.0f);
}
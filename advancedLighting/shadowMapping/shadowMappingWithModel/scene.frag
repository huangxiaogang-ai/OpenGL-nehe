#version 330 core

// ��������interface block
in VS_OUT
{
	vec3 FragPos;
	vec2 TextCoord;
	vec3 FragNormal;
	vec4 FragPosInLightSpace;
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
uniform sampler2D depthMap;
uniform bool bUseShadow;
uniform bool bUsePCF;
// �޸���������Ӧģ�͵Ļ���
uniform sampler2D texture_diffuse0; 
uniform sampler2D texture_specular0;

out vec4 color;

// ͨ��depth-map���ڹ�Դ����ϵ��λ�� ������Ӱ
float calculateShadow1(vec4 fragPosInLightSpace)
{
	vec3 ndcPos = fragPosInLightSpace.xyz / fragPosInLightSpace.w;  // �Ӳü�����ϵת����NDC
	vec3 winPos = ndcPos * 0.5 + 0.5; // ��NDC�任����Ļ����ϵ ʵ������������������ĵ���
	float closestDepth = texture(depthMap, winPos.xy).r; // ʹ��xy���������������
	float currentDepth = winPos.z;
	return currentDepth > closestDepth ? 1.0 : 0.0;
}
// ʹ��ƫ�����޸�shadow-acne
float calculateShadow2(vec4 fragPosInLightSpace, vec3 normal, vec3 lightDir)
{
	vec3 ndcPos = fragPosInLightSpace.xyz / fragPosInLightSpace.w;  // �Ӳü�����ϵת����NDC
	vec3 winPos = ndcPos * 0.5 + 0.5; // ��NDC�任����Ļ����ϵ ʵ������������������ĵ���
	float closestDepth = texture(depthMap, winPos.xy).r; // ʹ��xy���������������
	float currentDepth = winPos.z;
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005f); // ʹ��dot�������ƫ����
	return currentDepth - bias > closestDepth ? 1.0 : 0.0;
}

// ʹ��ƫ�����޸�shadow-acne ͬʱ������light frustum�Ĳ���ȥ����Ӱ
float calculateShadow3(vec4 fragPosInLightSpace, vec3 normal, vec3 lightDir)
{
	vec3 ndcPos = fragPosInLightSpace.xyz / fragPosInLightSpace.w;  // �Ӳü�����ϵת����NDC
	vec3 winPos = ndcPos * 0.5 + 0.5; // ��NDC�任����Ļ����ϵ ʵ������������������ĵ���
	float closestDepth = texture(depthMap, winPos.xy).r; // ʹ��xy���������������
	float currentDepth = winPos.z;
	if(currentDepth > 1.0)  // �ⲿ����far plane���� ����Ҫ��Ӱ
	{
	   return 0.0f;
	}

	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005f); // ʹ��dot�������ƫ����
	return currentDepth - bias > closestDepth ? 1.0 : 0.0;
}
// ʹ��ƫ�����޸�shadow-acne ͬʱ������light frustum�Ĳ���ȥ����Ӱ
// ͬʱʹ���� percentage-closer filtering ��������Ӱ
float calculateShadow4(vec4 fragPosInLightSpace, vec3 normal, vec3 lightDir)
{
	vec3 ndcPos = fragPosInLightSpace.xyz / fragPosInLightSpace.w;  // �Ӳü�����ϵת����NDC
	vec3 winPos = ndcPos * 0.5 + 0.5; // ��NDC�任����Ļ����ϵ ʵ������������������ĵ���
	float currentDepth = winPos.z;
	if(currentDepth > 1.0)  // �ⲿ����far plane���� ����Ҫ��Ӱ
	{
	   return 0.0f;
	}

	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005f); // ʹ��dot�������ƫ����
	vec2 texelSize = 1.0 / textureSize(depthMap,0);
	float shadow = 0.0f;
	for(int x = -1; x <= 1;++x)
	{
		for(int y = -1; y <= 1;++y)
		{
		    float pcfDepth = texture(depthMap, winPos.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow = shadow / 9.0f;
	return shadow;
}

void main()
{   
	// ������ɷ�
	vec3	ambient = light.ambient * vec3(texture(texture_diffuse0, fs_in.TextCoord));

	// �������ɷ� ��ʱ��Ҫ���߷���Ϊָ���Դ
	vec3	lightDir = normalize(light.position - fs_in.FragPos);
	vec3	normal = normalize(fs_in.FragNormal);
	float	diffFactor = max(dot(lightDir, normal), 0.0);
	vec3	diffuse = diffFactor * light.diffuse * vec3(texture(texture_diffuse0, fs_in.TextCoord));

	// �����ɷ� ʹ��Blinn-Phongģ��
	vec3	viewDir = normalize(viewPos - fs_in.FragPos);
	vec3	halfDir = normalize(lightDir + viewDir);
	float	specFactor = pow(max(dot(halfDir, normal), 0.0), 32.0); // 32.0Ϊ����߹�ϵ��
	vec3	specular = specFactor * light.specular * vec3(texture(texture_specular0, fs_in.TextCoord));
	float shadow = 0.0f;
	if(bUsePCF)
	{
		shadow = bUseShadow ? calculateShadow4(fs_in.FragPosInLightSpace, normal, lightDir) : 0.0;
	}
	else
	{
		shadow = bUseShadow ? calculateShadow3(fs_in.FragPosInLightSpace, normal, lightDir) : 0.0;
	}
	vec3	result = (ambient + (1.0 - shadow) * (diffuse + specular ));
	color	= vec4(result , 1.0f);
}
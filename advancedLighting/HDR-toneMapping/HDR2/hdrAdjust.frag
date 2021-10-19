#version 330

in vec2 TextCoord;

out vec4 color;

uniform sampler2D hdrText;
uniform float exposureFactor;

vec3 withOutToneMapping()
{
	vec3 hdrColor = texture(hdrText, TextCoord).rgb;
	return hdrColor;
}

vec3 simpleToneMapping()
{
	const float gamma = 2.2f;
	vec3 hdrColor = texture(hdrText, TextCoord).rgb;
	// �������ֵ
	vec3 mappedColor = hdrColor / (hdrColor + vec3(1.0f));
	// GammaУ��
	mappedColor = pow(mappedColor, vec3(1.0f / gamma));

	return mappedColor;
}

vec3 toneMappingWithExposure()
{
	const float gamma = 2.2f;
	vec3 hdrColor = texture(hdrText, TextCoord).rgb;
	// ʹ���ع�ϵ���������ֵ
	vec3 mappedColor = vec3(1.0f) - exp(-hdrColor * exposureFactor);
	// GammaУ��
	mappedColor = pow(mappedColor, vec3(1.0f / gamma));

	return mappedColor;
}


void main()
{
	color = vec4(toneMappingWithExposure(), 1.0);
}
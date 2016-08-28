#version 330 core
out vec4 FragColor;

in vec3 vFragPos;
in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vUV;

uniform sampler2D uAlbedoMap;
uniform vec3 uLightPos;
uniform vec3 uCamPos;
uniform bool uUseLight;
uniform float uUseLightAmount;
uniform float uAttenuation;
uniform float uAmbient;

void main()
{
	bool blinn = true;

    vec3 color = texture(uAlbedoMap, vUV).rgb;

    // Ambient
    vec3 ambient = uAmbient * color;

    // Diffuse
    vec3 lightDir = normalize(uLightPos - vWorldPos);
    vec3 normal = normalize(vNormal);
    float diff = max(dot(lightDir, normal), 0.0);
	float lightPower = max(uAttenuation / (length(uLightPos - vWorldPos)), 0.75);
    vec3 diffuse = diff * color * lightPower;

    // Specular
    vec3 viewDir = normalize(uCamPos - vFragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;

    if(blinn)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    }

    vec3 specular = vec3(0.3) * spec; // assuming bright white light color

	if(uUseLight) {
		FragColor = vec4(ambient + diffuse * uUseLightAmount + specular, 1.0f);
	} else {
		FragColor = vec4(color * ambient, 1.0f);
	}
}
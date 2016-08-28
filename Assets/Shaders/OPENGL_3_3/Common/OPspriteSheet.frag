#version 330 core

in vec3 vFragPos;
in vec3 vWorldPos;
in vec2 vUV;

uniform sampler2D uColorTexture;

uniform vec3 uLightPos;
uniform vec3 uCamPos;
uniform bool uUseLight;
uniform float uAttenuation;
uniform float uAmbient;
uniform bool uShowHurt;

out vec4 FragColor;

void main(){
	bool blinn = true;

    vec4 color = texture(uColorTexture, vUV);
	if(uShowHurt) {
		FragColor = vec4(1,1,1, color.a);
	} else {

		// Ambient
		vec3 ambient = uAmbient * color.rgb;

		// Diffuse
		vec3 lightDir = normalize(uLightPos - vWorldPos);
		vec3 normal = normalize(vec3(0,0,1));
		float diff = max(dot(lightDir, normal), 0.0);
		float lightDistance = length(uLightPos - vWorldPos);	
		//float lightPower = min(uAttenuation / lightDistance, 1.0);
		float lightPower = uAttenuation / lightDistance;
		lightPower -= uAmbient;
		lightPower = max(lightPower, 0);
		vec3 diffuse = diff * color.rgb * lightPower;

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
			FragColor = vec4(ambient + diffuse + specular, color.a);
		} else {
			FragColor = vec4(ambient, color.a);
		}
		//FragColor = color;
	}
}
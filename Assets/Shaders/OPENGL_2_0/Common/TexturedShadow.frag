varying vec2 vUV;
varying vec4 vShadowCoord; // FragPosLightSpace
varying vec3 vNormal;
varying vec3 vFragPos;
varying vec3 vLightDirection;

uniform sampler2D uColorTexture;
uniform sampler2DShadow uShadow;

void main(){

	vec2 poissonDisk[16];
   	poissonDisk[0] = vec2( -0.94201624, -0.39906216 );
   	poissonDisk[1] = vec2( 0.94558609, -0.76890725 );
   	poissonDisk[2] = vec2( -0.094184101, -0.92938870 );
   	poissonDisk[3] = vec2( 0.34495938, 0.29387760 );
   	poissonDisk[4] = vec2( -0.91588581, 0.45771432 );
   	poissonDisk[5] = vec2( -0.81544232, -0.87912464 );
   	poissonDisk[6] = vec2( -0.38277543, 0.27676845 );
   	poissonDisk[7] = vec2( 0.97484398, 0.75648379 );
   	poissonDisk[8] = vec2( 0.44323325, -0.97511554 );
   	poissonDisk[9] = vec2( 0.53742981, -0.47373420 );
   	poissonDisk[10] = vec2( -0.26496911, -0.41893023 );
   	poissonDisk[11] = vec2( 0.79197514, 0.19090188 );
   	poissonDisk[12] = vec2( -0.24188840, 0.99706507 );
   	poissonDisk[13] = vec2( -0.81409955, 0.91437590 );
   	poissonDisk[14] = vec2( 0.19984126, 0.78641367 );
   	poissonDisk[15] = vec2( 0.14383161, -0.14100790 );

	vec3 n = normalize( vNormal );
	vec3 l = normalize( vLightDirection );

	float cosTheta = clamp( dot( n, l ), 0.0, 1.0 );

	//float bias = 0.005 * tan(acos(cosTheta));
	//bias = clamp(bias, 0.0, 0.01);
	float bias = 0.005;

	vec4 shadowCoord = vShadowCoord / vShadowCoord.w;
	shadowCoord.z -= bias;

	vec3 MaterialDiffuseColor = texture2D( uColorTexture, vUV ).rgb;


	//vec3 coord = vec3(shadowCoord.xy, (shadowCoord.z)/shadowCoord.w);
	float visibility = shadow2DProj( uShadow, shadowCoord ).r;

 	//for (int i=0;i<16;i++){
	//	if ( texture2D( uShadow, shadowCoord.xy + poissonDisk[i]/1400.0 ).z  <  shadowCoord.z ){
	//		shadow-=0.05;
	//		shadow = 0.0;
	//	}
	//}

	gl_FragColor =	 vec4(
		visibility * MaterialDiffuseColor
		, 1);
}

#version 460 core
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D texSampler;
layout(binding = 2) uniform sampler2D normalSampler;

layout(location = 0) in vec3 fragNormal; // contain in TBN
layout(location = 1) in vec3 fragVertexColor;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 3) in mat4 modelMatrix;
layout(location = 7) in mat4 viewMatrix;
layout(location = 11) in vec3 viewPosition;
layout(location = 12) in mat3 TBN; // tangent bi normal
layout(location = 15) in vec3 lightDir;
layout(location = 16) in vec3 fragPos;
layout(location = 17) in vec2 lightSetting;
layout(location = 18) in vec3 lightColor;

layout(location = 0) out vec4 outColor;

vec3 directional_light(vec3 normal, vec3 lightColor, vec3 surface, vec3 lightDirection, mat4 modelMatrix, mat4 viewMatrix, vec3 viewPosition, float shininess, float specularity)
{
  vec3 direction = normalize((vec4(lightDirection, 1.0)).xyz);

  //direction = tbn * direction;
  //viewPosition = tbn * viewPosition;

  vec3 halfDirection = normalize(direction + viewPosition);

  vec3 tNormal = normal;//normalize((modelMatrix * vec4(normal, 1.0)).xyz);

  float diffuse = max(dot(tNormal, direction), 0.0);
  float halfDot = max(dot(tNormal, halfDirection), 0.0);
  float specular = max(pow(halfDot, shininess), 0.0);

  return max(lightColor * (diffuse * surface +diffuse * specular * specularity), vec3(0.0));
}

vec3 directional_light(vec3 normal, vec3 lightColor, vec3 surface, vec3 lightDirection, mat4 modelMatrix, mat4 viewMatrix, vec3 viewPosition)
{
  vec3 direction = normalize((vec4(lightDirection, 1.0)).xyz);
  float diffuse = max(dot(normal, direction), 0.0);

  return max(lightColor * diffuse * surface, vec3(0.0));
}

void main()
{
	vec3 textureNormal = texture(normalSampler, fragTexCoord).rgb;
	textureNormal = normalize(textureNormal * 2.0 - 1.0);
	textureNormal = normalize(TBN * textureNormal);

	vec4 textureColor = texture(texSampler, fragTexCoord * 2.0);

	//vec3 lightDir = lightPos - fragPos;

	//float lightDistance = length(lightDir);
	//vec3 k = vec3(1.0, 0, 0);

	//float attenuation = 1.0 / (k.x+(k.y*lightDistance)+(k.z*lightDistance*lightDistance));

	float shininess = lightSetting.x; // Set to lower values for matte, higher for gloss.
	float specularity = lightSetting.y; //  The amount by which to scale the specular light cast on the object.

	outColor = vec4(directional_light(TBN[2], lightColor, textureColor.rgb, lightDir, modelMatrix, viewMatrix, viewPosition), 1.0);
}
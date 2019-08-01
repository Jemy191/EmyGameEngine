#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec3 fragVertexColor;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 3) in mat4 modelMatrix;
layout(location = 7) in mat4 viewMatrix;
layout(location = 11) in vec3 viewPosition;

layout(location = 0) out vec4 outColor;

const vec3 lightDir = vec3(0.0, 0.0, 1.0);

const float shininess = 5; // Set to lower values for matte, higher for gloss.
const float specularity = 1; //  The amount by which to scale the specular light cast on the object.

const vec3 lightColor = vec3(1.0, 1.0, 1.0);

vec3 directional_light(vec3 normal, vec3 lightColor, vec3 surface, vec3 lightDirection, mat4 modelMatrix, mat4 viewMatrix, vec3 viewPosition, float shininess, float specularity)
{
  vec3 direction = normalize((vec4(lightDirection, 1.0)).xyz);

  vec3 halfDirection = normalize(direction + viewPosition);

  vec3 tNormal = normalize((modelMatrix * vec4(normal, 1.0)).xyz);

  float diffuse = max(dot(tNormal, direction), 0.0);
  float halfDot = max(dot(tNormal, halfDirection), 0.0);
  float specular = max(pow(halfDot, shininess), 0.0);

  return max(lightColor * (diffuse * surface +diffuse * specular * specularity), vec3(0.0));
}

vec3 directional_light(vec3 normal, vec3 lightColor, vec3 surface, vec3 lightDirection, mat4 modelMatrix, mat4 viewMatrix, vec3 viewPosition)
{
  vec3 direction = normalize((modelMatrix * vec4(lightDirection, 1.0)).xyz);

  float diffuse = max(dot(normal, direction), 0.0);

  return max(lightColor * diffuse * surface, vec3(0.0));
}

void main()
{
	vec4 textureColor = texture(texSampler, fragTexCoord);

	outColor = vec4(directional_light(fragNormal, lightColor, textureColor.rgb, lightDir, modelMatrix, viewMatrix, viewPosition, shininess, specularity), 1.0);
}
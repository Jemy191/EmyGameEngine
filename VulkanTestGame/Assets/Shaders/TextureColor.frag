#version 450
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
layout(location = 15) in vec3 lightPos;
layout(location = 16) in vec3 fragPos;

layout(location = 0) out vec4 outColor;

void main()
{
	vec4 textureColor = texture(texSampler, fragTexCoord * 2.0);

	outColor = textureColor;
}
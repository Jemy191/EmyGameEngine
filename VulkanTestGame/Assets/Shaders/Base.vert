#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

// In
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec2 inTexCoord;

//Out
layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec3 fragVertexColor;
layout(location = 2) out vec2 fragTexCoord;
layout(location = 3) out vec4 eye;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    fragVertexColor = inColor;
	fragTexCoord = inTexCoord;
	fragNormal = inNormal;

	eye = -(ubo.view * vec4(inPosition, 1.0));
}
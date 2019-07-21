#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec3 fragVertexColor;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in vec4 inEye;

layout(location = 0) out vec4 outColor;

vec3 lightDir = vec3(0.773, 0.334, 0.539);

const vec4 diffuse = vec4(1.0, 0.0, 0.0, 1.0);
const vec4 ambient = vec4(0.0, 0.0, 0.0, 1.0);
const vec4 specular = vec4(1.0, 1.0, 1.0, 1.0);
const float shininess = 10;

void main()
{
	vec4 spec = vec4(0.0);

	vec3 normal = normalize(fragNormal);
	vec3 eye = normalize(vec3(inEye));

	float intensity = max(dot(normal, lightDir), 0.0);

	// if the vertex is lit compute the specular color
    if (intensity > 0.0)
	{
        // compute the half vector
        vec3 h = normalize(lightDir + eye);  
        // compute the specular term into spec
        float intSpec = max(dot(h,normal), 0.0);
        spec = specular * pow(intSpec,shininess);

    }
	vec4 textureColor = texture(texSampler, fragTexCoord);
	outColor = max(intensity *  textureColor + spec, ambient);

	//outColor = ;
}
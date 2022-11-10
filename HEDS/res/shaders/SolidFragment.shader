#version 330 core
out vec4 Color;
in vec3 Normal;
in vec3 worldPos;
uniform vec3 cameraPos;

//材质
struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
	//sampler2D emission;
};
uniform Material material;

//定向光
struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform DirLight dirLight;
vec3 CalcDirLight(DirLight light, vec3 viewDirection);
//点光源
struct PointLight {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

uniform PointLight pointLight;
vec3 CalcPointLight(PointLight light, vec3 viewDir);
//聚光灯
struct SpotLight {
	vec3 position;
	vec3 direction;
	float cutOff;
	float innerCutOff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};
uniform SpotLight spotLight;
vec3 CalcSpotLight(SpotLight light, vec3 viewDir);

void main()
{
	vec3 viewDirection = normalize(cameraPos - worldPos);
	//定向光照
	vec3 result = CalcDirLight(dirLight, viewDirection);
	//点光源
	result += CalcPointLight(pointLight, viewDirection);
	//聚光灯
	result += CalcSpotLight(spotLight, viewDirection);
	Color = vec4(result, 1.0f);
}

vec3 CalcDirLight(DirLight light, vec3 viewDir)
{
	//ambient
	vec3 ambient = light.ambient * material.ambient;
	//diffuse
	vec3 lightDirection = normalize(-light.direction);
	vec3 normal = normalize(Normal);
	float diff = max(dot(lightDirection, normal), 0.0f);
	vec3 diffuse = light.diffuse * diff * material.diffuse;
	//specular
	float ifHaveSpecular = dot(lightDirection, normal) <= 0 ? 0.0f : 1.0f;
	float spec = pow(max(dot(normal, normalize(lightDirection + viewDir)), 0.0f), material.shininess);
	vec3 specular = light.specular * ifHaveSpecular * spec * material.specular;

	return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 viewDir)
{
	//attenuation
	float dist = distance(light.position, worldPos);
	float attenuation = 1.0f / (light.constant + light.linear * dist + light.quadratic * dist * dist);
	//ambient
	vec3 ambient = attenuation * light.ambient * material.ambient;
	//diffuse
	vec3 lightDirection = normalize(light.position - worldPos);
	vec3 normal = normalize(Normal);
	float diff = max(dot(lightDirection, normal), 0.0f);
	vec3 diffuse = attenuation * light.diffuse * diff * material.diffuse;
	//specular
	float ifHaveSpecular = dot(normal, lightDirection) <= 0 ? 0.0f : 1.0f;
	float spec = pow(max(dot(normal, normalize(lightDirection + viewDir)), 0.0f), material.shininess);
	vec3 specular = attenuation * spec * light.specular * material.specular;
	return (ambient + specular + diffuse);
}

vec3 CalcSpotLight(SpotLight light, vec3 viewDir)
{
	vec3 normal = normalize(Normal);
	//聚光灯参数
	vec3 lightDirection = normalize(light.position - worldPos);
	float episilon = light.innerCutOff - light.cutOff;
	float theta = dot(-lightDirection, normalize(light.direction));
	float intensity = clamp((theta - light.cutOff) / episilon, 0.0f, 1.0f);
	//attenuation
	float dist = distance(light.position, worldPos);
	float attenuation = 1.0f / (light.constant + light.linear * dist + light.quadratic * dist * dist);
	//ambient
	vec3 ambient = intensity * attenuation * light.ambient * material.ambient;
	//diffuse
	float diff = max(dot(lightDirection, normal), 0.0f);
	vec3 diffuse = intensity * attenuation * diff * light.diffuse * material.diffuse;
	//specular
	float ifHaveSpecular = dot(lightDirection, normal) <= 0 ? 0.0f : 1.0f;
	float spec = pow(max(dot(normal, normalize(viewDir + lightDirection)), 0.0f), material.shininess);
	vec3 specular = ifHaveSpecular * attenuation * intensity * spec * light.specular * material.specular;
	return (ambient + diffuse + specular)*10;
}
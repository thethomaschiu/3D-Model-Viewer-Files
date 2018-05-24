#version 330 core
struct Material {
	int obj_mode;
	vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

struct Light {
	int mode;
	vec3 color;
    vec3 position;
    vec3 direction;

    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	float cutoff;
	float exponent;
};

in vec3 FragPos;
in vec3 Normal;

out vec4 color;

uniform int lightObj;		//int to tell if we are drawing light or obj
uniform int renderMode;		//int to tell if we are coloring with normals or phong
uniform vec3 viewPos;
uniform Light light;
uniform Material material;

//function
vec3 CalcDirectLight(Light light, vec3 normal, int objMode, vec3 viewDir);
vec3 CalcPointLight(Light light, vec3 normal, int objMode, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(Light light, vec3 normal, int objMode, vec3 fragPos, vec3 viewDir);


void main(){
	vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
	vec3 result;

	//normal rendering vs phong
	if(renderMode == 1){
		result = norm;
	}
	if(renderMode == 2){
		//differentiate if we are drawing obj or light
		if(lightObj == 1){
			result = light.color;
		}
		else {
			//DIRECTIONAL
			if(light.mode == 1){
				result = CalcDirectLight(light, norm, material.obj_mode, viewDir);
			}
			//POINT
			if (light.mode == 2){
				result = CalcPointLight(light, norm, material.obj_mode, FragPos, viewDir);
			}
			//SPOT
			if (light.mode == 3){
				result = CalcSpotLight(light, norm, material.obj_mode, FragPos, viewDir);
			}
		}
	}

    color = vec4(result, 1.0f);
}

//DIRECT LIGHT CALCULATIONS
vec3 CalcDirectLight(Light light, vec3 normal, int objMode, vec3 viewDir)
{
    //vec3 lightDir = normalize(-light.direction);
	vec3 lightDir = normalize(-light.direction);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // Combine results
    vec3 ambient = light.ambient * material.ambient;
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
	if(objMode == 1){
		return (ambient + specular);
	}
	if(objMode == 2){
		return (diffuse);
	}
	if(objMode == 3){
		return (ambient + diffuse + specular);
	}
}

//POINT LIGHT CALCULATIONS
vec3 CalcPointLight(Light light, vec3 normal, int objMode, vec3 fragPos, vec3 viewDir){
    vec3 lightDir = normalize(light.position - fragPos);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // Combine results
    vec3 ambient = light.ambient * material.ambient;
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
	if(objMode == 1){
		return (ambient + specular);
	}
	if(objMode == 2){
		return (diffuse);
	}
	if(objMode == 3){
		return (ambient + diffuse + specular);
	}
}

//SPOT LIGHT CALCULATIONS
vec3 CalcSpotLight(Light light, vec3 normal, int objMode, vec3 fragPos, vec3 viewDir){

	vec3 lightDir = normalize(-light.direction);
	//vec3 negL = normalize(light.position - fragPos);
	vec3 nL = normalize(light.position - fragPos);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), light.exponent);
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // Combine results
    vec3 ambient = light.ambient * material.ambient;
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

	vec3 final;
	if(objMode == 1){
		final = (ambient + specular);
	}
	if(objMode == 2){
		final = (diffuse);
	}
	if(objMode == 3){
		final = (ambient + diffuse + specular);
	}

	float negLD = max(dot(nL,lightDir),0.0f);
	//Spotlight cutoff calculation
	if( negLD > light.cutoff){
		final *= (pow(negLD ,light.exponent));
	}
	//for points outside cutoff angle
	if( negLD <= light.cutoff){
		final = vec3(0.,0,0);
	}

    return final;
}
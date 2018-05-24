#version 330 core

struct Material {
	vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

struct Light {
	vec3 color;
    vec3 position;

	float constant;
    float linear;
    float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 viewPos;
uniform Light light;
uniform Material material;

float stepmix(float edge0, float edge1, float E, float x){
    float T = clamp(0.5 * (x - edge0 + E) / E, 0.0, 1.0);
    return mix(edge0, edge1, T);
}

void main(){
    vec3 N = normalize(Normal);
	vec3 L  = normalize(light.position);
    vec3 Eye = vec3(0, 0, 1);
    vec3 H = normalize(L + Eye);
     
    float df = max(0.0, dot(N, L));
    float sf = max(0.0, dot(N, H));
    sf = pow(sf, material.shininess);
 
    const float A = 0.1;
    const float B = 0.3;
    const float C = 0.6;
    const float D = 1.0;
    float E = fwidth(df);
 
    if      (df > A - E && df < A + E) df = stepmix(A, B, E, df);
    else if (df > B - E && df < B + E) df = stepmix(B, C, E, df);
    else if (df > C - E && df < C + E) df = stepmix(C, D, E, df);
    else if (df < A) df = 0.0;
    else if (df < B) df = B;
    else if (df < C) df = C;
    else df = D;
 
    E = fwidth(sf);
    if (sf > 0.5 - E && sf < 0.5 + E){
        sf = smoothstep(0.5 - E, 0.5 + E, sf);
    }
    else{
        sf = step(0.5, sf);
    }
 
    vec3 color = material.ambient + df * material.diffuse + sf * material.specular;
    FragColor = vec4(color, 1.0);
}
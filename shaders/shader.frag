#version 450
#extension GL_ARB_separate_shader_objects : enable

#define maxstep 1000
#define maxdist 100.
#define surfdist .001

layout(location = 0) out vec4 outColor;

float GetDist(vec3 p){
    vec4 sphere = vec4(0, -1, 6, 1);
    float dS = length(sphere.xyz - p) - sphere.w;
    float dP = -p.y;
    return min(dP, dS);
}

float RayMarch(vec3 ro, vec3 rd){
    float dO = 0.;

    for(int i = 0; i < maxstep; ++ i){
        vec3 p = ro + dO * rd;
        float dS = GetDist(p);
        dO += dS;
        if(dO > maxdist || dO < surfdist){
            break;
        }
    }

    return dO;
}

vec3 GetNormal(vec3 p){
	float d = GetDist(p);
    vec2 e = vec2(.01, 0);
    vec3 n = d - vec3(
    	GetDist(p - e.xyy),
        GetDist(p - e.yxy),
        GetDist(p - e.yyx));
    return normalize(n);
}

float GetLight(vec3 p){
	vec3 lightPos = vec3(-2, -5, 6);
    vec3 l = normalize(lightPos - p);
	vec3 n = GetNormal(p);
    
    float dif = clamp(dot(n, l), 0., 1.);
    
    float d = RayMarch(p + n*surfdist*2, l);
    if(d < length(lightPos - p)){
        dif *= .2;
    }
    return dif;
}

void main() {
    vec2 resolution = vec2(1024, 768);
    vec2 uv = (gl_FragCoord.xy - .5 * resolution.xy) / resolution.y;
    vec3 col = vec3(0.);

    vec3 ro = vec3(0, -1, 0);
    vec3 rd = normalize(vec3(uv.xy, 1));
    float d = RayMarch(ro, rd);
    vec3 p = ro + d * rd;
    float dif = GetLight(p);
    col = vec3(dif);
    // outColor = vec4(fragColor, 1.f);
    outColor = vec4(col, 1.f);
}
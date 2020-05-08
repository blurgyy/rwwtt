#version 450
#extension GL_ARB_separate_shader_objects : enable

const int maxstep = 1000;
const float maxdist = 100.;
const float surfdist = .001;

layout(binding = 0) uniform UniformBufferObject{
    vec2 res;
} passedInfo;
layout(location = 0) out vec4 outColor;

float distField1(vec3 p){
    float dist = 0.;
    vec4 sphere = vec4(0, -2, 5, .6);
    dist = length(sphere.xyz - p) - sphere.w;
    return dist;
}
float distField2(vec3 p){
    float dist = 0.;
    dist = -p.y;
    return dist;
}

float getDist(vec3 p){
    float d[] = {
        distField1(p),
        distField2(p)
    };
    float minDist = maxdist;
    for(int i = 0; i < 2; ++ i){
        minDist = min(minDist, d[i]);
    }
    return minDist;
}

float rayMarch(vec3 ro, vec3 rd){
    float dO = 0.;
    for(int i = 0; i < maxstep; ++ i){
        vec3 p = ro + dO * rd;
        float dS = getDist(p);
        dO += dS;
        if(dO > maxdist || dO < surfdist){
            break;
        }
    }
    return dO;
}

vec3 getNormal(vec3 p){
    vec2 eps = vec2(.001, 0);
    vec3 n = vec3(
    	getDist(p + eps.xyy) - getDist(p - eps.xyy),
        getDist(p + eps.yxy) - getDist(p - eps.yxy),
        getDist(p + eps.yyx) - getDist(p - eps.yyx));
    return normalize(n);
}

float getPixelColor(vec3 p){
	vec3 lightPos = vec3(-2, -5, 6);
    vec3 l = normalize(lightPos - p);
	vec3 n = getNormal(p);
    
    float dif = clamp(dot(n, l), 0., 1.);
    
    float d = rayMarch(p + n*surfdist*2, l);
    if(d < length(lightPos - p)){
        dif *= .2;
    }
    return dif;
}

void main() {
    vec2 uv = (gl_FragCoord.xy - .5 * passedInfo.res.xy) / passedInfo.res.y;
    vec3 col = vec3(0.);

    vec3 ro = vec3(0, -1, 0);
    vec3 rd = normalize(vec3(uv.xy, 1));
    float d = rayMarch(ro, rd);
    vec3 p = ro + d * rd;
    float dif = getPixelColor(p);
    col = vec3(dif);
    outColor = vec4(col, 1.);
}
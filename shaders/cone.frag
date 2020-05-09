#version 450
#extension GL_ARB_separate_shader_objects : enable
layout(binding = 0) uniform UniformBufferObject{
    vec2 res;
    float time;
} passedInfo;
layout(location = 0) out vec4 outColor;
const int MAXSTEPS = 100;
const float MAXDIST = 1e8;
const float EPS = 1e-5;
const float PI = acos(-1.0);
const float TWOPI = 2*acos(-1.0);

// ------------------------------------------------
// from: https://www.shadertoy.com/view/4tByz3
vec3 rotateY( in vec3 p, float t ){
    float co = cos(t);
    float si = sin(t);
    p.xz = mat2(co,-si,si,co)*p.xz;
    return p;
}
vec3 rotateX( in vec3 p, float t ){
    float co = cos(t);
    float si = sin(t);
    p.yz = mat2(co,-si,si,co)*p.yz;
    return p;
}
vec3 rotateZ( in vec3 p, float t ){
    float co = cos(t);
    float si = sin(t);
    p.xy = mat2(co,-si,si,co)*p.xy;
    return p;
}
// smooth min function for blending
// from: https://iquilezles.org/www/articles/smin/smin.htm
float smin( float a, float b, float k ){
    // polynomial smooth min (k = 0.1);
    float h = clamp( 0.5+0.5*(b-a)/k, 0.0, 1.0 );
    return mix( b, a, h ) - k*h*(1.0-h);
}
// ------------------------------------------------

float dot2(vec2 x){return dot(x, x);}
float dot2(vec3 x){return dot(x, x);}

// ------------------------------------------------

float sdSphere(vec3 p, vec4 sph){
    p -= sph.xyz;
    return length(p) - sph.w;
}
float sdBox(vec3 p, vec3 rad){
    vec3 q = abs(p) - rad;
    return length(max(q, 0.)) + min(max(q.x, max(q.y, q.z)), 0.);
}
float sdRoundBox(vec3 p, vec3 rad, float r){
    vec3 q = abs(p) - rad;
    return length(max(q, 0.)) + min(max(q.x, max(q.y, q.z)), 0.) - r;
}
float sdTwistedBox(vec3 p, vec3 rad, float ext){
    vec3 q = rotateY(p, p.y*ext);
    return sdBox(q, rad);
}
float sdPlane(vec3 p){
    return -p.y;
}
float sdCappedCone( vec3 p, float h, float r1, float r2, float corner )
{
    r1 -= corner; r2 -= corner;
    vec2 q = vec2( length(p.xz), p.y );
    vec2 k1 = vec2(r2,h);
    vec2 k2 = vec2(r2-r1,2.0*h);
    vec2 ca = vec2(q.x-min(q.x,(q.y<0.0)?r1:r2), abs(q.y)-h);
    vec2 cb = q - k1 + k2*clamp( dot(k1-q,k2)/dot2(k2), 0.0, 1.0 );
    float s = (cb.x<0.0 && ca.y<0.0) ? -1.0 : 1.0;
    return s*sqrt( min(dot2(ca),dot2(cb)) ) - corner;
}
float sdBowl( vec3 p, float r, float r1, float r2 ) {
    float h1 = sqrt(r*r - r1*r1);
    float h2 = sqrt(r*r - r2*r2);
    float dist = MAXDIST;
    vec2 q = vec2(length(p.xz), -p.y);

    if(q.y > -h1 && q.x < r1){
        dist = q.y + h1;
    } else if(q.y * r1 > -h1 * q.x){
        dist = length(q - vec2(r1, -h1));
    } else if(q.y < -h2 && q.x < r2){
        dist = -h2 - q.y;
    } else {
        dist = min(length(q-vec2(r2, -h2)), length(q)-r);
    }
    return dist;
}
float sdTorus( vec3 p, vec2 t ){
    vec2 q = vec2(length(p.xz)-t.x,p.y);
    return length(q)-t.y;
}
float sdRoundCone( vec3 p, float r, float h, float corner ){
    h -= 2*corner; r -= corner;
    vec2 q = vec2(length(p.xz), -p.y);
    float dist = MAXDIST;
    vec2 m = vec2(-r, h);
    vec2 hq = vec2(q.x, q.y-h);
    vec2 rq = vec2(q.x-r, q.y);
    float m_hq = dot(m, hq);
    float m_rq = dot(m, rq);
    float mm = dot2(m);
    if(q.x < r && q.y < 0){
        dist = -q.y;
    } else {
        dist = length(m * clamp(m_rq/mm, 0, 1) - rq);
    }
    return dist - corner;
}

// ------------------------------------------------

float mapCream(vec3 p, vec3 bottom, vec3 offset,
               int rep, float angx, float angy, float r, float h){
    // vec3 offset (in which offset.y is the radius of the round corner)
    // int rep, float angx, float angy, float r, float h
    // const int rep = 7;
    // const float angx = -PI/7;
    // const float angy = PI/10;
    float cosx = cos(angx), sinx = sin(angx);
    float cosy = cos(angy), siny = sin(angy);
    const mat3 rotx = mat3(
        1,  0,   0,
        0, cosx, -sinx,
        0, sinx,  cosx
    );
    const mat3 roty = mat3(
        cosy, 0, -siny,
           0, 1,     0,
        siny, 0,  cosy
    );
    mat3 invrot = inverse(roty * rotx);
    float corner = abs(offset.y);
    float dist = MAXDIST;
    for(int i = 0; i < rep; ++ i){
        vec3 q = rotateY(p, TWOPI*i/rep);
        q = q - bottom - offset;
        q = invrot * q;
        // dist = min(dist, sdRoundCone(q, r, h, corner));
        dist = smin(dist, sdRoundCone(q, r, h, corner), 0.01);
    }
    return dist;
}

// ------------------------------------------------

float mapHead(vec3 p, vec3 bottom){
    vec3 q_top = rotateY(p, -p.y * 6);
    vec3 q_bot = rotateY(p, -p.y * 5);
    // vec3 q = p;
    // bottom cream
    vec3 offset_bot = vec3(0.1, -0.03, 0.3);
    int rep_bot = 7;
    float r_bot = 0.16,
          h_bot = 0.40,
        //   x_bot = -PI/3,
          x_bot = -1,
          y_bot = PI/10;
    // vec3 offset_bot = vec3(0.1, -0.03, 0.3);
    // int rep_bot = 7;
    // float x_bot = -PI/7,
    //       y_bot = PI/10,
    //       r_bot = 0.14,
    //       h_bot = 0.75;
    // top cream
    vec3 offset_top = vec3(0.06, -0.03, 0.3);
    int rep_top = 5;
    float r_top = 0.10,
          h_top = 0.60,
          x_top = -0.33,
          y_top = -PI/3;

    float dist = mapCream(q_bot, bottom, offset_bot, rep_bot, x_bot, y_bot, r_bot, h_bot);
    bottom += -h_bot * cos(x_bot) + vec3(0, 0.05, 0);
    dist = smin(dist, mapCream(q_top, bottom, offset_top, rep_top, x_top, y_top, r_top, h_top), 0.05);
    // dist = min(dist, mapCream(q, bottom, vec3(0.1, -0.03, 0.3), 7, -PI/7, PI/10));
    return dist;
}

vec4 mapCone(vec3 p){
    // cone1
    vec3 cone1_cent = vec3(0, -0.23, 0);
    vec3 cone1_top = 2*cone1_cent;
    vec3 cone1_bot = vec3(0, 0, 0);
    float cone1_r_top = 0.25,
          cone1_r_bot = 0.2,
          cone1_halfh = abs(cone1_cent.y);
    // cone2
    vec3 cone2_cent = vec3(0, -0.02, 0);
    vec3 cone2_top = 2*cone2_cent;
    vec3 cone2_bot = vec3(0, 0, 0);
    float cone2_r_top = cone1_r_top * 0.95,
          cone2_r_bot = cone1_r_top * 1.07,
          cone2_halfh = abs(cone2_cent.y);
    cone2_cent += cone1_top;
    cone2_top += cone1_top;
    cone2_bot += cone1_top;
    // bowl
    vec3 bowl_cent = vec3(0);
    float bowl_r = cone1_r_top * 1.3,
          bowl_r_top = bowl_r * 0.99,
          bowl_r_bot = bowl_r * 0.78,
          bowl_h_top = sqrt(bowl_r*bowl_r - bowl_r_top*bowl_r_top),
          bowl_h_bot = sqrt(bowl_r*bowl_r - bowl_r_bot*bowl_r_bot),
          bowl_h = abs(bowl_h_bot - bowl_h_top);
    bowl_cent += cone2_top - vec3(0,bowl_h_bot,0);
    vec3 bowl_top = bowl_cent + vec3(0,bowl_h_top,0);
    vec3 bowl_bot = bowl_cent + vec3(0,bowl_h_bot,0);
    // rings on cone1
    float ring_wid = 0.0075;
    vec3 ring1_cent = mix(cone1_top, cone1_bot, 0.25); vec2 ring1 = vec2(mix(cone1_r_top, cone1_r_bot, 0.25), ring_wid);
    vec3 ring2_cent = mix(cone1_top, cone1_bot, 0.50); vec2 ring2 = vec2(mix(cone1_r_top, cone1_r_bot, 0.50), ring_wid);
    vec3 ring3_cent = mix(cone1_top, cone1_bot, 0.75); vec2 ring3 = vec2(mix(cone1_r_top, cone1_r_bot, 0.75), ring_wid);

    float dist = sdTorus(p-ring1_cent, ring1);
    dist = min(dist, sdTorus(p-ring2_cent, ring2));
    dist = min(dist, sdTorus(p-ring3_cent, ring3));
    // dist = min(dist, sdCappedCone(p, cone1_top, cone1_bot, cone1_r_top, cone1_r_bot));
    dist = smin(dist, sdCappedCone(p-cone1_cent, cone1_halfh, cone1_r_top, cone1_r_bot, ring_wid), ring_wid);
    dist = min(dist, sdCappedCone(p-cone2_cent, cone2_halfh, cone2_r_top, cone2_r_bot, ring_wid));
    dist = min(dist, sdBowl(p-bowl_cent, bowl_r, bowl_r_top, bowl_r_bot));
    // return dist;
    return vec4(dist, bowl_top);
}

float map(vec3 p){
    vec3 body_center = vec3(0, -0.25, 0);
    vec3 q = vec3(fract(p.x + 0.5) - 0.5, p.yz);
    vec4 dtop = mapCone(p);
    vec3 cone_top = dtop.yzw;
    float dist = dtop.x;
    dist = smin(dist, mapHead(p, cone_top), 0.01);
    dist = min(dist, sdPlane(p));
    // dist = min(dist, sdBox(q-body_center, vec3(0.2, 0.25, 0.2)));
    return dist;
    // min(
    //     sdBox(q-body_center, vec3(0.2, 0.25, 0.2)),
    //     // sdRoundBox(q-body_center, vec3(0.2, 0.15, 0.2), 0.1),
    //     // sdTwistedBox(q-body_center, vec3(0.2, 0.25, 0.2)),
    //     // sdCappedCone(p-body_center, vec3(0, -0.5, 0), vec3(0, 0, 0), 0.3, 0.2),
    //     // sdTorus(p-body_center, vec2(0.5, 0.1)),
    //     mapCone(p),
    //     sdPlane(p)
    // );
}

vec3 getNormal(vec3 p){
    // vec2 e = vec2(1.0,-1.0)*0.5773*0.0005;
    // return normalize( e.xyy*map( p + e.xyy ) + 
	// 				  e.yyx*map( p + e.yyx ) + 
	// 				  e.yxy*map( p + e.yxy ) + 
	// 				  e.xxx*map( p + e.xxx ) );
    vec2 eps = vec2(EPS, 0);
    vec3 n = vec3(
    	map(p + eps.xyy) - map(p - eps.xyy),
        map(p + eps.yxy) - map(p - eps.yxy),
        map(p + eps.yyx) - map(p - eps.yyx));
    return normalize(n);
}

float castRay(vec3 ro, vec3 rd){
    float tmin = 1.0;
    float tmax = MAXDIST;

    float t = tmin;
    for(int i = 0; i < MAXSTEPS; ++ i){
        if(t > tmax){
            break;
        }
        float adaptive_eps = 1e-4 * t; // !
        vec3 pos = ro + t * rd;
        float delta = map(pos);
        if(delta < adaptive_eps){
            break;
        }
        t += delta;
    }
    if(t > tmax){
        t = -1.0;
    }
    return t;
}

float softShadow(vec3 ro, vec3 rd, float tmin, float tmax){
    // use tmin > 0 to prevent rays stop without leaving local area
    float ret = 1.0;
    float t = tmin;
    float prev_h = 1e20;
    for(int i = 0; i < MAXSTEPS; ++ i){
        vec3 pos = ro + t * rd;
        float h = map(pos);
        float y = h*h/(2*prev_h);
        float d = sqrt(h*h - y*y);
        ret = min( ret, 10*d/max(t-y,0) );
        prev_h = h;
        t += h;
        if(t > tmax || ret < 1e-4){
            break;
        }
    }
    return clamp(ret, 0, 1);
}

float ambientOcc(vec3 p, vec3 n){
    float occ = 0;
    float scalar = 1;
    for(int i = 0; i < 5; ++ i){
        float h = EPS + 0.15 * i;
        float d = map(p + h*n);
        occ += (h-d) * scalar;
        scalar *= 0.5;
    }
    return clamp(1.0 - 1.5 * occ, 0, 1);
}

// rendering process, returns color
vec3 render(vec3 ro, vec3 rd){
    vec3 ret_color = vec3(0);

    float t = castRay(ro, rd);

    if(t > 0.5){
        vec3 p = ro + t * rd;
        vec3 n = getNormal(p);

        vec3 kd = vec3(0.3);

        // vec3 l = normalize(vec3(-1, -3, -5)-p);
        vec3 l = normalize(vec3(-1, -3, -5)); // parallel light rays
        vec3 h = normalize(l-rd);

        vec3 light = vec3(1.0, 0.7, 0.5);
        float diffuse = clamp(dot(n, l), 0, 1) * softShadow(p, l, 0.01, 3.0);
        float specular = pow(clamp(dot(h, n), 0, 1), 150) * diffuse;

        ret_color = (kd*diffuse + specular) * light;

        vec3 ambientLight = vec3(0, 0.1, 0.2);
        float occ = ambientOcc(p, n);
        float AO = clamp(0.5 + 0.5 * n.y, 0, 1);

        ret_color += kd * occ * AO * ambientLight;

        // fog
        ret_color *= exp( -0.0005*t*t*t );
        // ret_color += diffuse + specular;
        // ret_color += softShadow(p, l, 0.01, 3.0);
        // vec3 mate = vec3(0.3);
        // ret_color = mate * 4.0*diffuse*vec3(1.00,0.70,0.5);
        // ret_color +=      12.0*specular*vec3(1.00,0.70,0.5);
    }

    return clamp(ret_color, 0, 1);
}

// ----------------------------------------------------

mat3 setCamera( in vec3 ro, in vec3 ta, float cr ){
	vec3 cw = normalize(ta-ro);
	vec3 cp = vec3(sin(cr), cos(cr),0.0);
	vec3 cu = normalize( cross(cw,cp) );
	vec3 cv = normalize( cross(cu,cw) );
    return mat3( cu, cv, cw );
}

// ----------------------------------------------------

void main(){
    vec2 uv = (gl_FragCoord.xy - .5 * passedInfo.res.xy) / passedInfo.res.y;
    vec3 col = vec3(0.);

    vec3 ro = vec3(3*sin(0.7*passedInfo.time),
                   -1.5 + 0.3*sin(0.5*passedInfo.time),
                   2*cos(0.7*passedInfo.time));
    // vec3 ro = vec3(-1, -1, -2);
    vec3 ta = vec3(0, -0.25, 0);
    // vec3 ta = vec3(passedInfo.time, -0.25, 0);
    mat3 camRot = setCamera( ro, ta, 0.0 );
    vec3 rd = camRot * normalize(vec3(uv.x, uv.y, 1));

    col = render(ro, rd);

    // float dScene = castRay(ro, rd);
    // vec3 p = ro + dScene * rd;
    // col = phongShading(p, rd);
    // col = naiveShading(p, rd);
    // col = getNormal(p);
    // col = vec3(dScene) / 20.0;
    outColor = vec4(col.xyz, 1.);
}

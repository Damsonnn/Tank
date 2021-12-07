#version 330

uniform sampler2D tex;

out vec4 endColor; 
 
in vec4 n;
in vec4 l1;
in vec4 l2;
in vec4 v;
in vec2 iTexCoords;

void main(void) {

	vec4 ml1 = normalize(l1);
	vec4 ml2 = normalize(l2);
	vec4 mn = normalize(n);
	vec4 mv = normalize(v);

	vec4 mr1 = reflect(-ml1, mn);
	vec4 mr2 = reflect(-ml2, mn);

	vec4 kd = texture(tex, iTexCoords);

	float nl1 = clamp(dot(mn, ml1), 0, 1);
	float rv1 = pow(clamp(dot(mr1, mv), 0, 1),90);
	float nl2 = clamp(dot(mn, ml2), 0, 1);
	float rv2 = pow(clamp(dot(mr2, mv), 0, 1),25);

	if (nl1 > nl2) endColor= vec4(kd.rgb * nl1, kd.a) + (rv1,rv1,rv1, 0);
	else  endColor= vec4(kd.rgb * nl2, kd.a) + (rv2,rv2,rv2, 0);
}

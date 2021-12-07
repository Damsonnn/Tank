#version 330


uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec4 lp1;
uniform vec4 lp2;

in vec4 vertex; 
in vec4 normal; 
in vec2 texCoords;

//Zmienne interpolowane
out vec4 l1;
out vec4 l2;
out vec4 n;
out vec4 v;
out vec2 iTexCoords;


void main(void) {
    l1 = normalize(V * lp1 - V*M*vertex); 
    l2 = normalize(V * lp2 - V*M*vertex); 
    v = normalize(vec4(0, 0, 0, 1) - V * M * vertex); 
    n = normalize(V * M * normal); 
    iTexCoords = texCoords;
   
    gl_Position=P*V*M*vertex;
}

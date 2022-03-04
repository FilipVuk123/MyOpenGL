#version 330 core
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D texture;

void main(){
   float R = texture2D(texture, TexCoord).r;
   float G = texture2D(texture, TexCoord).g; 
   float B = texture2D(texture, TexCoord).b;
   float A = texture2D(texture, TexCoord).a;
   FragColor = vec4(R, G, B, A);
}; 
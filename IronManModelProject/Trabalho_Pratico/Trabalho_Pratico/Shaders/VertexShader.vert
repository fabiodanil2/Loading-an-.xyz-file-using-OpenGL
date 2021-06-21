#version 440 core


uniform mat4 Model;
uniform mat4 View;
uniform mat4 ModelView;		// View * Model
uniform mat4 Projection;
uniform mat3 NormalMatrix;
layout(location = 0) uniform float time;

layout( location = 0 ) in vec3 vPosition;
layout( location = 1 ) in vec3 vColors;
layout( location = 2 ) in vec2 vTextureCoords;
layout( location = 3 ) in vec3 vNormal;			// Normal do v�rtice
uniform mat4 MVP;

out vec3 vPositionEyeSpace;
out vec3 vNormalEyeSpace;

layout(location = 0) out vec3 color;
out vec2 textureCoord;

void main()
{
	
   //gl_Position = MVP * vec4(vPosition, 1.0f);
   
   
	// Posi��o do v�rtice em coordenadas do olho.
	vPositionEyeSpace = (ModelView * vec4(vPosition, 1.0)).xyz;

	color = vColors;
	textureCoord = vTextureCoords;


	// Transformar a normal do v�rtice.
	vNormalEyeSpace = normalize(NormalMatrix * vNormal);


	// Posi��o final do v�rtice (em coordenadas de clip)
	gl_Position = Projection * ModelView * vec4(vPosition*abs(sin(time))/* Deforma�ao em funcao do tempo*/, 1.0f);
}

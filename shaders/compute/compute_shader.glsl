#version 460 core

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D outputImage;

uniform sampler2D frontTex;
uniform sampler2D depthTex;
uniform sampler2D backTex;
uniform int startFog;
uniform int endFog;


float linearize_depth(float d,float zNear,float zFar)
{
    return zNear * zFar / (zFar + d * (zNear - zFar));
}

void main() {
	ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);
	
	vec4 outputColor = vec4(0., 0., 0., 1.);
	vec4 frontColor = texelFetch(frontTex, pixelCoord, 0);
	vec4 backColor = texelFetch(backTex, pixelCoord, 0);

	float depth = texelFetch(depthTex, pixelCoord, 0).x;

	depth = linearize_depth(depth, 0.1, 16000.0);

	vec4 color = vec4(0., 0., 0., 1.);

	int start = startFog;
	int end = endFog;
	
	if (depth > start && depth < end)
	{
		outputColor = mix(frontColor, backColor, (depth-start)/end);
	}
	else if (depth > end)
	{
		outputColor = backColor;
	}
	else
	{
		outputColor = frontColor;
	}
	

    
    imageStore(outputImage, pixelCoord, outputColor);
}
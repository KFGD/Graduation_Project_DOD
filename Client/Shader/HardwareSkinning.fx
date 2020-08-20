float4 gLightDir = { 0.0f, -1.0f, 1.0f, 1.0f };    //light Direction 
float4 gLightDiffuse = { 1.0f, 1.0f, 1.0f, 1.0f }; // Light Diffuse

float4x3	gMatrixPalette[26] : WORLDMATRIXARRAY;
int	gNumBoneInfluences = 2;

float4x4	gMatWorld;
float4x4	gMatVP;

texture		gDiffuseTexture;
sampler DiffuseSampler = sampler_state
{
	MinFilter = linear;
	MagFilter = linear;
	MipFilter = linear;
	Texture = gDiffuseTexture;
};


struct VS_IN
{
	float4	Position		: POSITION;
	float3	Normal			: NORMAL;
	float2	TexUV			: TEXCOORD0;

	float4	BlendWeights	: BLENDWEIGHT;
	float4	BlendIndices	: BLENDINDICES;
};

struct VS_OUT
{
	float4	Position	: POSITION;
	float4	Normal		: NORMAL;
	float2	TexUV		: TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT	Out = (VS_OUT)0;

	float3	position = 0.f;
	float3	normal = 0.f;
	float	lastWeight = 0.f;
	
	//	Skinning(Position, Normal)
	int4	indexVector = D3DCOLORtoUBYTE4(In.BlendIndices);

	float	blendWeightsArray[4] = (float[4])In.BlendWeights;
	int	indexArray[4] = (int[4])indexVector;

	for (int i = 0; i < gNumBoneInfluences - 1; ++i)
	{
		lastWeight = lastWeight + blendWeightsArray[i];

		position += mul(In.Position, gMatrixPalette[indexArray[i]]) * blendWeightsArray[i];
		normal += mul(In.Normal, gMatrixPalette[indexArray[i]]) * blendWeightsArray[i];
	}

	lastWeight = 1.0f - lastWeight;

	position += (mul(In.Position, gMatrixPalette[indexArray[gNumBoneInfluences - 1]]) * lastWeight);
	normal += mul(In.Normal, gMatrixPalette[indexArray[gNumBoneInfluences - 1]]) * lastWeight;
	
	//	Position Transform: Local -> World -> View & Projection
	Out.Position = mul(float4(position, 1.f), gMatWorld);
	Out.Position = mul(Out.Position, gMatVP);

	//	Normal Transform: Local -> World
	Out.Normal = normalize(mul(float4(normal, 0.f), gMatWorld));

	//	UV
	Out.TexUV = In.TexUV;

	return Out;
}

float4 PS_MAIN(VS_OUT In) : COLOR
{
	float4 Diffuse = tex2D(DiffuseSampler, In.TexUV);

	float4 lightDir = normalize(gLightDir);
	float4 Shade = saturate(dot(lightDir * -1.f, In.Normal));

	float4 Color = (gLightDiffuse * Diffuse) * (Shade + 0.3f);

	return Color;
}


technique HardwareSkinning
{
	pass Skinning
	{
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
}
float4 gLightDir = { 0.0f, -1.0f, 1.0f, 1.0f };    //light Direction 
float4 gLightDiffuse = { 1.0f, 1.0f, 1.0f, 1.0f }; // Light Diffuse

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
	float3	Position		: POSITION;
	float3	Normal			: NORMAL;
	float2	TexUV			: TEXCOORD0;
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
	
	//	Position Transform: Local -> World -> View & Projection
	Out.Position = mul(float4(In.Position, 1.f), gMatWorld);
	Out.Position = mul(Out.Position, gMatVP);

	//	Normal Transform: Local -> World
	Out.Normal = normalize(mul(float4(In.Normal, 0.f), gMatWorld));

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

	Color *= 0.25f;

	return Color;
}


technique MeshRender
{
	pass Render
	{
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
}
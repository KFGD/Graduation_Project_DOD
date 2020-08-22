float4x4	gMatVP;
float4x4	gMatWorld;
float4		gColor;

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
	return gColor;
}


technique NaviMeshRender
{
	pass Render
	{
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
}
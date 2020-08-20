float4 gLightDir = { 0.0f, -1.0f, 1.0f, 1.0f };    //light Direction 
float4 gLightDiffuse = { 1.0f, 1.0f, 1.0f, 1.0f }; // Light Diffuse

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
	float3	Position: POSITION;
	float3	Normal	: NORMAL;
	float2	TexUV	: TEXCOORD0;

	float4	InstanceRight	: TEXCOORD1;
	float4	InstanceUp		: TEXCOORD2;
	float4	InstanceLook	: TEXCOORD3;
	float4	InstancePos		: TEXCOORD4;
};

struct VS_OUT
{
	float4	Position	: POSITION;
	float4	Normal		: NORMAL;
	float2	TexUV		: TEXCOORD0;
};

VS_OUT	VS_MAIN(VS_IN In)
{
	VS_OUT	Out = (VS_OUT)0;

	float4x4 matWorld = { In.InstanceRight, In.InstanceUp, In.InstanceLook, In.InstancePos };

	matrix matWVP = mul(matWorld, gMatVP);

	Out.Position = mul(float4(In.Position, 1.f), matWVP);
	Out.TexUV = In.TexUV;
	Out.Normal = mul(float4(In.Normal, 0.f), matWorld);

	return Out;
}

float4	PS_MAIN(VS_OUT In) : COLOR
{
	float4 Diffuse = tex2D(DiffuseSampler, In.TexUV);

	float4 lightDir = normalize(gLightDir);
	float4 Shade = saturate(dot(lightDir * -1.f, In.Normal));

	float4 Color = (gLightDiffuse * Diffuse) * (Shade + 0.3f);

	return Color;
}

technique HardwareInstancing
{
	pass Instancing
	{
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
};
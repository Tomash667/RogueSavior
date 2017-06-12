/* SUPER SHADER
--------------------------------------------------------------------------------
Switches:
- ANIMATED

Defines:
- VS_VERSION
- PS_VERSION
*/

//******************************************************************************
// macierze
shared float4x4 matCombined;
shared float4x4 matWorld;
shared float4x4 matBones[32];

// dodatkowy kolor/alpha
shared float4 tint;

// oswietlenie ambient
shared float3 ambientColor;

// mgła
shared float3 fogColor;
shared float3 fogParams; //(x=fogStart,y=fogEnd,z=(fogEnd-fogStart))

// oswietlenie na zewnatrz
shared float3 lightDir;
shared float3 lightColor;

shared float3 cameraPos;
shared float3 specularColor;
shared float specularIntensity;
shared float specularHardness;

// tekstury
shared texture texDiffuse;

// samplery
sampler2D samplerDiffuse = sampler_state
{
	texture = <texDiffuse>;
	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
};

//******************************************************************************
struct MESH_INPUT
{
  float3 pos : POSITION;
	#ifdef ANIMATED
		half weight : BLENDWEIGHT0;
		int4 indices : BLENDINDICES0;
	#endif
  float3 normal : NORMAL;
  float2 tex : TEXCOORD0;
};

//******************************************************************************
struct MESH_OUTPUT
{
	float4 pos : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : TEXCOORD1;
	float posViewZ : TEXCOORD2;
	float3 viewDir : TEXCOORD6;
};

//******************************************************************************
void vs_mesh(in MESH_INPUT In, out MESH_OUTPUT Out)
{
	// uv tekstury
	Out.tex = In.tex;
	
	// pozycja
	#ifdef ANIMATED
		float3 pos = mul(float4(In.pos,1), matBones[In.indices[0]]) * In.weight;
		pos += mul(float4(In.pos,1), matBones[In.indices[1]]) * (1-In.weight);
		Out.pos = mul(float4(pos,1), matCombined);
	#else
		float3 pos = In.pos;
		Out.pos = mul(float4(In.pos,1), matCombined);
	#endif

	// normalna / kolor
	#if defined(ANIMATED)
		float3 normal = mul(float4(In.normal,1), matBones[In.indices[0]]) * In.weight;
		normal += mul(float4(In.normal,1), matBones[In.indices[1]]) * (1-In.weight);
		Out.normal = mul(normal, matWorld);
	#else
		Out.normal = mul(In.normal, matWorld);
	#endif

	// odleglosc dla mgly
	Out.posViewZ = Out.pos.w;
	
	Out.viewDir = cameraPos - mul(float4(pos,1), matWorld);
}

//******************************************************************************
float4 ps_mesh(in MESH_OUTPUT In) : COLOR0
{
	float4 tex = tex2D(samplerDiffuse, In.tex) * tint;
	
	float fog = saturate((In.posViewZ-fogParams.x)/fogParams.z);

	float3 normal = In.normal;
	
	float specularInt = specularIntensity;
	float specular = 0;
	
	float lightIntensity = 0;

	// oswietlenie kierunkowe
	lightIntensity = saturate(dot(normal, lightDir));
	float3 diffuse = lightColor * lightIntensity;
	if(lightIntensity > 0)
	{
		float3 reflection = normalize(lightIntensity*2*normal - lightDir);
		specular = pow(saturate(dot(reflection, normalize(In.viewDir))), specularHardness) * specularInt;
	}		
	
	tex = float4(saturate((tex.xyz * saturate(ambientColor + diffuse) ) + specularColor * specular), tex.w);
	
	return float4(lerp(tex.xyz, fogColor, fog), tex.w);
}

//******************************************************************************
technique mesh
{
	pass pass0
	{
		VertexShader = compile VS_VERSION vs_mesh();
		PixelShader = compile PS_VERSION ps_mesh();
	}
}

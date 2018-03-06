cbuffer cbPlayerInfo : register(b0)
{
	matrix		gmtxPlayerWorld : packoffset(c0);
};

cbuffer cbCameraInfo : register(b1)
{
	matrix		gmtxView : packoffset(c0);
	matrix		gmtxProjection : packoffset(c4);
	float3		gvCameraPosition : packoffset(c8);
};

cbuffer cbGameObjectInfo : register(b2)
{
	matrix		gmtxGameObject : packoffset(c0);
	uint		gnMaterial : packoffset(c4);
};

struct TB_EFFECTOBJFECT_INFO
{
	matrix		m_gmtxEffectObject;
	float2		 m_f2AddUV;
};

StructuredBuffer<TB_EFFECTOBJFECT_INFO> gEffectObjectInfos : register(t7);

#include "Light.hlsl"

struct VS_DIFFUSED_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
};

struct VS_DIFFUSED_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

VS_DIFFUSED_OUTPUT VSDiffused(VS_DIFFUSED_INPUT input)
{
	VS_DIFFUSED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.color = input.color;

	return(output);
}

float4 PSDiffused(VS_DIFFUSED_OUTPUT input) : SV_TARGET
{
	return(input.color);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
VS_DIFFUSED_OUTPUT VSPlayer(VS_DIFFUSED_INPUT input)
{
	VS_DIFFUSED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxPlayerWorld), gmtxView), gmtxProjection);
	output.color = input.color;

	return(output);
}

float4 PSPlayer(VS_DIFFUSED_OUTPUT input) : SV_TARGET
{
	return(input.color);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
Texture2D gtxtTexture : register(t0);

SamplerState gWrapSamplerState : register(s0);
SamplerState gClampSamplerState : register(s1);

struct VS_TEXTURED_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
};

struct VS_TEXTURED_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

VS_TEXTURED_OUTPUT VSTextured(VS_TEXTURED_INPUT input)
{
	VS_TEXTURED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
}

float4 PSTextured(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtTexture.Sample(gWrapSamplerState, input.uv);

	return(cColor);
}

#define _WITH_VERTEX_LIGHTING

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_LIGHTING_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
};

struct VS_LIGHTING_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
#ifdef _WITH_VERTEX_LIGHTING
	float4 color : COLOR;
#endif
};

VS_LIGHTING_OUTPUT VSLighting(VS_LIGHTING_INPUT input)
{
	VS_LIGHTING_OUTPUT output;

	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
#ifdef _WITH_VERTEX_LIGHTING
	output.normalW = normalize(output.normalW);
	output.color = Lighting(output.positionW, output.normalW);
#endif
	return(output);
}

float4 PSLighting(VS_LIGHTING_OUTPUT input) : SV_TARGET
{
#ifdef _WITH_VERTEX_LIGHTING
	float4 cIllumination = input.color;
#else
	input.normalW = normalize(input.normalW);
	float4 cIllumination = Lighting(input.positionW, input.normalW);
#endif
	return(cIllumination);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_TEXTURED_LIGHTING_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

struct VS_TEXTURED_LIGHTING_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	//	nointerpolation float3 normalW : NORMAL;
	float2 uv : TEXCOORD;
#ifdef _WITH_VERTEX_LIGHTING
	float4 color : COLOR;
#endif
};

VS_TEXTURED_LIGHTING_OUTPUT VSTexturedLighting(VS_TEXTURED_LIGHTING_INPUT input)
{
	VS_TEXTURED_LIGHTING_OUTPUT output;

	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv;
#ifdef _WITH_VERTEX_LIGHTING
	output.normalW = normalize(output.normalW);
	output.color = Lighting(output.positionW, output.normalW);
#endif
	return(output);
}

float4 PSTexturedLighting(VS_TEXTURED_LIGHTING_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtTexture.Sample(gWrapSamplerState, input.uv);
#ifdef _WITH_VERTEX_LIGHTING
	float4 cIllumination = input.color;
#else
	input.normalW = normalize(input.normalW);
	float4 cIllumination = Lighting(input.positionW, input.normalW);
#endif
	return(lerp(cColor, cIllumination, 0.5f));
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
Texture2D gtxtTerrainBaseTexture : register(t1);
Texture2D gtxtTerrainDetailTexture : register(t2);
Texture2D gtxtTerrainNormalTexture : register(t3);

struct VS_TERRAIN_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
};

struct VS_TERRAIN_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
	float3 normalW : NORMAL;
	float3 tangentW : TANGENT;
	float   normalY : NORMALY;
};

VS_TERRAIN_OUTPUT VSTerrain(VS_TERRAIN_INPUT input)
{
	VS_TERRAIN_OUTPUT output;

	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);

	output.tangentW = mul(input.tangent, (float3x3)gmtxGameObject);
	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);

	output.color = input.color;

	output.uv0 = input.uv0;
	output.uv1 = input.uv1;

	output.normalY = input.normal.y;

	return(output);
}

float4 PSTerrain(VS_TERRAIN_OUTPUT input) : SV_TARGET
{
	float3 N = normalize(input.normalW);	// 탄젠트가 없는 경우 기존 노말값 사용
	if (input.tangentW.x != 0 || input.tangentW.y != 0 || input.tangentW.z != 0 )
	{
		float3 T = normalize(input.tangentW - dot(input.tangentW, N) * N);
		float3 B = cross(N, T);	// 노말과 탄젠트를 외적해서 바이 탄젠트(바이 노말)생성
		float3x3 TBN = float3x3(T, B, N); // 이를 바탕으로 TBN행렬 생성
		float3 normal = gtxtTerrainNormalTexture.Sample(gWrapSamplerState, input.uv1); // 노말 맵에서 해당하는 uv에 해당하는 노말 읽기
		normal = 2.0f * normal - 1.0f; // 노말을 -1에서 1사이의 값으로 변환
		N = mul(normal, TBN); // 노말을 TBN행렬로 변환
	}

	float4 cBaseTexColor = gtxtTerrainBaseTexture.Sample(gWrapSamplerState, input.uv0);
	float4 cDetailTexColor = gtxtTerrainDetailTexture.Sample(gWrapSamplerState, input.uv1);
	float4 cIlluminationBase = Lighting(input.positionW, N);
	float4 cColor;

	// Y가 작을 수록 디테일 텍스처를 더 많이 적용 -> 절벽일 수록 더 강하게 적용
	cColor = input.color * saturate((cBaseTexColor * input.normalY) + (cDetailTexColor * (1- input.normalY))) * cIlluminationBase;
	
	return(cColor);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
Texture2D gtxtSkyBox : register(t4);

float4 PSSkyBox(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtSkyBox.Sample(gClampSamplerState, input.uv);

	return(cColor);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
Texture2DArray gtxtGrassTreeTextureArray : register(t5);

struct VS_BILLBOARD_INPUT
{
	float3 position : POSITION;
	float2 size : SIZE;
	uint	textureNumber : TEXTURENUM;
};

struct VS_BILLBOARD_OUTPUT
{
	float3 centerW : POSITION;
	float2 sizeW : SIZE;
	uint	textureNumber : TEXTURENUM;
};

struct GS_BILLBOARD_OUTPUT
{
	float4 posH : SV_POSITION;
	float3 posW : POSITION;
	float3 normalW : NORMAL;
	float2 uv : TEXCOORD;
	uint	textureNumber : TEXTURENUM;
};

VS_BILLBOARD_OUTPUT VS_BillBoard(VS_BILLBOARD_INPUT input)
{
	VS_BILLBOARD_OUTPUT output;
	output.centerW = input.position;
	output.sizeW = input.size;
	output.textureNumber = input.textureNumber;
	return output;
}

[maxvertexcount(4)]
void GS_BillBoard(point VS_BILLBOARD_OUTPUT input[1], inout TriangleStream<GS_BILLBOARD_OUTPUT> outStream)
{
	float3 vUp = float3(0.0f, 1.0f, 0.0f); 
	float3 vLook = gvCameraPosition - input[0].centerW;
	vLook = normalize(vLook);	// Look 벡터 (픽셀 쉐이더에서 노말로 사용)
	float3 vRight = cross(vUp, vLook);	// Right 벡터 생성(y축 회전 담당)
	vUp = cross(vLook, vRight);		// 회전한 Up벡터 생성(x축 회전 담당)

	float fHalfW = input[0].sizeW.x * 0.5f;
	float fHalfH = input[0].sizeW.y * 0.5f;

	float4 pVertices[4];
	pVertices[0] = float4(input[0].centerW + fHalfW * vRight - fHalfH * vUp, 1.0f);	// 좌 하단 점
	pVertices[1] = float4(input[0].centerW + fHalfW * vRight + fHalfH * vUp, 1.0f); // 좌 상단 점
	pVertices[2] = float4(input[0].centerW - fHalfW * vRight - fHalfH * vUp, 1.0f); //우 하단 점
	pVertices[3] = float4(input[0].centerW - fHalfW * vRight + fHalfH * vUp, 1.0f); // 우 상단 점

	float2 pUVs[4] = { float2(0.0f, 1.0f), float2(0.0f, 0.0f), float2(1.0f,1.0f), float2(1.0f, 0.0f) };

	GS_BILLBOARD_OUTPUT output;
	for (int i = 0; i < 4; ++i)
	{
		output.posW = pVertices[i].xyz;
		output.posH = mul(mul(pVertices[i], gmtxView), gmtxProjection);
		output.normalW = vLook;
		output.uv = pUVs[i];
		output.textureNumber = input[0].textureNumber;
		outStream.Append(output);
	}
}

float4 PS_BillBoard(GS_BILLBOARD_OUTPUT input) : SV_Target
{
	float4 cIllumination = Lighting(input.posW, input.normalW);
	float3 uvw = float3(input.uv, input.textureNumber);
	float4 cTexture = gtxtGrassTreeTextureArray.Sample(gWrapSamplerState, uvw);
	float4 cColor = cIllumination *cTexture;
	cColor.a = cTexture.a;

	if (cColor.a < 0.2) discard;

	return (cColor);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
Texture2D gtxtEffect : register(t6);

VS_TEXTURED_OUTPUT VSInstancingTextured(VS_TEXTURED_INPUT input, uint nInstanceID : SV_InstanceID)
{
	VS_TEXTURED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gEffectObjectInfos[nInstanceID].m_gmtxEffectObject), gmtxView), gmtxProjection);
	output.uv = input.uv + gEffectObjectInfos[nInstanceID].m_f2AddUV;

	return(output);
}

float4 PSInstancingTextured(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtEffect.Sample(gWrapSamplerState, input.uv);
	if (cColor.a <= 0.2f) discard;
	return(cColor);
}
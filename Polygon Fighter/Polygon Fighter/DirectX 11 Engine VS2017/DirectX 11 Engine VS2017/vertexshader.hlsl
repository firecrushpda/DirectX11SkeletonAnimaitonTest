#pragma pack_matrix( row_major )

cbuffer perObjectBuffer : register(b0)
{
    float4x4 wvpMatrix;
    float4x4 worldMatrix;
}; 

cbuffer boneinfoBuffer : register(b1)
{
	float4x4 bones[255];
};

struct VS_INPUT
{
    float3 inPos : POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
	float3 inTangent : TANGENT;
	float4 BoneIDs : COLOR0;
	float4 Weights : COLOR1;
};

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float2 outTexCoord : TEXCOORD;
    float3 outNormal : NORMAL;
	float3 outTangent: TANGENT;
    float3 outWorldPos : WORLD_POSITION;
};

VS_OUTPUT main(VS_INPUT input) 
{
	//float4x4 modelMatrix = worldMatrix * transpose(wvpMatrix);

	//bonetransform
	float4x4 BoneTransform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	if (input.Weights.x != -1) {
		BoneTransform = bones[input.BoneIDs.x] * input.Weights.x;
	}
	if (input.Weights.y != -1)
	{
		BoneTransform += bones[input.BoneIDs.y] * input.Weights.y;
	}
	if (input.Weights.z != -1)
	{
		BoneTransform += bones[input.BoneIDs.z] * input.Weights.z;
	}
	if (input.Weights.w != -1)
	{
		BoneTransform += bones[input.BoneIDs.w] * input.Weights.w;
	}

	float4 PosL = mul(float4(input.inPos, 1.0),(BoneTransform));
	float4 NorL = normalize(mul(float4(input.inNormal,0.0),(BoneTransform)));
	float4 TanL = normalize(mul(float4(input.inTangent,1.0), (BoneTransform)));
	float2 TexCoordL = normalize(mul(input.inTexCoord, (BoneTransform)));

    VS_OUTPUT output;
    output.outPosition = mul(PosL,wvpMatrix);
	output.outWorldPos = mul(PosL,worldMatrix).rgb;
	output.outNormal = normalize(mul(NorL, worldMatrix)).rgb;//NorL;//
	output.outTangent = normalize(mul(TanL, worldMatrix)).rgb;
	output.outTexCoord = input.inTexCoord;

    return output;
}
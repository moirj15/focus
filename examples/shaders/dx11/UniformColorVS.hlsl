cbuffer Constants : register(b0)
{
	float4x4 mvp;
}


float4 VSMain(float3 position : POSITION) : SV_POSITION
{
	return mul(mvp, float4(position, 1.0));
}

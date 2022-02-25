cBuffer PerObject
{
  float4x4 modelViewProjection;
  float3 color;
};

struct VertexIn {
  float3 position : POSITION;
};

struct VertexOut {
  float4 position : SV_POSITION;
  float4 color : COLOR;
};

VertexOut main(VertexIn input)
{
  VertexOut ret;
  ret.position = modelViewProjection * input.position;
  ret.color = color;
  return ret;
}
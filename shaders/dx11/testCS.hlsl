RWBuffer<float4> color_buf;

[numthreads(1, 1, 1)]
void CSMain(int3 dispatchThreadID : SV_DispatchThreadID) {
    color_buf[(dispatchThreadID.x * 256) + dispatchThreadID.y] = float4(1.0, 0.0, 0.0, 1.0);
}
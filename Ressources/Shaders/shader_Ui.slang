struct VSInput {
    float2 inPosition;
    float2 inTexCoord;
    float2 inTexCoord2;
};

struct Transform
{
    float3x3 model;
}

struct PushData
{
    float4 inColor;
    float3 raw1;
    float width;
    float3 raw2;
    float height;
    float4 raw3;
}

[[vk::binding(0, 0)]]
ConstantBuffer<Sampler2D> texture;

[[vk::binding(0, 1)]]
ConstantBuffer<Sampler2D> texture2;

[[vk::push_constant()]]
ConstantBuffer<PushData> pushConstantData;

struct VSOutput
{
    float4 pos : SV_Position;
    float2 fragTexCoord;
    float2 fragTexCoord2;
    float4 fragColor;
};


[shader("vertex")]
VSOutput vertMain(VSInput input) {  
    VSOutput output;

    float aspectRatio = pushConstantData.width / pushConstantData.height;
    float3x3 transform3x3 = float3x3(pushConstantData.raw1.xyz, pushConstantData.raw2.xyz, pushConstantData.raw3.xyz);
    float3 wpos = mul(transform3x3, float3(input.inPosition, 1.0f));
    float2 pos;
    pos.x = wpos.x / aspectRatio;
    pos.y = wpos.y;
    output.pos = float4(pos,0.0f,1.0f);
    output.fragColor = pushConstantData.inColor;
    output.fragTexCoord = input.inTexCoord;
    output.fragTexCoord2 = input.inTexCoord2;
    return output;
}

[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_TARGET {
    float4 color = pow(texture.Sample(vertIn.fragTexCoord).rgba, 2.2f.xxxx);
    float4 color2 = pow(texture2.Sample(vertIn.fragTexCoord2).rgba, 2.2f.xxxx);
    return color2*color*vertIn.fragColor;
}
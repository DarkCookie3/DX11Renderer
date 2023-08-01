float3 fxaa(Texture2D<float4> tex, SamplerState samplerArg,  float2 p, float2 RES)
{
    float FXAA_SPAN_MAX = 8.0;
    float FXAA_REDUCE_MUL = 1.0 / 8.0;
    float FXAA_REDUCE_MIN = 1.0 / 128.0;

    float3 rgbNW = tex.Sample(samplerArg, p + (float2(-1., -1.) / RES));
    float3 rgbNE = tex.Sample(samplerArg, p + (float2(1., -1.) / RES));
    float3 rgbSW = tex.Sample(samplerArg, p + (float2(-1., 1.) / RES));
    float3 rgbSE = tex.Sample(samplerArg, p + (float2(1., 1.) / RES));
    float3 rgbM = tex.Sample(samplerArg, p);

    float3 luma = float3(0.299, 0.587, 0.114);

    float lumaNW = dot(rgbNW, luma);
    float lumaNE = dot(rgbNE, luma);
    float lumaSW = dot(rgbSW, luma);
    float lumaSE = dot(rgbSE, luma);
    float lumaM = dot(rgbM, luma);

    float2 dir;
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y = ((lumaNW + lumaSW) - (lumaNE + lumaSE));
    
    float lumaSum = lumaNW + lumaNE + lumaSW + lumaSE;
    float dirReduce = max(lumaSum * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
    float rcpDirMin = 1. / (min(abs(dir.x), abs(dir.y)) + dirReduce);

    dir = min(float2(FXAA_SPAN_MAX, FXAA_SPAN_MAX), max(float2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), dir * rcpDirMin)) / RES;

    float3 rgbA = .5 * (tex.Sample(samplerArg, p + dir * (1. / 3. - .5)) +
        			  tex.Sample(samplerArg, p + dir * (2. / 3. - .5)));
    float3 rgbB = rgbA * .5 + .25 * (
        			  tex.Sample(samplerArg, p + dir * (0. / 3. - .5)) +
        			  tex.Sample(samplerArg, p + dir * (3. / 3. - .5)));
    
    float lumaB = dot(rgbB, luma);
    
    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

    return ((lumaB < lumaMin) || (lumaB > lumaMax)) ? rgbA : rgbB;
}
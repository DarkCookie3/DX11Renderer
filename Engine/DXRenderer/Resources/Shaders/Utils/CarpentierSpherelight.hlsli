
void SphereMaxNoH(float NoV, inout float NoL, inout float VoL, float SinAlpha, float CosAlpha, bool bNewtonIteration, out float NoH, out float VoH)
{
    float RoL = NoL * NoV - VoL;
    if (RoL >= CosAlpha)
    {
        NoH = 1;
        VoH = abs(NoV);
    }
    else
    {
        float rInvLengthT = SinAlpha * rsqrt(1 - RoL * RoL);
        float NoTr = rInvLengthT * (NoV - RoL * NoL);
        float VoTr = rInvLengthT * (2 * NoV * NoV - 1 - RoL * VoL);

        if (bNewtonIteration && SinAlpha != 0)
        {
			// dot( cross(N,L), V )
            float NxLoV = sqrt(saturate(1 - pow(NoL, 2) - pow(NoV, 2) - pow(VoL, 2) + 2 * NoL * NoV * VoL));

            float NoBr = rInvLengthT * NxLoV;
            float VoBr = rInvLengthT * NxLoV * 2 * NoV;

            float NoLVTr = NoL * CosAlpha + NoV + NoTr;
            float VoLVTr = VoL * CosAlpha + 1 + VoTr;

            float p = NoBr * VoLVTr;
            float q = NoLVTr * VoLVTr;
            float s = VoBr * NoLVTr;

            float xNum = q * (-0.5 * p + 0.25 * VoBr * NoLVTr);
            float xDenom = p * p + s * (s - 2 * p) + NoLVTr * ((NoL * CosAlpha + NoV) * pow(VoLVTr, 2) + q * (-0.5 * (VoLVTr + VoL * CosAlpha) - 0.5));
            float TwoX1 = 2 * xNum / (pow(xDenom, 2) + pow(xNum, 2));
            float SinTheta = TwoX1 * xDenom;
            float CosTheta = 1.0 - TwoX1 * xNum;
            NoTr = CosTheta * NoTr + SinTheta * NoBr;
            VoTr = CosTheta * VoTr + SinTheta * VoBr;
        }

        NoL = NoL * CosAlpha + NoTr;
        VoL = VoL * CosAlpha + VoTr;

        float InvLenH = rsqrt(2 + 2 * VoL);
        NoH = saturate((NoL + NoV) * InvLenH);
        VoH = saturate(InvLenH + InvLenH * VoL);
    }
}
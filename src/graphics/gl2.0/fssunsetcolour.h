class SunsetColourGenerator
{
    public:
        void GetColourByAngle(double angle, double dayTime, double &r, double &g, double &b) const;

        void GetColourByAngleAndAlt(double angle, double alt, double dayTime, double &r, double &g, double &b) const;
    private:
        const float dayHorizonR = 0.706;
        const float dayHorizonG = 0.765;
        const float dayHorizonB = 0.843;
        const float dayLowerR = 0.314;
        const float dayLowerG =  0.647;
        const float dayLowerB = 0.902;
        const float dayMiddleR = 0.314;
        const float dayMiddleG = 0.647;
        const float dayMiddleB = 0.902;
        const float dayUpperR =  0.314;
        const float dayUpperG = 0.647;;
        const float dayUpperB = 0.902;
        const float dayZenithR = 0.129;
        const float dayZenithG = 0.310;
        const float dayZenithB = 0.651;

        const float sunsetHorizonR = 0.92;
        const float sunsetHorizonG = 0.756;
        const float sunsetHorizonB = 0.118;
        const float sunsetLowerR = 0.843;
        const float sunsetLowerG = 0.47;
        const float sunsetLowerB = 0.157;
        const float sunsetMiddleR = 0.078;
        const float sunsetMiddleG = 0.250;
        const float sunsetMiddleB = 0.392;
        const float sunsetUpperR = 0.04;
        const float sunsetUpperG = 0.1;
        const float sunsetUpperB = 0.2;
        const float sunsetZenithR = 0.059;
        const float sunsetZenithG = 0.065;
        const float sunsetZenithB = 0.200;
        const float sunsetOpposition;

        const float nightHorizonR = 0.08;
        const float nightHorizonG = 0.15;
        const float nightHorizonB = 0.25;
        const float nightLowerR = 0.08;
        const float nightLowerG = 0.15;
        const float nightLowerB = 0.25;
        const float nightMiddleR = 0.08;
        const float nightMiddleG = 0.15;
        const float nightMiddleB = 0.25;
        const float nightUpperR = 0.08;
        const float nightUpperG = 0.1;
        const float nightUpperB = 0.2;
        const float nightZenithR = 0.04;
        const float nightZenithG = 0.05;
        const float nightZenithB = 0.1;

        
};

float map(float value, float min1, float max1, float min2, float max2)
{
    float perc = (value - min1) / (max1 - min1);
    float result = perc * (max2 - min2) + min2;
    return result;
}
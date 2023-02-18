package literscounter;

/**
 * @author Andrea Ravazzini
 * https://github.com/RavazziniAndrea
 */
public class DataEsp32
{
    int liters;
    int brightness;
    
    public DataEsp32(){}
    public DataEsp32(double liters,int brightness)
    {
        this.liters=(int)liters;
        this.brightness=brightness;
    }

    public double getLiters()
    {
        return liters;
    }

    public void setLiters(double liters)
    {
        this.liters = (int) Math.round(liters);
    }

    public int getBrightness()
    {
        return brightness;
    }

    public void setBrightness(int brightness)
    {
        this.brightness = brightness;
    }
    
}

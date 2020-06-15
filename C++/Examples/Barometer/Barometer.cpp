/*
Provided to you by Emlid Ltd (c) 2014.
twitter.com/emlidtech || www.emlid.com || info@emlid.com

Example: Get pressure from MS5611 barometer onboard of Navio shield for Raspberry Pi

To run this example navigate to the directory containing it and run following commands:
make
./Barometer
*/

#include <Common/MS5611.h>
#include <Common/Util.h>
#include <unistd.h>
#include <stdio.h>
#include <boost/circular_buffer.hpp>
#define C_TO_KELVIN 273.15f

float get_altitude_difference(float base_pressure, float pressure, float temperature) 
{
    float ret;
    float temp    = temperature + C_TO_KELVIN;
    float scaling = pressure / base_pressure;

    // This is an exact calculation that is within +-2.5m of the standard
    // atmosphere tables in the troposphere (up to 11,000 m amsl).
    ret = 153.8462f * temp * (1.0f - expf(0.190259f * logf(scaling)));

    return ret;
}

class AverageFilter 
{
public:
    AverageFilter(int size = 20) : buffer(size){

    }
    float update(float value)
    {
        buffer.push_back(value);
        float sum = 0.0f;
        for(auto v: buffer)
        {
            sum += v;
        }
        return sum/buffer.size();
    }
private:
    boost::circular_buffer<float> buffer;
};

int main()
{
    MS5611 barometer;

    if (check_apm()) {
        return 1;
    }

    barometer.initialize();
    
    AverageFilter filter, base_pressure_filter;
    float base_pressure = -1;
    long  count = 0;
    while (true) {
        barometer.refreshPressure();
        usleep(10000); // Waiting for pressure data ready
        barometer.readPressure();

        barometer.refreshTemperature();
        usleep(10000); // Waiting for temperature data ready
        barometer.readTemperature();

        barometer.calculatePressureAndTemperature();
        if(count < 100)
        {
            base_pressure = barometer.getPressure();
            base_pressure = base_pressure_filter.update(base_pressure);
        }
        float relative_altitude = get_altitude_difference(base_pressure, barometer.getPressure(), barometer.getTemperature());
        float filtered_relative_altitude = filter.update(relative_altitude);
        printf("#%d: Temperature(C): %f Pressure(millibar): %f, relative height(m): %f, filtered: %f\n", count, 
                barometer.getTemperature(), barometer.getPressure(), relative_altitude, filtered_relative_altitude);
                
        //usleep(300000);
        count ++;
    }

    return 0;
}

#include "rodos.h"
#include "hal/hal_adc.h"
#include <cfloat>

// constant for calibration
#define NUM_MEASUREMENTS 10000

// constants for a single measurement
#define ADC_NUM_ITERATIONS 5                   // averages across 5 adc readings
#define SUN_PIN RODOS::ADC_CHANNEL::ADC_CH_006 // PA6
#define ADC_BITS 12
#define ADC_RESOLUTION float((1 << (ADC_BITS - 1)) - 1) 2 ^ ADC_BITS - 1
RODOS::HAL_ADC adcSolarSensor(RODOS::ADC_IDX::ADC_IDX1);

class gl5516Calib : public StaticThread<>
{

public:
    gl5516Calib() : StaticThread("gl5516Calib", 100) {}

    uint16_t calib[2] = {400, 3600};

    uint16_t min_v = UINT16_MAX;
    uint16_t max_v = 0;

    uint16_t measure_sun_sensor()
    {
        uint16_t sun_value = 0;
        for (int i = 0; i < ADC_NUM_ITERATIONS; i++)
        {
            auto a = adcSolarSensor.read(SUN_PIN);
            sun_value += a;
            PRINTF("%d\n", a);
        }
        return sun_value / ADC_NUM_ITERATIONS;
    }

    void init()
    {
        adcSolarSensor.config(RODOS::ADC_PARAMETER_TYPE::ADC_PARAMETER_RESOLUTION, ADC_BITS);
        adcSolarSensor.init(SUN_PIN);
    }

    void run()
    {
        PRINTF("Starting calibration! spin me right round pls :)");
        for (int i = 0; i < 10000; i++)
        {
            // PRINTF("%d\n", i);
            auto value = measure_sun_sensor();
            min_v = min(min_v, value);
            max_v = max(max_v, value);
            AT(NOW() + 10 * MILLISECONDS);
        }
        PRINTF("min,max = {%f,%f}\n", min_v, max_v);
    }

} blinky;
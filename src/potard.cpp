#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <lv2.h>
// #include <iostream>

/**********************************************************************************************************************************************************/

#define MIN(a,b) ( (a) < (b) ? (a) : (b) )
#define MAX(a,b) ( (a) > (b) ? (a) : (b) )
#define RAIL(v, min, max) (MIN((max), MAX((min), (v))))
#define MAP(v, inmin, inmax, outmin, outmax) ((outmin + ((v - inmin)/(inmax - inmin)) * (outmax - outmin)))

#define PLUGIN_URI "http://plujain/plugins/potard"
enum {IN, OUT, POTARD, MINIMUM, MAXIMUM, SHAPE, VOLUME, PLUGIN_PORT_COUNT};

enum {UP, DOWN, STABLE};
/**********************************************************************************************************************************************************/

class Plujain
{
public:
    Plujain() {}
    ~Plujain() {}
    static LV2_Handle instantiate(const LV2_Descriptor* descriptor, double samplerate, const char* bundle_path, const LV2_Feature* const* features);
    static void activate(LV2_Handle instance);
    static void deactivate(LV2_Handle instance);
    static void connect_port(LV2_Handle instance, uint32_t port, void *data);
    static void run(LV2_Handle instance, uint32_t n_samples);
    static void cleanup(LV2_Handle instance);
    static const void* extension_data(const char* uri);
    float *in;
    float *out;
    float *potard;
    float *minimum;
    float *maximum;
    float *shape;
    float *volume;
    float gain;
    double samplerate;
};

/**********************************************************************************************************************************************************/

static const LV2_Descriptor Descriptor = {
    PLUGIN_URI,
    Plujain::instantiate,
    Plujain::connect_port,
    Plujain::activate,
    Plujain::run,
    Plujain::deactivate,
    Plujain::cleanup,
    Plujain::extension_data
};

/**********************************************************************************************************************************************************/

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
    if (index == 0) return &Descriptor;
    else return NULL;
}

/**********************************************************************************************************************************************************/

LV2_Handle Plujain::instantiate(const LV2_Descriptor* descriptor, double samplerate, const char* bundle_path, const LV2_Feature* const* features)
{
    Plujain *plugin = new Plujain();
    
    plugin->samplerate = samplerate;
    plugin->gain = 1.0f;
    return (LV2_Handle)plugin;
}

/**********************************************************************************************************************************************************/

void Plujain::activate(LV2_Handle instance)
{
    // TODO: include the activate function code here
}

/**********************************************************************************************************************************************************/

void Plujain::deactivate(LV2_Handle instance)
{
    // TODO: include the deactivate function code here
}
        

/**********************************************************************************************************************************************************/

void Plujain::connect_port(LV2_Handle instance, uint32_t port, void *data)
{
    Plujain *plugin;
    plugin = (Plujain *) instance;

    switch (port)
    {
        case IN:
            plugin->in = (float*) data;
            break;
        case OUT:
            plugin->out = (float*) data;
            break;
        case POTARD:
            plugin->potard = (float*) data;
            break;
        case MINIMUM:
            plugin->minimum = (float*) data;
            break;
        case MAXIMUM:
            plugin->maximum = (float*) data;
            break;
        case SHAPE:
            plugin->shape = (float*) data;
            break;
        case VOLUME:
            plugin->volume = (float*) data;
            break;
    }
}

/**********************************************************************************************************************************************************/

void Plujain::run(LV2_Handle instance, uint32_t n_samples)
{
    Plujain *plugin;
    plugin = (Plujain *) instance;
    
    
//     float potard_base = minimum + *plugin->potard * (maximum - minimum);
    float potard_base = *plugin->potard;
    
    float rshape = *plugin->shape;
    while (rshape > 1.0f)
    {
        potard_base = sin(M_PI_2 * potard_base);
        rshape -= 1.0;
    }
    
    while (rshape < -1.0f)
    {
        potard_base = sin(M_PI_2 * (potard_base -1)) + 1;
        rshape += 1.0;
    }
    
    float potard;
    
    if (rshape >= 0){
        potard = potard_base + rshape * (sin(M_PI_2 * potard_base) - potard_base);
    } else {
        potard = potard_base - rshape * (sin(M_PI_2 * (potard_base -1)) +1 - potard_base);
    }
    
    float minimum = *plugin->minimum;
    float maximum = *plugin->maximum;
    potard = minimum + potard * (maximum - minimum);
    
    uint32_t movement = STABLE;
    if (plugin->gain > potard){
        movement = DOWN;
    } else if (plugin->gain < potard){
        movement = UP;
    }
    
    for ( uint32_t i = 0; i < n_samples; i++)
    {   
        switch (movement)
        {
            case STABLE:
                break;
            case DOWN:
                plugin->gain -= 0.001;
                if (plugin->gain <= potard){
                    plugin->gain = potard;
                    movement = STABLE;
                }
                break;
            case UP:
                plugin->gain += 0.001;
                if (plugin->gain >= potard){
                    plugin->gain = potard;
                    movement = STABLE;
                }
                break;
        }
        
        plugin->out[i] = plugin->in[i] * plugin->gain;
    }
    *plugin->volume = plugin->gain;
}

/**********************************************************************************************************************************************************/

void Plujain::cleanup(LV2_Handle instance)
{
    delete ((Plujain *) instance);
}

/**********************************************************************************************************************************************************/

const void* Plujain::extension_data(const char* uri)
{
    return NULL;
}

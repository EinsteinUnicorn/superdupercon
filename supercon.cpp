#include "daisy_seed.h"
#include "daisysp.h"
 
using namespace daisy;
using namespace daisysp;
using namespace daisy::seed;
 
DaisySeed hw;

Switch Button1;
bool Button1On;

float pot1, pot2;


//Percussion Noise
Phaser     phaser;


void DeclareInitHardware();

void UpdateKnobs(float &k1, float &k2);

void UpdateButtons(bool &b1);

void Controls();

void GetEffectSample(float &outl, float inl);


void MyCallback(AudioHandle::InterleavingInputBuffer  in,
                AudioHandle::InterleavingOutputBuffer out, 
                size_t size) 
{
  float outl, inl;
  Controls();

  for(size_t i = 0; i < size; i += 2)
  {
    inl = in[i];
    outl = inl;
    if(Button1On){
      GetEffectSample(outl, inl);
    }
    
    out[i] = outl;
    out[i + 1] = outl;
  }

}
 
int main(void) {

  DeclareInitHardware();
  float sample_rate = hw.AudioSampleRate();
  GPIO LED1;
  //D is where LED 1 is located
  LED1.Init(D21, GPIO::Mode::OUTPUT);

  phaser.Init(sample_rate);
  phaser.SetFreq(100.f);
  
 
  while(1) {
    hw.StartAudio(MyCallback);
    LED1.Write(Button1On);
    phaser.SetLfoDepth(pot1);

  }
}

void DeclareInitHardware(){

  // Initialize the Daisy Seed
  hw.Configure();
  hw.Init();

  hw.SetAudioBlockSize(4);

  //D26 is where button 1 is located
  Button1.Init(hw.GetPin(26), 1000); 

  const int num_adc_channels = 2;
  // Create an ADC Channel Config object
  AdcChannelConfig adc_config[num_adc_channels];

  adc_config[0].InitSingle(A0);
  adc_config[1].InitSingle(A1);

  // Initialize the ADC peripheral with that configuration
  hw.adc.Init(adc_config, num_adc_channels);

  hw.adc.Start();

}

// We only have 1 pot on the seed right now, but here's a function that
// can handle both
void UpdateKnobs(float &k1, float &k2)
{
  k1 = hw.adc.GetFloat(0);
  k2 = hw.adc.GetFloat(1);

}

// You can add another bool argument here to have 2 
void UpdateButtons(bool &b1){
  Button1.Debounce();
  b1 = Button1.Pressed();

}

void Controls(){


  UpdateButtons(Button1On);
  UpdateKnobs(pot1, pot2);

}

void GetEffectSample(float &outl, float inl){
  outl = phaser.Process(inl);
}
#include "is31fl3246_output.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace is31fl3246 {

static const char *const TAG = "is31fl3246";

// This is a minimal implementation
// Only supports setting the PWM value of a channel with 8 bit PWM
// No use of diagnostic functionality


// --- Registers ---
// All registers are readable/writable, except reset and update register (0-write triggers functionality)
const uint8_t IS31FL3246_CTL_REG = 0x00;

const uint8_t IS31FL3246_START_HFPWM01L_REG = 0x01;
const uint8_t IS31FL3246_START_HFPWM01H_REG = 0x02;
const uint8_t IS31FL3246_START_LFPWM01_REG = 0x49;

const uint8_t IS31FL3246_UPDATE_REG = 0x6D;
const uint8_t IS31FL3246_GCCR_REG = 0x6E;
const uint8_t IS31FL3246_GCCG_REG = 0x6F;
const uint8_t IS31FL3246_GCCB_REG = 0x70;

const uint8_t IS31FL3246_PH_DLY_REG = 0x71;
const uint8_t IS31FL3246_RESET_REG = 0x7F;

// * marks register defaults
// note that IS31FL3246 initalizes all registers to 0
// 0*: Register auto increment disabled, 1: Register auto increment enabled
const uint8_t IS31FL3246_CTL_SSD = (1 << 0);

// 0*: oscillator clock frequency selection
const uint8_t IS31FL3246_CTL_OSC_8MHZ = (0x00) << 4;
const uint8_t IS31FL3246_CTL_OSC_16MHZ = (0x01) << 4;
const uint8_t IS31FL3246_CTL_OSC_32MHZ = (0x02) << 4;


// 0*: PWM resolution
const uint8_t IS31FL3246_CTL_PWM_8BIT = (0x0) << 1;
const uint8_t IS31FL3246_CTL_PWM_10BIT = (0x1) << 1;

// 0*: RGB MODE
const uint8_t IS31FL3246_CTL_RGB_OFF = (0x0) << 6; //36 channel mode
const uint8_t IS31FL3246_CTL_RGB_ON = (0x1) << 6;

const uint8_t IS31FL3246_FMS_HFP_LFP = 0x00;
const uint8_t IS31FL3246_FMS_HFP_ONLY = 0x01;
const uint8_t IS31FL3246_FMS_ALWAYS_ON = 0x02;
const uint8_t IS31FL3246_FMS_SHUTDOWN = 0x03;

const uint8_t FMS = IS31FL3246_FMS_HFP_LFP;


void IS31FL3246Output::setup() {
  ESP_LOGCONFIG(TAG, "Setting up IS31FL3246OutputComponent...");

  ESP_LOGV(TAG, "  Resetting all devices on the bus...");

  // Reset device
  if (!this->write_byte(IS31FL3246_RESET_REG, 0)) {
    ESP_LOGE(TAG, "RESET failed");
    this->mark_failed();
    return;
  }

  this->enable(true);

  // Set Global Current Control 
  for (int i = 0 ; i < 3 ; i++) {
    if (!this->write_byte(IS31FL3246_GCCR_REG + i, 0xFF)) {
      ESP_LOGE(TAG, "GLOBAL CURRENT failed");
      this->mark_failed();
      return;
    }
  }

  // turn off all LEDs
  //
  for (int channelNo = 0 ; channelNo <  36; channelNo++)
  {    
    this->set_channel_LFPWM(channelNo, 0x00); 
    this->set_channel_HFPWM(channelNo, 0xFF); 
  }   

  // update
  this->update();
  this->loop();
}
/*
 void IS31FL3246A::setHFPWM(uint8_t channel, uint8_t FMS, uint8_t HFPWMLevel_H, uint8_t HFPWMLevel_L)
 {
     if(channel >= 1 && channel <= 36) {
     _i2c_bus->writeByte(IS31FL3246A_ADDRESS, (2*channel - 1), HFPWMLevel_L);  // set lower bytes of HF PWM level (0 - 255) for specified led channel
     _i2c_bus->writeByte(IS31FL3246A_ADDRESS, (2*channel), (FMS << 2) | HFPWMLevel_H);  // set freq control mode and upper 2 bits of HF PWM level for specified led channel
     }
 }


 void IS31FL3246A::setLFPWM(uint8_t channel, uint8_t PWMLevel)
 {
     if(channel >= 1 && channel <= 36) {
     _i2c_bus->writeByte(IS31FL3246A_ADDRESS, channel + 0x48, PWMLevel);  // set LF PWM level (0 - 255) for specified led channel
     }
 }
*/

bool IS31FL3246Output::set_channel_HFPWM(uint8_t channelNo, uint16_t pwm) {
  uint8_t reg =  IS31FL3246_START_HFPWM01L_REG + (channelNo*2);
  uint8_t lowVal =  pwm & 0xFF;
  uint8_t highVal = (pwm >> 8) & 0x03;
  highVal |= (FMS << 2);

  if (!this->write_byte(reg, lowVal)) {
    this->status_set_warning();
    return false;
  }
  else
  {
    if (!this->write_byte(reg + 1 , highVal)) {
      this->status_set_warning();
      return false;
    }
  }
  
  return true;
}

bool IS31FL3246Output::set_channel_LFPWM(uint8_t channelNo, uint8_t pwm) {
  uint8_t reg =  IS31FL3246_START_LFPWM01_REG + channelNo;

  // this only sets the low value of PWM as 8 bit is the only resolution we support
  if (!this->write_byte(reg, pwm)) {
    this->status_set_warning();
    return false;
  }
  return true;
}

void IS31FL3246Output::update() {
  if (!this->write_byte(IS31FL3246_UPDATE_REG, 0x00)) {
    ESP_LOGE(TAG, "UPDATE failed");
    this->mark_failed();
    return;
  }
}

void IS31FL3246Output::enable(bool enable) {
  if(enable)
  {
    // Enable device
    if (!this->write_byte(IS31FL3246_CTL_REG, IS31FL3246_CTL_SSD)) {
      ESP_LOGE(TAG, "ENABLE failed");
      this->mark_failed();
      return;
    }
  }
  else
  {
    // Disable device
    if (!this->write_byte(IS31FL3246_CTL_REG, 0)) {
      ESP_LOGE(TAG, "DISABLE failed");
      this->mark_failed();
      return;
    }
  }
}

void IS31FL3246Output::dump_config() {
  ESP_LOGCONFIG(TAG, "IS31FL3246:");
  ESP_LOGCONFIG(TAG, "  dump config to be implemented");

  if (this->is_failed()) {
    ESP_LOGE(TAG, "Setting up IS31FL3246 failed!");
  }
}

void IS31FL3246Output::loop() {
  if (this->min_channel_ == 0xFF || !this->update_)
    return;

  for (uint8_t channel = this->min_channel_; channel <= this->max_channel_; channel++) {
    
    uint8_t pwm = this->pwm_amounts_[channel];
    if(pwm != this->pwm_amounts_[36+channel])
    {
      ESP_LOGV(TAG, "Channel %02u: pwm=%02u ", channel, pwm);
    
      this->set_channel_LFPWM(channel, pwm);
      //this->set_channel_HFPWM(channel, pwm);
      this->pwm_amounts_[36+channel] = pwm;
    }
/*
    uint8_t reg = this->get_PWM_reg_for_channel(channel);
    if (!this->write_byte(reg, pwm)) {
      this->status_set_warning();
      return;
    }
*/
  }
  this->update();
  this->status_clear_warning();
  this->update_ = false;
}

void IS31FL3246Output::register_channel(IS31FL3246Channel *channel) {
  auto c = channel->channel_;
  this->min_channel_ = std::min(this->min_channel_, c);
  this->max_channel_ = std::max(this->max_channel_, c);
  channel->set_parent(this);
}

void IS31FL3246Channel::write_state(float state) {
  const uint8_t max_duty = 255;
  const float duty_rounded = roundf(state * max_duty);
  auto duty = static_cast<uint8_t>(duty_rounded);
  this->parent_->set_channel_value_(this->channel_, duty);
}

}  // namespace is31fl3246
}  // namespace esphome

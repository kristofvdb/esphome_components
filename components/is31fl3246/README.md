# is31fl3246

## About
This library implements rudimentary support for is31fl3246. It needs I2C support.
It is largely based on the code for TLC59208F and should be similar in terms of functionality

future improvements:
- ... 
 
## Configuration

example configuration:

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/kristofvdb/esphome_components
    components: [ is31fl3246 ]

is31fl3246:
  - address: 0x30
    id: is31fl3246_1

# example of two RGB lights on 2 different IC's

# expose the output channels
output:
  - platform: is31fl3246
    id: 'is31fl3246_1_ch0'
    channel: 0
    is31fl3246_id: 'is31fl3246_1'
  - platform: is31fl3246
    id: 'is31fl3246_1_ch1'
    channel: 1
    is31fl3246_id: 'is31fl3246_1'
  - platform: is31fl3246
    id: 'is31fl3246_1_ch2'
    channel: 2
    is31fl3246_id: 'is31fl3246_1'

# example of one RGB lights
# sidenote: grouping can be done on a higher level
#  by creating "light groups" (helper) 
light:
  - platform: rgb
    name: "Led2"
    red: is31fl3246_1_ch1
    green: is31fl3246_1_ch0
    blue: is31fl3246_1_ch2
    id: led2
```

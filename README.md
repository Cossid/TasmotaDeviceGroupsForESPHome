# Tasmota Device Groups for ESPHome
This repository is for an external component for ESPHome to add (limited) Tasmota device groups compatibility.  Since the majority of the code is taken directly from Tasmota, the license from Tasmota (GNU GPL v3.0) also follows.

# Configuration
Configuration for ESPHome is opt-in instead of automatic.  The reason for this is detached entities are not a special flag, and we don't want to attach to anything meant to be detached.

```yaml
external_components:
  - source: github://cossid/tasmotadevicegroupsforesphome@main
    components: [ device_groups ]
    refresh: 10 min

device_groups:
  - id: "testgroup" # devgroupname
    switches:
      - gpio_switch # ESPHome id
      - template_switch
    lights:
      - light_rgbww1
      - light_rgbww2
  - id: "testdgr2" # devgroupname
    switches:
      - gpio_switch2
```

### Supported:
* Power State
* Light Ste
  * On/Off
  * Brightness
  * Color channels

### Not yet supported
* Send/Recieve masking
* Fade/Transitions/Speed
* Schemes
* Commands (ESPHome doesn't have a direct equivalent)

### Misc:
You may see a notice about blocking for too long.  This should not really be a problem, it is a generic ESPHome notice about performance.  Delays are mostly caused by waiting on network traffic.  The notice will look like this:
```
[W][component:204]: Component device_groups took a long time for an operation (0.15 s).
[W][component:205]: Components should block for at most 20-30ms.
```

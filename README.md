# Tasmota Device Groups for ESPHome

This repository is for an external component for ESPHome to add (limited) Tasmota device groups compatibility.  Since the majority of the code is taken directly from Tasmota, the license from Tasmota (GNU GPL v3.0) also follows.

## Configuration

Configuration for ESPHome is opt-in instead of automatic.  The reason for this is detached entities are not a special flag, and we don't want to attach to anything meant to be detached.

```yaml
external_components:
  - source: github://cossid/tasmotadevicegroupsforesphome@main
    components: [ device_groups ]
    refresh: 10 min

device_groups:
  - group_name: "testgroup1"         # Tasmota device group name
    send_mask: 0xFFFFFFFF    # Optional, defaults to 0xFFFFFFFF (send everything).  Can be integer or hex
    receive_mask: 0xFFFFFFFF # Optional, defaults to 0xFFFFFFFF (receive everything).  Can be integer or hex
    switches:
      - gpio_switch          # ESPHome entity id
      - template_switch      # ESPHome entity id
    lights:
      - light_rgbww1         # ESPHome entity id
      - light_rgbww2         # ESPHome entity id
  - group_name: "testgroup2"         # Tasmota device group name
    switches:
      - gpio_switch2         # ESPHome entity id
```

### Send/Receive masking

Masks can be set as integer or hex values.  Integer will work better when you want specific combinations, hex will work better when you want all categories set to be processed.

```yaml
0 / 0x0 = Deny all
1 = Power
2 = Light brightness
4 = Light fade/speed
8 = Light scheme
16 = Light color
32 = Dimmer settings (presets)
64 = Event
127 / 0x7F = Process all (shorthand, subject to expansion)
4294967295 / 0xFFFFFFFF = Process all (default)
```

If you want combinations, you just add them together, same as Tasmota.  For example, Power + Light Color = 17

### Supported

* Power States
* Light States
  * On/Off
  * Brightness (color_interlock: true required for RGBW, or you will not get RGB brightness control)
  * Color channels
* Send/Receive masking

### Not yet supported

* Color Brightness on RGBW lights without color_interlock
* Fade/Transitions/Speed
* Schemes
* Commands (ESPHome doesn't have a direct equivalent)
  * Similar can be accomplished with template devices, see [Command alternative](#command-alternative) below

### Command alternative

Since commands are not implemented, the suggested workaround is to make internal matching entites backed by templates to what you want to change, add that internal entity to your group, and set the desired state on that entity.

For example, if you have a switch with a button and would like to control a light entity, you could use:

```yaml
output:
  - platform: template
    id: dummy_output
    type: float
    write_action:
      - lambda: return;

light:
  - platform: rgbww
    id: internal_light
    color_interlock: true
    cold_white_color_temperature: 6500 K
    warm_white_color_temperature: 2700 K
    red: dummy_output
    green: dummy_output
    blue: dummy_output
    cold_white: dummy_output
    warm_white: dummy_output

button:
  - platform: template
    name: "Set light to red"
    on_press:
      - light.control:
          id: internal_light
          state: on
          red: 100%
          green: 0%
          blue: 0%

device_groups:
  - group_name: "light_group"    # Tasmota device group name
    lights:
      - internal_light   # ESPHome entity id
```

Button is just an example, but you could hook into any of the `on_` events for `binary_sensor`, `button`, `switch`, etc.

## Framework Support

### ESP-IDF Support

This component now includes full ESP-IDF support through a custom WiFiUDP implementation (`device_groups_WiFiUdp.h` and `device_groups_WiFiUdp.cpp`). The implementation provides ESPHome-compatible UDP functionality using ESP-IDF's native socket API, allowing the component to work seamlessly with ESP-IDF without requiring Arduino framework dependencies.

**Features of the ESP-IDF WiFiUDP implementation:**
- Full ESPHome WiFiUDP API compatibility
- Multicast support for device group communication
- Native ESP-IDF socket operations
- Automatic conditional compilation (uses local WiFiUdp.h when `USE_ESP_IDF` is defined)

**ESP-IDF Compilation Fix:**
- Fixed `ifaddrs.h` dependency issue by replacing `getifaddrs()` with ESP-IDF's `esp_netif_get_ip_info()` API
- Added proper ESP-IDF includes (`esp_wifi.h`, `esp_netif.h`) with conditional compilation
- Updated `localIP()` method to use ESP-IDF network interface API instead of POSIX ifaddrs

**Troubleshooting ESP-IDF Issues:**
If you experience intermittent behavior with ESP-IDF builds, the enhanced logging will help identify the root cause:
- Check for "Network not ready" messages indicating WiFi connectivity issues
- Look for "Socket error detected" messages indicating socket state problems
- Monitor "Socket would block" messages during high network load
- Verify buffer allocation success in memory-constrained environments

### Arduino Framework Support

The component continues to support Arduino-based frameworks (ESP32 Arduino, ESP8266 Arduino) using the standard WiFiUDP libraries.

### Known Issues

* Multiple relays in the same group cannot currently be individually addressed, so an action against the group will apply to all entities in the group on ESPHome.  [Issue #2](https://github.com/Cossid/TasmotaDeviceGroupsForESPHome/issues/2) will track the potential resolution for this.  As a workaround, you can enable SetOption88 on Tasmota and assign individual groups.  While Tasmota by default is limited to 4 groups, ESPHome has no limit.
* ESPHome handles brightness between RGB and White channels differently, and both modes cannot be supported at the same time.  As a result, RGB brightness cannot currently be supported for RGBW bulbs without color_interlock.

### Misc

You may see a notice about blocking for too long.  This should not really be a problem, it is a generic ESPHome notice about performance.  Delays are mostly caused by waiting on network traffic.  The notice will look like this:

```text
[W][component:204]: Component device_groups took a long time for an operation (0.15 s).
[W][component:205]: Components should block for at most 20-30ms.
```

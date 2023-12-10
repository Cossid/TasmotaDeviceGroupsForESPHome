#pragma once

#include "esphome/core/application.h"
#include "esphome/core/component.h"
#include <vector>
#include "esphome/components/network/ip_address.h"

#if defined(USE_ESP32)
#include <esp_wifi.h>
#include <WiFiUdp.h>
#elif USE_ESP8266
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#else
#include <WiFi.h>
#endif

#ifdef USE_SWITCH
#include "esphome/components/switch/switch.h"
#endif
#ifdef USE_LIGHT
#include "esphome/components/light/light_state.h"
#include "esphome/components/light/color_mode.h"
#endif

namespace esphome {
namespace device_groups {

// #define DEVICE_GROUPS_DEBUG
#define DGR_MULTICAST_REPEAT_COUNT 1              // Number of times to re-send each multicast
#define DGR_ACK_WAIT_TIME 150                     // Initial ms to wait for ack's
#define DGR_MEMBER_TIMEOUT 45000                  // ms to wait for ack's before removing a member
#define DGR_ANNOUNCEMENT_INTERVAL 60000           // ms between announcements
#define DEVICE_GROUP_MESSAGE "TASMOTA_DGR"
#define DEVICE_GROUPS_ADDRESS 239, 255, 250, 250  // Device groups multicast address
#define DEVICE_GROUPS_PORT 4447                   // Device groups multicast port
#define USE_DEVICE_GROUPS_SEND                    // Add support for the DevGroupSend command (+0k6 code)
#define D_CMND_DEVGROUPSTATUS "DevGroupStatus"

const uint8_t MAX_DEV_GROUP_NAMES = 4;  // Max number of Device Group names
const uint16_t TOPSZ = 151;             // Max number of characters in topic string
const char kDeviceGroupMessage[] = DEVICE_GROUP_MESSAGE;

typedef uint32_t power_t;                  // Power (Relay) type
const uint32_t POWER_MASK = 0xFFFFFFFFUL;  // Power (Relay) full mask
const uint32_t POWER_SIZE = 32;            // Power (relay) bit count
const uint8_t SET_DEV_GROUP_NAME1 = 72;

enum DevGroupState { DGR_STATE_UNINTIALIZED, DGR_STATE_INITIALIZING, DGR_STATE_INITIALIZED };

enum DevGroupMessageType {
  DGR_MSGTYP_FULL_STATUS,
  DGR_MSGTYP_PARTIAL_UPDATE,
  DGR_MSGTYP_UPDATE,
  DGR_MSGTYP_UPDATE_MORE_TO_COME,
  DGR_MSGTYP_UPDATE_DIRECT,
  DGR_MSGTYPE_UPDATE_COMMAND,
  DGR_MSGTYPFLAG_WITH_LOCAL = 128
};

enum DevGroupMessageFlag {
  DGR_FLAG_RESET = 1,
  DGR_FLAG_STATUS_REQUEST = 2,
  DGR_FLAG_FULL_STATUS = 4,
  DGR_FLAG_ACK = 8,
  DGR_FLAG_MORE_TO_COME = 16,
  DGR_FLAG_DIRECT = 32,
  DGR_FLAG_ANNOUNCEMENT = 64,
  DGR_FLAG_LOCAL = 128
};

enum DevGroupItem {
  DGR_ITEM_EOL,
  DGR_ITEM_STATUS,
  DGR_ITEM_FLAGS,
  DGR_ITEM_LIGHT_FADE,
  DGR_ITEM_LIGHT_SPEED,
  DGR_ITEM_LIGHT_BRI,
  DGR_ITEM_LIGHT_SCHEME,
  DGR_ITEM_LIGHT_FIXED_COLOR,
  DGR_ITEM_BRI_PRESET_LOW,
  DGR_ITEM_BRI_PRESET_HIGH,
  DGR_ITEM_BRI_POWER_ON,
  // Add new 8-bit items before this line
  DGR_ITEM_LAST_8BIT,
  DGR_ITEM_MAX_8BIT = 63,
  // DGR_ITEM_ANALOG1, DGR_ITEM_ANALOG2, DGR_ITEM_ANALOG3, DGR_ITEM_ANALOG4, DGR_ITEM_ANALOG5,
  //  Add new 16-bit items before this line
  DGR_ITEM_LAST_16BIT,
  DGR_ITEM_MAX_16BIT = 127,
  DGR_ITEM_POWER,
  DGR_ITEM_NO_STATUS_SHARE,
  // Add new 32-bit items before this line
  DGR_ITEM_LAST_32BIT,
  DGR_ITEM_MAX_32BIT = 191,
  DGR_ITEM_EVENT,
  DGR_ITEM_COMMAND,
  // Add new string items before this line
  DGR_ITEM_LAST_STRING,
  DGR_ITEM_MAX_STRING = 223,
  DGR_ITEM_LIGHT_CHANNELS
};

enum DevGroupItemFlag { DGR_ITEM_FLAG_NO_SHARE = 1 };

enum DevGroupShareItem {
  DGR_SHARE_POWER = 1,
  DGR_SHARE_LIGHT_BRI = 2,
  DGR_SHARE_LIGHT_FADE = 4,
  DGR_SHARE_LIGHT_SCHEME = 8,
  DGR_SHARE_LIGHT_COLOR = 16,
  DGR_SHARE_DIMMER_SETTINGS = 32,
  DGR_SHARE_EVENT = 64
};

enum XsnsFunctions { FUNC_DEVICE_GROUP_ITEM = 41 };

enum ExecuteCommandPowerOptions {
  POWER_OFF,
  POWER_ON,
  POWER_TOGGLE,
  POWER_BLINK,
  POWER_BLINK_STOP,
  POWER_OFF_NO_STATE = 8,
  POWER_ON_NO_STATE,
  POWER_TOGGLE_NO_STATE,
  POWER_SHOW_STATE = 16
};

enum CommandSource {
  SRC_IGNORE,
  SRC_MQTT,
  SRC_RESTART,
  SRC_BUTTON,
  SRC_SWITCH,
  SRC_BACKLOG,
  SRC_SERIAL,
  SRC_WEBGUI,
  SRC_WEBCOMMAND,
  SRC_WEBCONSOLE,
  SRC_PULSETIMER,
  SRC_TIMER,
  SRC_RULE,
  SRC_MAXPOWER,
  SRC_MAXENERGY,
  SRC_OVERTEMP,
  SRC_LIGHT,
  SRC_KNX,
  SRC_DISPLAY,
  SRC_WEMO,
  SRC_HUE,
  SRC_RETRY,
  SRC_REMOTE,
  SRC_SHUTTER,
  SRC_THERMOSTAT,
  SRC_CHAT,
  SRC_TCL,
  SRC_BERRY,
  SRC_FILE,
  SRC_SSERIAL,
  SRC_USBCONSOLE,
  SRC_SO47,
  SRC_MAX
};

enum LoggingLevels {
  LOG_LEVEL_NONE,
  LOG_LEVEL_ERROR,
  LOG_LEVEL_INFO,
  LOG_LEVEL_DEBUG,
  LOG_LEVEL_DEBUG_MORE,
  LOG_LEVEL_ALL
};

enum ProcessGroupMessageResult {
  PROCESS_GROUP_MESSAGE_ERROR,
  PROCESS_GROUP_MESSAGE_SUCCESS,
  PROCESS_GROUP_MESSAGE_UNMATCHED
};

struct device_group_member {
  struct device_group_member *flink;
  IPAddress ip_address;
  uint16_t received_sequence;
  uint16_t acked_sequence;
  uint32_t unicast_count;
};

struct device_group {
  uint32_t next_announcement_time;
  uint32_t next_ack_check_time;
  uint32_t member_timeout_time;
  uint32_t no_status_share;
  uint16_t outgoing_sequence;
  uint16_t last_full_status_sequence;
  uint16_t message_length;
  uint16_t ack_check_interval;
  uint8_t message_header_length;
  uint8_t initial_status_requests_remaining;
  uint8_t multicasts_remaining;
  char group_name[TOPSZ];
  uint8_t message[128];
  struct device_group_member *device_group_members;
#ifdef USE_DEVICE_GROUPS_SEND
  uint8_t values_8bit[DGR_ITEM_LAST_8BIT];
  uint16_t values_16bit[DGR_ITEM_LAST_16BIT - DGR_ITEM_MAX_8BIT - 1];
  uint32_t values_32bit[DGR_ITEM_LAST_32BIT - DGR_ITEM_MAX_16BIT - 1];
#endif  // USE_DEVICE_GROUPS_SEND
};

struct TasmotaGlobal_t {
  bool skip_light_fade = false;  // Temporarily skip light fading
  uint8_t devices_present = 20;  // Max number of devices supported
  power_t power;                 // Current copy of Settings->power
  uint8_t restart_flag = 0;      // Tasmota restart flag
  int32_t fade = -1;
  int32_t speed = -1;
  int32_t scheme = -1;
  bool processing_light_transition = false;
};

struct XDRVMAILBOX {
  bool grpflg;
  bool usridx;
  uint16_t command_code;
  uint32_t index;
  uint32_t data_len;
  int32_t payload;
  char *topic;
  char *data;
  char *command;
};

typedef union {
  uint32_t data;                          // Allow bit manipulation using SetOption
  struct {                                // SetOption82 .. SetOption113
    uint32_t device_groups_enabled : 1;   // bit 3 (v8.1.0.9)   - SetOption85  - (DevGroups) Enable Device Groups (1)
    uint32_t multiple_device_groups : 1;  // bit 6 (v8.1.0.9)   - SetOption88  - (DevGroups) Enable relays in separate
                                          // device groups/PWM Dimmer Buttons control remote devices (1)
  };
} SOBitfield4;

typedef struct {
  SOBitfield4 flag4;                // EF8
  uint8_t device_group_tie[4];      // FB0
  uint32_t device_group_share_in;   // FCC  Bitmask of device group items imported
  uint32_t device_group_share_out;  // FD0  Bitmask of device group items exported
} TSettings;

struct multicast_packet {
  uint32_t id;
  int length;
  uint8_t payload[512];
  IPAddress remoteIP;
};

#if defined(ESP8266)
static WiFiUDP device_groups_udp;
static std::vector<multicast_packet> received_packets{};
static std::vector<std::string> registered_group_names{};
static uint32_t packetId = 0;
#endif

class device_groups : public Component {
 public:
  void register_device_group_name(std::string group_name) { this->device_group_name_ = std::move(group_name); }
#ifdef USE_SWITCH
  void register_switches(const std::vector<switch_::Switch *> &switches) { this->switches_ = switches; }
#endif
#ifdef USE_LIGHT
  void register_lights(const std::vector<light::LightState *> &lights) { this->lights_ = lights; }
#endif
  void register_send_mask(uint32_t send_mask) { this->send_mask_ = send_mask; }
  void register_receive_mask(uint32_t receive_mask) { this->receive_mask_ = receive_mask; }
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::AFTER_WIFI; }

  /// Send new values if they were updated.
  void loop() override;

 protected:
  void SendReceiveDeviceGroupMessage(struct device_group *device_group, struct device_group_member *device_group_member,
                                     uint8_t *message, int message_length, bool received);
  bool _SendDeviceGroupMessage(int32_t device, DevGroupMessageType message_type, ...);
#define SendDeviceGroupMessage(DEVICE_INDEX, REQUEST_TYPE, ...) \
  _SendDeviceGroupMessage(DEVICE_INDEX, REQUEST_TYPE, ##__VA_ARGS__, 0)
  ProcessGroupMessageResult ProcessDeviceGroupMessage(multicast_packet);
  bool XdrvCall(uint8_t Function);
  void ExecuteCommandPower(uint32_t device, uint32_t state, uint32_t source);
  void ExecuteCommand(const char *cmnd, uint32_t source);
  void InitTasmotaCompatibility();
  void DeviceGroupsInit();
  bool DeviceGroupsStart();
  void DeviceGroupsLoop();
  void DeviceGroupsStop();
  void DeviceGroupStatus(uint8_t device_group_index);

  std::string device_group_name_;
  bool update_{true};
  uint32_t send_mask_{0xffffffff};
  uint32_t receive_mask_{0xffffffff};

#ifdef USE_SWITCH
  std::vector<switch_::Switch *> switches_{};
#endif
#ifdef USE_LIGHT
  std::vector<light::LightState *> lights_{};
#endif

#if !defined(ESP8266)
  WiFiUDP device_groups_udp;
#endif
  struct device_group *device_groups_;
  uint32_t next_check_time;
  bool device_groups_initialized = false;
  bool device_groups_up = false;
  bool building_status_message = false;
  bool ignore_dgr_sends = false;
  TSettings *Settings = nullptr;
  TasmotaGlobal_t TasmotaGlobal;
  XDRVMAILBOX XdrvMailbox;
  DevGroupState dgr_state = DGR_STATE_UNINTIALIZED;
  bool setup_complete = false;

  uint8_t device_group_count = 0;
  bool first_device_group_is_local = true;

#ifdef USE_LIGHT
  void get_light_values(light::LightState *obj, bool &power_state, float &brightness, float &red, float &green, float &blue, float &cold_white, float &warm_white, esphome::light::ColorMode &color_mode);
  void set_light_intial_values(light::LightState *obj);
  bool previous_power_state = false;
  float previous_brightness = 0.0f;
  float previous_red = 0.0f;
  float previous_green = 0.0f;
  float previous_blue = 0.0f;
  float previous_warm_white = 0.0f;
  float previous_cold_white = 0.0f;
  float previous_color_temperature = 0.0f;
  esphome::light::ColorMode previous_color_mode = esphome::light::ColorMode::UNKNOWN;
#endif
};

}  // namespace device_groups
}  // namespace esphome

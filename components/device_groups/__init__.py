from esphome.const import (
    CONF_ID,
)
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch, light
from esphome.core import CORE

CODEOWNERS = ["@Cossid"]
DEPENDENCIES = ["network"]

device_groups_ns = cg.esphome_ns.namespace("device_groups")
device_groups = device_groups_ns.class_("device_groups", cg.Component)

MULTI_CONF = True
CONF_GROUP_NAME = "group_name"
CONF_SWITCHES = "switches"
CONF_LIGHTS = "lights"
CONF_SEND_MASK = "send_mask"
CONF_RECEIVE_MASK = "receive_mask"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ID): cv.declare_id(device_groups),
        cv.Required(CONF_GROUP_NAME): cv.string,
        cv.Optional(CONF_SWITCHES): cv.All(cv.ensure_list(cv.use_id(switch.Switch)), cv.Length(min=1)),
        cv.Optional(CONF_LIGHTS): cv.All(cv.ensure_list(cv.use_id(light.LightState)), cv.Length(min=1)),
        cv.Optional(CONF_SEND_MASK, default=0xFFFFFFFF): cv.hex_uint32_t,
        cv.Optional(CONF_RECEIVE_MASK, default=0xFFFFFFFF): cv.hex_uint32_t,
    }, cv.has_at_least_one_key(CONF_SWITCHES, CONF_LIGHTS)
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    cg.add(var.register_device_group_name(config[CONF_GROUP_NAME]))
    cg.add(var.register_send_mask(config[CONF_SEND_MASK]))
    cg.add(var.register_receive_mask(config[CONF_RECEIVE_MASK]))

    if CONF_SWITCHES in config:
        switches = []
        for switch in config[CONF_SWITCHES]:
            new_switch = await cg.get_variable(switch)
            switches.append(new_switch)
        cg.add(var.register_switches(switches))

    if CONF_LIGHTS in config:
        lights = []
        for light in config[CONF_LIGHTS]:
            new_light = await cg.get_variable(light)
            lights.append(new_light)
        cg.add(var.register_lights(lights))

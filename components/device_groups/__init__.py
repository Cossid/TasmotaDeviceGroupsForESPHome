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
#CONF_ITEMS = "items"
CONF_SWITCHES = "switches"
CONF_LIGHTS = "lights"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_ID): cv.declare_id(device_groups),
        # cv.Required(CONF_ITEMS): cv.All(
        #     cv.ensure_list(cv.use_id(cv.Any(switch.Switch, light.LightState))), cv.Length(min=1)
        # ),
        cv.Optional(CONF_SWITCHES): cv.All(cv.ensure_list(cv.use_id(switch.Switch)), cv.Length(min=1)),
        cv.Optional(CONF_LIGHTS): cv.All(cv.ensure_list(cv.use_id(light.LightState)), cv.Length(min=1)),
    }, cv.has_at_least_one_key(CONF_SWITCHES, CONF_LIGHTS)
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    cg.add(var.set_device_group_name(str(var)))

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
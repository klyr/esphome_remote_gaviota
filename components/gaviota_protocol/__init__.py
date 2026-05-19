import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ADDRESS, CONF_CHANNEL, CONF_COMMAND
from esphome.components.remote_base import declare_protocol, register_dumper, register_action, register_trigger, register_binary_sensor

# Gaviota
GaviotaData, GaviotaBinarySensor, GaviotaTrigger, GaviotaAction, GaviotaDumper = declare_protocol(
    "Gaviota"
)

GAVIOTA_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_ADDRESS): cv.hex_int_range(0, 0x0FFFFFFF),  # 28 bits
        cv.Required(CONF_CHANNEL): cv.hex_int_range(0, 0x0F),         # 4 bits
        cv.Required(CONF_COMMAND): cv.hex_int_range(0, 0xFF),         # 8 bits
    }
)

CONFIG_SCHEMA = cv.COMPONENT_SCHEMA


@register_binary_sensor("gaviota", GaviotaBinarySensor, GAVIOTA_SCHEMA)
def gaviota_binary_sensor(var, config):
    cg.add(
        var.set_data(
            cg.StructInitializer(
                GaviotaData,
                ("address", config[CONF_ADDRESS]),
                ("channel", config[CONF_CHANNEL]),
                ("command", config[CONF_COMMAND]),
            )
        )
    )


@register_trigger("gaviota", GaviotaTrigger, GaviotaData)
def gaviota_trigger(var, config):
    pass


@register_dumper("gaviota", GaviotaDumper)
def gaviota_dumper(var, config):
    pass


@register_action("gaviota", GaviotaAction, GAVIOTA_SCHEMA)
async def gaviota_action(var, config, args):
    template_ = await cg.templatable(config[CONF_ADDRESS], args, cg.uint32)
    cg.add(var.set_address(template_))
    template_ = await cg.templatable(config[CONF_CHANNEL], args, cg.uint8)
    cg.add(var.set_channel(template_))
    template_ = await cg.templatable(config[CONF_COMMAND], args, cg.uint8)
    cg.add(var.set_command(template_))

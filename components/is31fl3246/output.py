import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import output
from esphome.const import CONF_CHANNEL, CONF_ID
from . import IS31FL3246Output, is31fl3246_ns

DEPENDENCIES = ["is31fl3246"]

IS31FL3246Channel = is31fl3246_ns.class_("IS31FL3246Channel", output.FloatOutput)
CONF_IS31FL3246_ID = "is31fl3246_id"

CONFIG_SCHEMA = output.FLOAT_OUTPUT_SCHEMA.extend(
    {
        cv.Required(CONF_ID): cv.declare_id(IS31FL3246Channel),
        cv.GenerateID(CONF_IS31FL3246_ID): cv.use_id(IS31FL3246Output),
        cv.Required(CONF_CHANNEL): cv.int_range(min=0, max=35),
    }
)


async def to_code(config):
    paren = await cg.get_variable(config[CONF_IS31FL3246_ID])
    var = cg.new_Pvariable(config[CONF_ID])
    cg.add(var.set_channel(config[CONF_CHANNEL]))
    cg.add(paren.register_channel(var))
    await output.register_output(var, config)

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from esphome.const import CONF_TIME

DEPENDENCIES = ["time"]

ntp_server_info_ns = cg.esphome_ns.namespace("ntp_server_info")

NTPServoInfo = ntp_server_info_ns.class_(
    "NTPServoInfo", text_sensor.TextSensor, cg.PollingComponent
)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.Optional(CONF_TIME): text_sensor.text_sensor_schema(NTPServoInfo)
        .extend(cv.polling_component_schema("20s"))
    }
)

async def setup_conf(config, key):
    if key in config:
        conf = config[key]
        var = await text_sensor.new_text_sensor(conf)
        await cg.register_component(var, conf)

async def to_code(config):
    await setup_conf(config, CONF_TIME)
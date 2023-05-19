import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_TIME

CODEOWNERS = ["@RobertJN64"]
#DEPENDENCIES = ["time"] #allow for use of ntp_time instead for more features

ntp_server_ns = cg.esphome_ns.namespace("ntp_server")
NTP_Server = ntp_server_ns.class_("NTP_Server", cg.Component)

CONF_GPS_ID = "ntp_server_id"
CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(NTP_Server),
        }
    )
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    cg.add_library("PaulStoffregen/Time", "1.6.1")
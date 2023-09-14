DESCRIPTION = "weather App"
LICENSE = "CLOSED"
PR = "r1"

FILESEXTRAPATHS:prepend := "${THISDIR}/TFG:"
SRC_URI = "file://TFG"

DEPENDS += " \
          jsoncpp \
          paho-mqtt-cpp \
    "

do_install() {
    install -d ${D}${sysconfdir}/weatherApp

    install -m 0755    ${WORKDIR}/TFG ${D}${sysconfdir}/weatherApp
}

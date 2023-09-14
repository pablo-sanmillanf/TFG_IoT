FILESEXTRAPATHS:prepend := "${THISDIR}/initscripts:"

SRC_URI += " file://weatherApp "

do_install:append() {
    install -d ${D}${sysconfdir}/init.d

    # The extra files need to go in the respective directories
    install -m 0755    ${WORKDIR}/weatherApp       ${D}${sysconfdir}/init.d

    # Create the symbolic links
    update-rc.d -r ${D} weatherApp defaults 70
}


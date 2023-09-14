do_install:append() {
    echo "local1.* /var/log/weatherApp.log" >> ${D}/${sysconfdir}/syslog.conf
}

do_install:append() {
    sed -i 's@# server time.server.example.com@server pool.ntp.org \
server 0.pool.ntp.org \
server 1.pool.ntp.org \
server 2.pool.ntp.org \
server 3.pool.ntp.org@g' ${D}/${sysconfdir}/ntp.conf
}


do_deploy:append() {
   echo "gpio=25=pu" >>"${DEPLOYDIR}/bootfiles/config.txt"
   echo "gpio=24=pu" >>"${DEPLOYDIR}/bootfiles/config.txt"
   echo "gpio=23=pu" >>"${DEPLOYDIR}/bootfiles/config.txt"
   echo "gpio=18=pu" >>"${DEPLOYDIR}/bootfiles/config.txt"
   echo "gpio=7=pu" >>"${DEPLOYDIR}/bootfiles/config.txt"
   echo "dtoverlay=spi0-1cs" >>"${DEPLOYDIR}/bootfiles/config.txt"
}


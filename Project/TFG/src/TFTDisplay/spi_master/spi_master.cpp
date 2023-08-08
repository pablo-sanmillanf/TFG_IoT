#include "spi_master.h"
#include "../../custom_gpio/custom_gpio.h"

static int fd;

static CustomGPIO::GPIO gpio17(17);
static bool previous_cmd = false;

struct spi_ioc_transfer tr;

/**
 * @brief Starts the SPI device and configures it to allow SPI comunnications.
 * 
 * @param[in] spi_device Indicates which SPI device of type spidev0.<number> 
 *                       within the /dev/ directory will be used.
 *
 * @return 0 if success, -1 if error.
 */
int SPI_Master::spi_start (int spi_device, int mode, int bits, int speed) {

	int ret = 0;

	if(fd == 0){
	//Open file descriptor
	char spiFile[15];
	sprintf(spiFile, "/dev/spidev0.%d", spi_device);

	fd = open(spiFile, O_RDWR);

	if(fd == -1){
	  return -1;
	}
	}

	ret = ioctl(fd, SPI_IOC_WR_MODE32, &mode);

	if (ret == -1)
	  return -1;

	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);

	if (ret == -1)
	  return -1;

	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);

	if (ret == -1)
	  return -1;

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);

	if (ret == -1)
	  return -1;

	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);

	if (ret == -1)
	  return -1;

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);

	if (ret == -1)
	  return -1;

	gpio17.setOutput();
	gpio17.write(true);

	return fd;
}

void SPI_Master::send_spi_msg(uint8_t* messages, uint8_t rx[], uint8_t cmd, int size, int delay, int speed){

	tr.tx_buf = (unsigned long)messages;
	tr.rx_buf = (unsigned long)rx;
	tr.len = size;
	tr.delay_usecs = delay;
	tr.speed_hz = speed;
	tr.bits_per_word = 0;

	if(previous_cmd != cmd){
	  previous_cmd = cmd;
	  gpio17.write(!cmd);
	}


	// Send SPI data

	int ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);

	if(ret == -1){
		fprintf(stderr,"ERROR: Error in SPI transmission. Couldn't perform. Error description: %s\n", strerror(errno));
	}
}


int SPI_Master::spi_end(){
	int status = close(fd);
	if(status != -1){
	fd = 0;
	}
	gpio17.~GPIO();

	return status;
}

#include "spi_master.h"
#include "../../custom_gpio/custom_gpio.h"

static int fd;

static CustomGPIO::GPIO gpio17(17);
static bool previous_cmd = false;

struct spi_ioc_transfer tr;

/**
 * @brief Starts the SPI device and configures it to allow SPI communications.
 * 
 * @param[in] spi_device Indicates which SPI device of type spidev0.<number> 
 *                       within the /dev/ directory will be used.
 *
 * @return 0 if success, -1 if error.
 */
int SPI_Master::spi_start (int spi_device, int mode, int bits, int speed) {

	if(fd == 0){
    //Open file descriptor
    char spiFile[15];
    sprintf(spiFile, "/dev/spidev0.%d", spi_device);

    fd = open(spiFile, O_RDWR);

    if(fd == -1){
      return -1;
    }
	}

	if(ioctl(fd, SPI_IOC_WR_MODE32, &mode))
	  return -1;

	if(ioctl(fd, SPI_IOC_RD_MODE, &mode))
	  return -1;

	if(ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits))
	  return -1;

	if(ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits))
	  return -1;

	if(ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed))
	  return -1;

	if(ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed))
	  return -1;

	gpio17.setOutput();
	gpio17.write(true);

	return 0;
}

/**
 * @brief It sends SPI data of length "size" to the other SPI end, at a speed "speed" and with a delay time "delay" after the data is sent.
 *        The response is stored in the rx buffer. To differentiate between normal data and commands, the GPIO17 pin will be enabled or
 *        disabled. This will be indicated by the parameter "cmd".
 *
 * @param[in] messages Pointer to the SPI data to send.
 * @param[in] rx Pointer to the array where the response will be stored.
 * @param[in] cmd If true, the data sent is a command, if false, it is normal data.
 * @param[in] size The size of the data sent.
 * @param[in] speed Speed of the SPI link.
 *
 */
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


/**
 * @brief End SPI communications and free all the related resources.
 *
 * @return 0 if success, -1 if error.
 */
int SPI_Master::spi_end(){
	int status = close(fd);
	if(status != -1){
	fd = 0;
	}
	gpio17.~GPIO();

	return status;
}

 #include <stdio.h>
 #include <string.h>
 #include <errno.h>
 #include <linux/spi/spidev.h>
 #include <fcntl.h>
 #include <sys/mman.h>

 ////////////////

 #include <stdint.h>
 #include <stdlib.h>
 #include <ctype.h>
 #include <poll.h>
 #include <unistd.h>
 #include <errno.h>
 #include <string.h>
 #include <time.h>
 #include <pthread.h>
 #include <sys/time.h>
 #include <sys/stat.h>
 #include <sys/wait.h>
 #include <sys/ioctl.h>

 ///////////////


 #define INPUT            0
 #define OUTPUT           1
 #define LOW              0
 #define HIGH             1


 #define BLOCK_SIZE      (4*1024)
 #define BCM2708_PERI_BASE 0x20000000
 #define GPIO_BASE       (BCM2708_PERI_BASE + 0x00200000)

 #define SPI_CHANNEL 0
 #define SPI_SPEED   1000000  // 1MHza

 static unsigned char     spiMode   = 0 ;
 static unsigned char     spiBPW    = 8 ;
 static unsigned short    spiDelay  = 0;
 static unsigned int    spiSpeeds [2] ;

 static volatile unsigned int *gpio ;
 static int         spiFds [2] ;

 // 25번 GPIO 핀의 Fuction Select을 한다.
 void pin_25_Mode( int mode);
 // Write으로 Fuction Select된 25번 GPIO의
 // HIGH 또는 LOW 값을 입력한다.
 void digitalWrite_pin_25(int value);
 // ADC의 인자로 넘긴 채널로 부터 12bit 값을 읽어온다.
 int read_mcp3208_adc(unsigned char adcChannel);
 // read_mcp_3204_adc 함수 내부에서 부리면서 실제적으로 SPI 통신을 하는 함수
 int SPIDataRW (int channel, unsigned char *data, int len);
 
 int main (void)
 {
     int adcChannel = 0;
     int adcValue   = 0;
     int fd;

     if ((fd = open ("/dev/mem", O_RDWR | O_SYNC) ) < 0)
     {
          return -1 ;
     }
	// 25번 GPIO는 MCP_3204와 SPI Master간의 Chip Select 신호로 쓰이며
	// 통신 동기화를 맞추는 역할을 한다.MCP_3204의 Timing Diagram을 참고/
     gpio = (unsigned int *)mmap(0, BLOCK_SIZE, PROT_READ|PROT_WRITE,
             MAP_SHARED, fd, GPIO_BASE) ;
     printf("%d\n", gpio);
     printf("%d\n", *gpio);
     
     if ((int)gpio == -1)
     {
         return -1 ;
     }
 
	// 리눅스에서 모든 주변장치(Peripheral)는 파일로 관리된다.
	// spi와 관련된 파일을 open하고 필요한 드리이버 API를 사용하게 된다.
     if ((fd = open ("/dev/spidev0.0", O_RDWR)) < 0)
     {
         printf("/dev/spidev0.0\n");
         return 1 ;
     }

     spiFds    [0] = fd ;

     pin_25_Mode(OUTPUT); // CS 신호로 사용하기 위해서 OUTPUT으로 
						  // Fuction Set 한다.

     while(1)
     {
		 // 값을 1초 마다 읽어 드린다. 이때 300 - ((adcValue*500)>>10
		 // 는 온도 센서 제조업체에서 제공하는 스케일링된 값이다.
         adcValue = read_mcp3208_adc(adcChannel);
         printf("adc0 Value = %u\n", adcValue);
         sleep(1);
     }

     return 0;

 }


 int SPIDataRW (int channel, unsigned char *data, int len)
 {	 //SPI 통신은 spi_ioc_transfer 구조체라는 일관된 인터페이스를
	 //통해 SPI Master/Slave 간 통신을 한다.
     struct spi_ioc_transfer spi ;

     channel &= 1 ;
	 // data를 통해 지정된 채널의 값을 읽어오도록 한다.
	 // tx_buf가 MCP_3204의 Timing Diagram에 D0, D1, D2에 해당한다.
	 // tx_buf를 그대로 이용하여 rx_buf로 12bit ADC된 값을 받는다
     spi.tx_buf        = (unsigned long)data ;
     spi.rx_buf        = (unsigned long)data ;
     spi.len           = len ;
     spi.delay_usecs   = spiDelay ;
     spi.speed_hz      = 1000000;
     spi.bits_per_word = spiBPW ;
	 //  spi_ioc_transfer 구조체의 configuration을 한다.

	 // SPI_IOC_MESSAGE(1) Command는 데이터를 써넣는 매크로인 _IOW로 
	 // 치환된다. 자세한 내용은 Linux/include/uapi/linux/spi/spidev.h을 참고
	 // ioctl 함수를 통해 쓰기 명령과 버퍼의 주소를 드라이버로 보낸다.
     return ioctl (spiFds [channel], SPI_IOC_MESSAGE(1), &spi) ;
 }

 int read_mcp3208_adc(unsigned char adcChannel)
{
  unsigned char buff[3];
  int adcValue = 0;

  buff[0] = 0x06 | ((adcChannel & 0x07) >> 7);
  buff[1] = ((adcChannel & 0x07) << 6);
  buff[2] = 0x00;

  digitalWrite_pin_25(0);  // Low : CS Active

  SPIDataRW(SPI_CHANNEL, buff, 3);

  buff[1] = 0x0F & buff[1];
  adcValue = ( buff[1] << 8) | buff[2];

  digitalWrite_pin_25(1);  // High : CS Inactive

  return adcValue;
}




 void pin_25_Mode(int mode)
 {
     int fSel, shift, alt ;

       fSel    = 2;
       shift   = 15;

	 if (mode == INPUT){
			*(gpio + fSel ) = (*(gpio + fSel) & ~(7 << shift)) ; // Sets bits to     zero = input
     }
	 else if (mode == OUTPUT){
			*(gpio ) = (*(gpio ) & ~(7 << shift)) | (1 << 24) ;
			printf("OUTPUT\n");
	 }
 }

 void digitalWrite_pin_25(int value)
 {
     int pin =8;
     int gpCLR = 10;
     int gpSET = 7;

     if (value == LOW)
         *(gpio + gpCLR) = 1 << (pin & 31) ;
     else
         *(gpio + gpSET) = 1 << (pin & 31) ;
}


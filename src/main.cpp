#include <mbed.h>

SPI spi(PF_9, PF_8, PF_7); // mosi, miso, sclk
DigitalOut cs(PC_1);
//SPI_HandleTypeDef hspi5;
//address of first register with gyro data
#define STATUS_BYTE 0xF0
#define OUT_DATA_1 0x00
#define OUT_DATA_2 0x00
#define OUT_DATA_3 0x00

#define READY_FLAG 1

uint8_t write_buf[32]; 
uint8_t read_buf[32];

EventFlags flags;
//The spi.transfer() function requires that the callback
//provided to it takes an int parameter
void cb(int event){
  flags.set(READY_FLAG);
  //deselect the sensor
  cs=1;

}

int main() {
    // Chip must be deselected
    cs = 1;
 
    // Setup the spi for 8 bit data, high steady state clock,
    // second edge capture, with a 5KHz clock rate
    spi.format(8,0);
    spi.frequency(500000);
  
    // Select the device by seting chip select low 
    cs=0;  
    while (1) {
    int32_t raw_gx;
    float gx;
      //prepare the write buffer to trigger a sequential read
      //write_buf[0]=STATUS_BYTE; //STATUS_BYTE = 0xF0
      //write_buf[1]=OUT_DATA_1; //OUT_DATA_1 = 0x00
      //write_buf[2]=OUT_DATA_2; //OUT_DATA_2 = 0x00
      //write_buf[3]=OUT_DATA_3; //OUT_DATA_3 = 0x00
      cs=0;

      //start sequential sample reading
      //spi.transfer(write_buf,4,read_buf,4,cb,SPI_EVENT_COMPLETE);
      //flags.wait_all(READY_FLAG);
      uint8_t configreg = spi.write(STATUS_BYTE);
      uint8_t data1 = spi.write(OUT_DATA_1);
      uint8_t data2 = spi.write(OUT_DATA_2);
      uint8_t data3 = spi.write(OUT_DATA_3);

      //read_buf after transfer: 0x00 <- 0x00 <- 0x00
      //raw_gx=( (((uint8_t)read_buf[3])<<16) | ((uint8_t)read_buf[2])<<8) | ((uint8_t)read_buf[1]);
      raw_gx=( (((uint32_t) data1)<<16) | ((uint32_t) data2)<<8) | ((uint32_t) data3);

      //printf("RAW| gx: %d \t gy: %d \t gz: %d\n",raw_gx,raw_gy,raw_gz);
      //float gx1 = HAL_SPI_Receive(&hspi5,read_buf,sizeof(read_buf),50000);
      gx = (( ( ((float)raw_gx) - 1677722) * (1 - (-1)) ) / (15099494 - 1677722)) - 1;
      
      printf("Actual| pressure: %4.5f\n",gx);

     thread_sleep_for(100);

    }
}
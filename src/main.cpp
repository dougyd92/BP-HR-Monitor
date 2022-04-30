#include <mbed.h>


SPI spi(PA_7, PA_6, PA_5); // mosi, miso, sclk
DigitalOut cs(PA_4);
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
    spi.frequency(50000);
  
    // Select the device by seting chip select low   
    while (1) {
    int32_t raw_gx;
    float gx;
    uint8_t data1;
    uint8_t data2;
    uint8_t data3;
      //prepare the write buffer to trigger a sequential read
      //write_buf[0]=STATUS_BYTE; //STATUS_BYTE = 0xF0
      //write_buf[1]=OUT_DATA_1; //OUT_DATA_1 = 0x00
      //write_buf[2]=OUT_DATA_2; //OUT_DATA_2 = 0x00
      //write_buf[3]=OUT_DATA_3; //OUT_DATA_3 = 0x00

      //start sequential sample reading
      //spi.transfer(write_buf,4,read_buf,4,cb,SPI_EVENT_COMPLETE);
      //flags.wait_all(READY_FLAG);
      
      cs=0;
      uint8_t status = spi.write(0xAA);
      uint8_t cmddata1 = spi.write(0x00);
      uint8_t cmddata2 = spi.write(0x00);
      cs = 1;

      // printf("Status 0 %02X \n",status);   

      thread_sleep_for(6);

      cs = 0;
      status = spi.write(0xF0); 
      // printf("Status 1 %02X \n",status);   
      
      // while(status & 0x20) {
      //   status = spi.write(0xF0);
      //   // data1 = spi.write(OUT_DATA_1);
      //   // data2 = spi.write(OUT_DATA_2);
      //   // data3 = spi.write(OUT_DATA_3);
      //   // printf("Status 2 %02X \n",status);   
      // }
      // status = spi.write(0xF0);
      

      //wait_us(500);
      // status = spi.write(STATUS_BYTE);
      data1 = spi.write(OUT_DATA_1);
      data2 = spi.write(OUT_DATA_2);
      data3 = spi.write(OUT_DATA_3);

      cs = 1;

      //read_buf after transfer: 0x00 <- 0x00 <- 0x00
      //raw_gx=( (((uint8_t)read_buf[3])<<16) | ((uint8_t)read_buf[2])<<8) | ((uint8_t)read_buf[1]);
      raw_gx=( (((uint32_t) data1)<<16) | ((uint32_t) data2)<<8) | ((uint8_t) data3);

      //printf("RAW| gx: %d \t gy: %d \t gz: %d\n",raw_gx,raw_gy,raw_gz);
      //float gx1 = HAL_SPI_Receive(&hspi5,read_buf,sizeof(read_buf),50000);
      gx = (( ( ((float)raw_gx) - 419430.4) * (300 ) ) / (3774873.6 - 419430.4));
      //gx = (float)raw_gx;
      printf("Actual new pressure: %4.5f \n",gx);
      // printf("Sensor values: %02X %02X %02X\n",data1,data2,data3);


     thread_sleep_for(100);

    }
}


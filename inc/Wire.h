#ifndef Wire_h
#define Wire_h

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <cstdio>

#include "Common.h"

class TwoWire
{
public:
  TwoWire() : m_address(0x42), i2c_fd(0) {}
  TwoWire(uint8_t default_address) : m_address(default_address), i2c_fd(0) {}
  ~TwoWire() { close(i2c_fd); }

  bool init(const char *filename = I2C_DEV)
  {

    if ((i2c_fd = open(filename, O_RDWR)) < 0)
    {
      printf("I2C: Failed to open bus '%s': %i-%s\n",
             filename, errno, strerror(errno));
      return false;
    }

    printf("I2C: Successfully initialized with default address [%s 0x%02x]\n", filename, m_address);
    return true;
  }

  bool beginTransmission(uint8_t address)
  {
    m_address = address;
    if (ioctl(i2c_fd, I2C_SLAVE, m_address) < 0)
    {
      printf("I2C: Failed to acquire bus access and/or talk to slave: %i-%s",
             errno, strerror(errno));
      return false;
    }
    return true;
  }
  uint8_t endTransmission(void)
  {
    if (m_sendBufPos > 0)
    {
      if (::write(i2c_fd, m_sendBuf, m_sendBufPos) != m_sendBufPos)
      {
        printf("Failed to write to the i2c bus.\n");
        return -1;
      }
    }

    return 0;
  }

  uint8_t endTransmission(bool stop) { return endTransmission(); }
  bool available() { return (m_readBufPos < m_quantity); }
  uint8_t read() { return m_readBuf[m_readBufPos++]; }

  size_t write(uint8_t byte)
  {
    if (byte == 0xFD)
    {
      get_available_bytes();
      return m_sendBufPos;
    }

    m_sendBuf[m_sendBufPos] = byte;
    return m_sendBufPos++;
  }

  size_t write(uint8_t *byte, size_t num_of_bytes)
  {
    size_t res;
    for (size_t i = 0; i < num_of_bytes; i++)
    {
      res = write(byte[i]);
    }
    return res;
  }

  uint8_t get_available_bytes()
  {
    // Dedicated function to read the available bytes register
    // Should be done with a single read-and-then-write operation, otherwise no valid length is returned

    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages[2];

    unsigned char outBuf = 0xFD;
    messages[0].addr = m_address;
    messages[0].flags = 0;
    messages[0].len = sizeof(outBuf);
    messages[0].buf = &outBuf;

    /* The data will get returned in this structure */
    messages[1].addr = m_address;
    messages[1].flags = I2C_M_RD;
    messages[1].len = 2; // registers 0xFD and 0xFE
    messages[1].buf = m_readBuf;

    /* Send the request to the kernel and get the result back */
    packets.msgs = messages;
    packets.nmsgs = 2;
    if (ioctl(i2c_fd, I2C_RDWR, &packets) < 0)
    {
      perror("Unable to get the number of available bytes");
      return 1;
    }

    m_quantity = 2;
    m_readBufPos = 0;
    l_IsAvailable = true;

    return 0;
  }

  uint8_t requestFrom(uint8_t address, uint8_t quantity)
  {
    if (l_IsAvailable)
    {
      // in the previous command we have read the lengt buffer, no need to redo this:
      l_IsAvailable = false;
      return m_quantity;
    }

    if (::read(i2c_fd, m_readBuf, quantity) != quantity)
    {
      printf("Failed to read from the i2c bus.\n");
      return 0;
    }

    m_address = address;
    m_quantity = quantity;
    m_readBufPos = 0;
    return quantity;
  }

private:
  uint8_t m_address;
  bool m_IsAvailable;
  bool m_IsConnected;
  uint8_t m_recvdByte;
  uint8_t m_quantity;
  int i2c_fd;

  unsigned char m_sendBuf[SHORT_BUFF];
  uint8_t m_sendBufPos = 0;

  unsigned char m_readBuf[SHORT_BUFF];
  uint8_t m_readBufPos = 0;

  bool l_IsAvailable = false;

  size_t read(uint8_t byte)
  {
    int iRxLen = 0;
    iRxLen = ::read(i2c_fd, (void *)&byte, sizeof(byte));
    if (iRxLen > 0)
    {
      m_IsAvailable = true;
    }
    else
    {
      m_IsAvailable = false;
    }
    return m_IsAvailable;
  }
};

extern TwoWire Wire;

#endif //Wire_h
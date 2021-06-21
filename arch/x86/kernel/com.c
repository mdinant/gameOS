#include <system.h>

#define PORT 0x3f8 // COM1

void com_handler(struct regs *r)
{
    printf("something happened with com\n");
}

bool init_serial()
{
    outpb(PORT + 1, 0x00); // Disable all interrupts
    outpb(PORT + 3, 0x80); // Enable DLAB (set baud rate divisor)
    outpb(PORT + 0, 0x03); // Set divisor to 3 (lo byte) 38400 baud
    outpb(PORT + 1, 0x00); //                  (hi byte)
    outpb(PORT + 3, 0x03); // 8 bits, no parity, one stop bit
    outpb(PORT + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
    outpb(PORT + 4, 0x0B); // IRQs enabled, RTS/DSR set
    outpb(PORT + 4, 0x1E); // Set in loopback mode, test the serial chip
    outpb(PORT + 0, 0xAE); // Test serial chip (send byte 0xAE and check if serial returns same byte)

    // Check if serial is faulty (i.e: not same byte as sent)
    if (inpb(PORT + 0) != 0xAE)
    {
        return FALSE;
    }

    // If serial is not faulty set it in normal operation mode
    // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
    outpb(PORT + 4, 0x0F);

    irq_install_handler(4, com_handler);

    return TRUE;
}
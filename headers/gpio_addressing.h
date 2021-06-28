#ifndef SRC_GPIO_ADDRESSING_H
#define SRC_GPIO_ADDRESSING_H

/*******************************************************************************/
// 
// DESCRIPTION : GPIO Register addressing as defined by the 
//               BCM2711 ARM Peripherals Manual.
//
// PURPOSE     : Define all GPIO register addressing for use my mmap or iowrite/read
//               calls.  
//
// DETAILS     : For the BCM2711 processor, there are 58 GPIO
//               pins they are split across 3 banks (0-indexed).
//               bank 0 contains GPIOs 0 to 27, bank 1 contains
//               GPIOs 28 to 45, and bank 3 continas GPIOs 46 to 57.
//               All GPIO pins have at least 2 alternative functions 
//               which usually are peripheral IO and a single peripheral
//               may appear in multiple banks to allow flexibility.
//               The GPIO peripheral has four dedicated interrupt lines, triggered
//               by setting bits in the event dect status register.
//
/*******************************************************************************/

/* ARM Peripheral Base Address */
#define ARM_PERIPHERAL_BASE_ADDR 0xFE000000 
/* GPIO Base Address Register */
#define GPIO_BASE_REG_ADDR (ARM_PERIPHERAL_BASE_ADDR + 0x00200000)

/* GPIO Register Size (all 32-bit words) */
#define GPIO_REG_SIZE 0x04 // 32-bit, 4 bytes in size

/* GPIO Write/Read Masks - Used for all registers, depending on bit mapping */
// For 1-bit mapped registers (AFEN, AREN, LEN, HEN etc.), no bits are masked
// in the lower register. In the high register, the upper 6-bits are masked
#define GPREG0_1BIT_WRITE_MASK 0xFFFFFFFF // all bits are used
#define GPREG1_1BIT_WRITE_MASK 0x03FFFFFF // upper 6 bits not used, should be 0
#define GPREG0_1BIT_READ_MASK  0xFFFFFFFF // all bits are used
#define GPREG1_1BIT_READ_MASK  0x03FFFFFF // upper 6 bits don't care, made to be 0
// For 2-bit mapped registers (PUP_PDN register), no bits masked in lower registers, 
// upper 12 masked in the highest register 3.
#define GPREG0_2_2BIT_WRITE_MASK 0xFFFFFFFF // all bits are used
#define GPREG3_2BIT_WRITE_MASK   0x000FFFFF // upper 12 bits not used, should be 0
#define GPREG0_2_2BIT_READ_MASK  0xFFFFFFFF // all bits are used
#define GPREG3_2BIT_READ_MASK    0x000FFFFF // upper 12 bits don't care, made to be 0
// For 3-bit mapped registers (FNSEL register), lower 2 bits masked in lower registers, 
// upper 8 masked in the highest register 5.
#define GPREG0_4_3BIT_WRITE_MASK 0x3FFFFFFF // upper two bits should be set to 0, for registers 0-4
#define GPREG5_3BIT_WRITE_MASK   0x00FFFFFF // upper eight bits should be set to 0, for register 5
#define GPREG0_4_3BIT_READ_MASK  0x3FFFFFFF // don't care about upper two bits
#define GPREG5_3BIT_READ_MASK    0x00FFFFFF // upper eight bits should be set to 0, for register 5

/* GPIO Function Selection Register Offsets 
 * 
 * DESCRIPTION : Select the operation of GPIO pins, the FSELn field determines
 *               the functionality of the nth GPIO pin. Unused alternative function
 *               lines are tied to ground and will output 0 if selected, all pins 
 *               reset to normal GPIO input operation.
 *
 * BIT MAP (GPFSEL0) :
 *  -------------------------------------------------------------------
 * | Bit(s)  | Field Name | Description                 | Type | Reset |
 *  ___________________________________________________________________
 *  ___________________________________________________________________
 * | 31:30   | -          | Reserved, write as 0, read  | RW   | 0     |
 * |         |            | is a don't care.            |      |       |
 *  ___________________________________________________________________
 * | 29:27   | FSEL9      | FSEL9 - Function Select 9   | RW   | 0     |
 * |         |            | 000 = GPIO Pin 9 as a input |      |       |
 * |         |            | 001 = GPIO Pin 9 as a output|      |       |
 * |         |            | 100 = GPIO Pin 9 as alt fn 0|      |       |
 * |         |            | 101 = GPIO Pin 9 as alt fn 1|      |       |
 * |         |            | 110 = GPIO Pin 9 as alt fn 2|      |       |
 * |         |            | 111 = GPIO Pin 9 as alt fn 3|      |       |
 * |         |            | 011 = GPIO Pin 9 as alt fn 4|      |       |
 * |         |            | 010 = GPIO Pin 9 as alt fn 5|      |       |
 *  ___________________________________________________________________
 * | 26:24   | FSEL8      | FSEL8 - Function Selection 8| RW   | 0     |
 *  ___________________________________________________________________
 * | 23:21   | FSEL7      | FSEL7 - Function Selection 7| RW   | 0     |
 *  ___________________________________________________________________
 * | 20:18   | FSEL6      | FSEL6 - Function Selection 6| RW   | 0     |
 *  ___________________________________________________________________
 * | 17:15   | FSEL5      | FSEL5 - Function Selection 5| RW   | 0     |
 *  ___________________________________________________________________
 * | 14:12   | FSEL4      | FSEL4 - Function Selection 4| RW   | 0     |
 *  ___________________________________________________________________
 * | 11:9    | FSEL3      | FSEL3 - Function Selection 3| RW   | 0     |
 *  ___________________________________________________________________
 * | 8:6     | FSEL2      | FSEL2 - Function Selection 2| RW   | 0     |
 *  ___________________________________________________________________
 * | 5:3     | FSEL1      | FSEL1 - Function Selection 1| RW   | 0     |
 *  ___________________________________________________________________
 * | 2:0     | FSEL0      | FSEL0 - Function Selection 0| RW   | 0     |
 *  -------------------------------------------------------------------
 *
 *  GPFSEL1-5 follow the same format, where GPFSEL1 is used for GPIO pins 19-10,
 *  GPFSEL2 is used for pins 29-20, ..., GPFSEL5 is used for GPIO pins 57-50
 */
// GPIO Function Selection Offsets
#define GPFN_SEL0_OFF 0x00
#define GPFN_SEL1_OFF 0x04
#define GPFN_SEL2_OFF 0x08
#define GPFN_SEL3_OFF 0x0C
#define GPFN_SEL4_OFF 0x10
#define GPFN_SEL5_OFF 0x14

/* GPIO Set Registers 
 *
 * DESCRIPTION : Output set registers used to set a GPIO pin, the SETn field defines
 *               the respective GPIO pin to set, writing a "0" has no effect. If the 
 *               GPIO pin is being used as an input (by default) then the SETn field
 *               is ignored, otherwise if the pin is subsequently defined as an output
 *               then the bit will be set according to the last set/clear operation. 
 *               Set/clear functions are separated so that the need for read-modify-write 
 *               operations is removed
 *
 *  BIT MAP :
 *  GPSET0 Register
 *  -----------------------------------------------------------------------
 * | Bit(s)  | Field Name     | Description                 | Type | Reset |
 *  _______________________________________________________________________
 *  _______________________________________________________________________
 * | 31:0    | SETn (n=0..31) | 0 = No effect               | WO   | 0     | 
 * |         |                | 1 = Set GPIO pin n          |      |       |
 *  -----------------------------------------------------------------------
 *
 *  GPSET1 Register
 *  -----------------------------------------------------------------------
 * | Bit(s)  | Field Name     | Description                 | Type | Reset |
 *  _______________________________________________________________________
 *  _______________________________________________________________________
 * | 31:26   | -              | Reserved - Write as 0, read | -    |       |
 * |         |                | don't care                  |      |       |
 *  _______________________________________________________________________
 *
 * | 25:0    | SETn (n=32..57)| 0 = No effect               | WO   | 0     | 
 * |         |                | 1 = Set GPIO pin n          |      |       |
 *  -----------------------------------------------------------------------
 *
 */
// GPIO Set Register Offsets (write only)
#define GPSET0_OFF 0x1C
#define GPSET1_OFF 0x20

/* GPIO Set Registers 
 *
 * DESCRIPTION : Output set registers used to clear a GPIO pin, the CLRn field defines
 *               the respective GPIO pin to clear, writing a "0" has no effect. If the 
 *               GPIO pin is being used as an input (by default) then the SETn field
 *               is ignored, otherwise if the pin is subsequently defined as an output
 *               then the bit will be set according to the last set/clear operation. 
 *               Set/clear functions are separated so that the need for read-modify-write 
 *               operations is removed.
 *
 * BIT MAP :
 *  GPCLR0 Register
 *  -----------------------------------------------------------------------
 * | Bit(s)  | Field Name     | Description                 | Type | Reset |
 *  _______________________________________________________________________
 *  _______________________________________________________________________
 * | 31:0    | SETn (n=0..31) | 0 = No effect               | WO   | 0     | 
 * |         |                | 1 = Clear GPIO pin n        |      |       |
 *  -----------------------------------------------------------------------
 *
 *  GPCLR1 Register
 *  -----------------------------------------------------------------------
 * | Bit(s)  | Field Name     | Description                 | Type | Reset |
 *  _______________________________________________________________________
 *  _______________________________________________________________________
 * | 31:26   | -              | Reserved - Write as 0, read | -    |       |
 * |         |                | don't care                  |      |       |
 *  _______________________________________________________________________
 *
 * | 25:0    | SETn (n=32..57)| 0 = No effect               | WO   | 0     | 
 * |         |                | 1 = Clear GPIO pin n        |      |       |
 *  -----------------------------------------------------------------------
 *
 */
// GPIO Clear Register Offsets (write only)
#define GPCLR0_OFF 0x28
#define GPCLR1_OFF 0x2C

/* GPIO Pin Level Registers (giving the actual value of the pin) 
 *
 * BIT MAP :
 *  GPLEV0 Register
 *  -----------------------------------------------------------------------
 * | Bit(s)  | Field Name     | Description                 | Type | Reset |
 *  _______________________________________________________________________
 *  _______________________________________________________________________
 * | 31:0    | LEVn (n=0..31) | 0 = GPIO pin n is low       | RO   | 0     | 
 * |         |                | 1 = GPIO pin n is high      |      |       |
 *  -----------------------------------------------------------------------
 *
 *  GPLEV1 Register
 *  -----------------------------------------------------------------------
 * | Bit(s)  | Field Name     | Description                 | Type | Reset |
 *  _______________________________________________________________________
 *  _______________________________________________________________________
 * | 31:26   | -              | Reserved - Write as 0, read | -    |       |
 * |         |                | don't care                  |      |       |
 *  _______________________________________________________________________
 * | 25:0    | LEVn (n=32..57)| 0 = GPIO pin n is low       | RO   | 0     | 
 * |         |                | 1 = GPIO pin n is high      |      |       |
 *  -----------------------------------------------------------------------
 *
 */
#define GPLEV0_OFF 0x34 // pins 0..31, 0 = low, 1 = high
#define GPLEV1_OFF 0x38 // pins 32..57, 0 = low, 1= high (upper 5 bits unused/0)

/* GPIO Pin Event Detect Status
 *
 * DESCRIPTION : Used to record level and edge events on the GPIO pins. The relevant bit in the event detect 
 * status registers is set when 1. an edge is detected that matches the type of edge programmed 
 * in the rising/falling detect enable registers 2. a level is detected that matches the type of 
 * level programmed in the high/low detect enable registers. The bit is cleared by writing a "1" 
 * to the relevant bit. The interrupt controller can be programmed to interrupt the processor whenever any of the 
 * status bits are set. The GPIO peripheral has four dedicated interrupt lines. Each GPIO bank
 * can generate an independent interrupt. The fourth line generates a single interrupt whenever 
 * any bit is set.
 *
 * BIT MAP :
 *  GPEDS0 Register
 *  -----------------------------------------------------------------------
 * | Bit(s)  | Field Name     | Description                 | Type | Reset |
 *  _______________________________________________________________________
 *  _______________________________________________________________________
 * | 31:0    | EDSn (n=0..31) | 0 = Event not detected on   | W1C  | 0     | 
 * |         |                |     GPIO pin n              |      |       |
 * |         |                | 1 = Event detected on GPIO  |      |       |
 * |         |                |     pin n                   |      |       |
 *  -----------------------------------------------------------------------
 *
 *  GPCLR1 Register
 *  -----------------------------------------------------------------------
 * | Bit(s)  | Field Name     | Description                 | Type | Reset |
 *  _______________________________________________________________________
 *  _______________________________________________________________________
 * | 31:26   | -              | Reserved - Write as 0, read | -    |       |
 * |         |                | don't care                  |      |       |
 *  _______________________________________________________________________
 * | 25:0    | EDSn (n=32..57)| 0 = Event not detected on   | W1C  | 0     | 
 * |         |                |     GPIO pin n              |      |       |
 * |         |                | 1 = Event detected on GPIO  |      |       |
 * |         |                |     pin n                   |      |       |
 *  -----------------------------------------------------------------------
 *
 */
// GPIO Event Detect Status Registers
#define GPEDS0_OFF 0x40 // pins 0..31, 0 = event not detected, 1 = event detected
#define GPEDS1_OFF 0x44 // pins 32..57, 0 = event not detected, 1 = event detected
                        // write 1 to clear the relevant bit

/******************************************************************************/
/******************************************************************************/
  /* +++++ TODO : More verbose descriptions of registers like above ++++++++ */
/******************************************************************************/
/******************************************************************************/
/* GPIO Rising Edge Detect Enable Registers 
 * 
 * DESCRIPTION : These registers define pins for which a rising edge transition
 *               sets a bit in the event detect status registers (GPEDSn). If both
 *               bits in GPRENn and GPFENn are set than ANY transition shall set the 
 *               relevant bit in the GPEDSn registers. GPRENn uses syncrhonous edge detection, 
 *               to suppress glitches the input is sampled using the system clock and looks for 
 *               the pattern "011". 
 *
 * ACCESS      : RW
 *
 */
#define GPREN0_OFF 0x4C // pins 0..31, 0 = Rising edge detect disabled, 1 = Rising edge on GPIO pin n 
                        //             sets corresponding bit in GPREDS0
#define GPREN1_OFF 0x50 // pins 32..57, 0 = Rising edge detect disabled, 1 = Rising edge on GPIO pin n 
                        //             sets corresponding bit in GPREDS1

/* GPIO Falling Edge Detect Enable Registers 
 * 
 * DESCRIPTION : These registers define pins for which a falling edge transition
 *               sets a bit in the event detect status registers (GPEDSn). If both
 *               bits in GPFENn and GPRENn are set than ANY transition shall set the 
 *               relevant bit in the GPEDSn registers. GPFENn uses syncrhonous edge detection, 
 *               to suppress glitches the input is sampled using the system clock and looks for 
 *               the pattern "100". 
 * 
 * ACCESS      : RW
 *
 */
#define GPFEN0_OFF 0x58 // pins 0..31, 0 = Falling edge detect disabled, 1 = Falling edge on GPIO pin n 
                        //             sets corresponding bit in GPEDS0
#define GPFEN1_OFF 0x5C // pins 32..57, 0 = Falling edge detect disabled, 1 = Falling edge on GPIO pin n 
                        //             sets corresponding bit in GPEDS1

/* GPIO High Level Detect Enable Registers 
 * 
 * DESCRIPTION : These registers define pins for which a high level sets a bit in the 
 *               sets a bit in the event detect status registers (GPEDSn). If the pin
 *               is still high when an attempt is made to clear the status bit in GPEDSn
 *               then the status bit will remain set.
 *
 * ACCESS      : RW
 *               
 */
#define GPHEN0_OFF 0x58 // pins 0..31, 0 = High detect disabled, 1 = High on GPIO pin n sets
                        //             corresponding bit in GPEDS0
#define GPHEN1_OFF 0x5C // pins 32..57, 0 = High detect disabled, 1 = High on GPIO pin n 
                        //             corresponding bit in GPEDS1

/* GPIO Low Level Detect Enable Registers 
 * 
 * DESCRIPTION : These registers define pins for which a low level sets a bit in the 
 *               sets a bit in the event detect status registers (GPEDSn). If the pin
 *               is still low when an attempt is made to clear the status bit in GPEDSn
 *               then the status bit will remain set.
 *
 * ACCESS      : RW
 *               
 */
#define GPLEN0_OFF 0x64 // pins 0..31, 0 = Low detect disabled, 1 = Low on GPIO pin n sets
                        //             corresponding bit in GPEDS0
#define GPLEN1_OFF 0x68 // pins 32..57, 0 = Low detect disabled, 1 = Low on GPIO pin n sets
                        //             corresponding bit in GPEDS1

/* GPIO Asynchronous Rising Edge Detect Enable Registers 
 * 
 * DESCRIPTION : These registers define pins for which an asynchronous rising edge transition 
 *               sets a bit in the event detect status registers (GPEDSn). The asynchronous nature 
 *               of this registers means the incoming signal is NOT smapled by the system clock. As
 *               such rising edges of very short duration can be detected.
 *
 * ACCESS      : RW
 *               
 */
#define GPAREN0_OFF 0x7C // pins 0..31, 0 = Async rising edge detect disabled, 1 = Async rising edge on GPIO pin n
                         //             sets corresponding bit in GPEDS0
#define GPAREN1_OFF 0x80 // pins 32..57, 0 = Async rising edge detect disabled, 1 = Async rising edge on GPIO pin n 
                         //             sets corresponding bit in GPEDS1

/* GPIO Asynchronous Falling Edge Detect Enable Registers 
 * 
 * DESCRIPTION : These registers define pins for which an asynchronous falling edge transition 
 *               sets a bit in the event detect status registers (GPEDSn). The asynchronous nature 
 *               of this registers means the incoming signal is NOT smapled by the system clock. As
 *               such falling edges of very short duration can be detected.
 *
 * ACCESS      : RW
 *               
 */
#define GPAFEN0_OFF 0x88 // pins 0..31, 0 = Async falling edge detect disabled, 1 = Async falling edge on GPIO pin n
                         //             sets corresponding bit in GPEDS0
#define GPAFEN1_OFF 0x8C // pins 32..57, 0 = Async falling edge detect disabled, 1 = Async falling edge on GPIO pin n 
                         //             sets corresponding bit in GPEDS1

/* GPIO Pull-up/Pull-down Registers 
 * 
 * DESCRIPTION : Control the actuation of internal pull up/down resistors. Reading them gives the current pull-state.
 *               The alternate function table also gives the current pull state which is applied after power-down.
 *
 * ACCESS      : RW
 *               
 */
// 00 = No resistor selected 
// 01 = Pull up resistor selected
// 10 = Pull down resistor selected 
// 11 = Reserved 
#define GP_PUP_PDN_CNTRL_REG0 0xE4 // GPIO pin 15 => 31:30 .. GPIO pin 0 => 01:00
#define GP_PUP_PDN_CNTRL_REG1 0xE8 // GPIO pin 31 => 31:30 .. GPIO pin 16 => 01:00
#define GP_PUP_PDN_CNTRL_REG2 0xEC // GPIO pin 47 => 31:30 .. GPIO pin 32 => 01:00
#define GP_PUP_PDN_CNTRL_REG3 0xF0 // GPIO pin 57 => 31:30 .. GPIO pin 48 => 01:00

// Refer to RPi's BCM2711 ARM Peripheral's Manual for information on Alternative Function Assignments

// GPIO "End" Address
#define GPIO_END_ADDR (GPIO_BASE_REG_ADDR + GP_PUP_PDN_CNTRL_REG3 + GPIO_REG_SIZE)

// GPIO Address Range Size
#define GPIO_ADDR_RANGE_SIZE (GPIO_END_ADDR - GPIO_BASE_REG_ADDR)

/* TODO: GENERAL PURPOSE CLOCK REGISTERS...*/
#endif

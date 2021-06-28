#include <stdbool.h>
#include <sys/mman.h>
#include <gpiod.h>
#include <gpio_addressing.h>
#include <stddef.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

/// ENUMS /// 
// Bit value, if bit were set
enum BitValue {
    BIT_VALUE_0 = 0x01,
    BIT_VALUE_1 = 0x02,
    BIT_VALUE_2 = 0x04,
    BIT_VALUE_3 = 0x08,
    BIT_VALUE_4 = 0x10,
    BIT_VALUE_5 = 0x20,
    BIT_VALUE_6 = 0x40,
    BIT_VALUE_7 = 0x80,
};
/// GLOBALS ///
// Max Pin Count
static const uint8_t PIN_MAX        = 57;
// Max Pin bit value in bit-mapped register
static const uint8_t PIN_MAX_BIT    = 32;
// Base 10 const
static const uint8_t BASE_TEN       = 10;
// Three bit mask
static const uint8_t THREE_BIT_MASK = 0x07;
// Size of a 32-bit int 
static const uint8_t BIT32_SIZE    = 32;
// Function Selection Registers
static const uint32_t FNSEL_REGS[]  = {
    GPFN_SEL0_OFF,
    GPFN_SEL1_OFF,
    GPFN_SEL2_OFF,
    GPFN_SEL3_OFF,
    GPFN_SEL4_OFF,
    GPFN_SEL5_OFF
};
// Pull-up/Pull-down Registers
static const uint32_t PUP_PDN_REGS [] = {
    GP_PUP_PDN_CNTRL_REG0,
    GP_PUP_PDN_CNTRL_REG1,
    GP_PUP_PDN_CNTRL_REG2,
    GP_PUP_PDN_CNTRL_REG3
};
// GPIO Set Registers
static const uint32_t GPSET_REGS [] = {
    GPSET0_OFF,
    GPSET1_OFF
};
// GPIO Clear Registers
static const uint32_t GPCLR_REGS [] = {
    GPCLR0_OFF,
    GPCLR1_OFF
};
// GPIO Level Registers 
static const uint32_t GPLEV_REGS [] = {
    GPLEV0_OFF,
    GPLEV1_OFF
};
// GPIO Event Detection Status Registers 
static const uint32_t GPEDS_REGS [] = {
    GPEDS0_OFF,
    GPEDS1_OFF
};
// GPIO Rising Edge Detect Enable Registers
static const uint32_t GPREN_REGS [] = {
    GPREN0_OFF,
    GPREN1_OFF
};
// GPIO Falling Edge Detect Enable Registers
static const uint32_t GPFEN_REGS [] = {
    GPFEN0_OFF,
    GPFEN1_OFF
};
// GPIO High Level Detect Enable Registers
static const uint32_t GPHEN_REGS [] = {
    GPHEN0_OFF,
    GPHEN1_OFF
};
// GPIO Low Level Detect Enable Registers
static const uint32_t GPLEN_REGS [] = {
    GPLEN0_OFF,
    GPLEN1_OFF
};
// GPIO Asynchronous Rising Edge Detect Enable Registers
static const uint32_t GPAREN_REGS [] = {
    GPAREN0_OFF,
    GPAREN1_OFF
};
// GPIO Asynchronous Falling Edge Detect Enable Registers
static const uint32_t GPAFEN_REGS [] = {
    GPAFEN0_OFF,
    GPAFEN1_OFF
};
// Number of Pull-up/Pull-down Registers
static const uint8_t PUP_PDN_REGS_SZ = sizeof(PUP_PDN_REGS)/sizeof(PUP_PDN_REGS[0]);
// Number of Function Selection Offsets
static const uint8_t FNSEL_REGS_SZ   = sizeof(FNSEL_REGS)/sizeof(FNSEL_REGS[0]);
// Number of 1-bit mapped registers in a given array
static const uint8_t BIT_1_REGS_SZ   = sizeof(GPSET_REGS)/sizeof(GPSET_REGS[0]);
/// STRUCTS ///
// Private data struct definition 
typedef struct _gpio_internals {
    /// File descriptor & Pin Value ///
    // File descriptor used in the memory mapped space
    int32_t _fd;
    // The Pin selected
    uint8_t _pin_value;
    /// Bit Masks ///
    // Write 1-bit mapped register mask
    uint32_t _1_bit_map_mask;
    // Write 2-bit mapped register mask
    uint32_t _2_bit_map_mask;
    // Write 3-bit mapped register mask
    uint32_t _3_bit_map_mask;
    /// Bit Shift ///
    // TODO: ...
    /// Functions ///
    // Function to set the gpio pin high/low
    int32_t (*_write_gpio)(_gpio_internals_t *, uint8_t);
    // Function to set the gpio pin function
    int32_t (*_set_gpio_fn)(_gpio_internals_t *, enum FunctionSelect);
    /// Registers ///
    // GPIO Function Selection Register
    void* _fn_sel_reg;
    // GPIO Pull Up/Pull Down Register
    void* _pup_pdn_reg;
    // GPIO Set Register
    void* _set_reg;
    // GPIO Clear Register
    void* _clr_reg;
    // GPIO Level Register
    void* _lvl_reg;
    // GPIO Event Detect Status Register
    void* _eds_reg;
    // GPIO Rising Edge Detect Register
    void* _ren_reg;
    // GPIO Falling Edge Detect Register
    void* _fen_reg;
    // GPIO High Level Detect Enable Register
    void* _hen_reg;
    // GPIO Low Level Detect Enable Register
    void* _len_reg;
    // GPIO Asynchronous Rising Edge Detect Enable
    void* _aren_reg;
    // GPIO Asynchronous Falling Edge Detect Enable
    void* _afen_reg;
}_gpio_internals_t;
/// FUNCTION DECLARATIONS ///
/* Check that the pin is in range */
int32_t pin_in_range(uint8_t);
/* Set the GPIO pin value */
int32_t __write_gpio(_gpio_internals_t *,
                     uint8_t);
/* Function selection of the GPIO pin */
int32_t __set_gpio_fn(_gpio_internals_t *,
                      enum FunctionSelect);



/// FUNCTION DEFINITIONS ///
/* "Public" Functions */
// Request a gpio line
int32_t request_gpio_line(gpio_line_t * gpio_line_req, 
                          uint8_t pin_value)
{
    // Function selection register index
    uint32_t fn_sel_reg_ind     = 0; 
    // Single bit-map register
    uint32_t single_bit_reg_ind = 0;
    // Pull-up/pull down register 
    uint32_t pup_pdn_reg_ind    = 0;
    // Return value for this request
    int32_t req_retval = 0;
    // File descriptor for handle to /dev/mem
    int32_t fd = -1;
    // The GPIO base address from ARM peripheral space
    void* gpio_base_uaddr = (void *) 0;
    // Check that the pin is in range
    if (-1 == pin_in_range(pin_value))
    {
        req_retval = EBAD_PIN;
    }
    // Open a file descriptor to devmem
    else if (-1 == (fd = open("/dev/mem", O_RDWR)))
    {
        req_retval = EDEVMEM_OPEN;
    } 
    /* 
     * Refer to the man page for mmap for the anatomy of this
     * system call
     */
    // Memory map of GPIO memory address space to user space 
    // MAP_FAILED = (void *) -1, the return value for value on an unsuccessful mmap
    else if ((void *) -1 == (gpio_base_uaddr = mmap(0, GPIO_ADDR_RANGE_SIZE, PROT_READ | PROT_WRITE, 
                                                    MAP_SHARED, fd, GPIO_BASE_REG_ADDR)))
    {
        req_retval = EMAP_FAIL;
    }
    // Allocate storage on the heap for the underlying private data structure
    // Check that the malloc call did not fail
    else if(NULL == (gpio_line_req->priv_dat = (_gpio_internals_t *) malloc(sizeof(_gpio_internals_t))))
    {
        req_retval = EMALLOC;
    }
    // Pin is in range, the /dev/mem open succeeded, GPIO register space is mapped, and malloc'ing the internals
    // succeeded. Now, set the internal "private" data
    else
    {
        /// Setting local(s) /// 
        // Figure the function selection (more generally 3-bit mapped) register 
        fn_sel_reg_ind     = ((pin_value / BASE_TEN) % FNSEL_REGS_SZ); 
        // Figure the pull-up/pull-down (more generally 2-bit mapped) register
        pup_pdn_reg_ind    = ((pin_value / (BIT32_SIZE / BIT_VALUE_1)) % PUP_PDN_REGS_SZ); // 16 pins per 2-bit allocated register (only pup_pdn)
        // Figure the one bit mapped registers
        single_bit_reg_ind = ((pin_value / PIN_MAX_BIT) % BIT_1_REGS_SZ);
        /// Set internal private fields ///
        // Internal file descriptor
        gpio_line_req->priv_dat->_fd = fd;
        // Internal pin value
        gpio_line_req->priv_dat->_pin_value = pin_value;
        // Bit masking for writing a function selection register specifically,
        // generally referred to as "3-bit" mapped register
        gpio_line_req->priv_dat->_3_bit_map_mask = fn_sel_reg_ind != (FNSEL_REGS_SZ - 1) ? 
                                                       GPREG0_4_3BIT_WRITE_MASK :
                                                       GPREG5_3BIT_WRITE_MASK;

        // Bit masking for writing a pull up/pull down register specifically
        gpio_line_req->priv_dat->_2_bit_map_mask = pup_pdn_reg_ind != (PUP_PDN_REGS_SZ - 1) ? 
                                                       GPREG0_2_2BIT_WRITE_MASK :
                                                       GPREG3_2BIT_WRITE_MASK;
        // Bit masking for writing all 1-bit masked registers
        gpio_line_req->priv_dat->_1_bit_map_mask = single_bit_reg_ind != (BIT_1_REGS_SZ - 1) ? 
                                                       GPREG0_1BIT_WRITE_MASK :
                                                       GPREG1_1BIT_WRITE_MASK;
        /// Register selection ///
        // The function selection register to use 
        gpio_line_req->priv_dat->_fn_sel_reg  = gpio_base_uaddr + FNSEL_REGS[fn_sel_reg_ind];
        // The pull-up/pull-down register to use 
        gpio_line_req->priv_dat->_pup_pdn_reg = gpio_base_uaddr + PUP_PDN_REGS[pup_pdn_reg_ind];
        // The set register to use
        gpio_line_req->priv_dat->_set_reg     = gpio_base_uaddr + GPSET_REGS[single_bit_reg_ind];
        // The clear register to use 
        gpio_line_req->priv_dat->_clr_reg     = gpio_base_uaddr + GPCLR_REGS[single_bit_reg_ind];
        // The level register to use
        gpio_line_req->priv_dat->_lvl_reg     = gpio_base_uaddr + GPLEV_REGS[single_bit_reg_ind];
        // The event detect register to use 
        gpio_line_req->priv_dat->_eds_reg     = gpio_base_uaddr + GPEDS_REGS[single_bit_reg_ind];
        // The rising edge detect enable register to use
        gpio_line_req->priv_dat->_ren_reg     = gpio_base_uaddr + GPREN_REGS[single_bit_reg_ind];
        // The falling edge detect enable register to use 
        gpio_line_req->priv_dat->_fen_reg     = gpio_base_uaddr + GPFEN_REGS[single_bit_reg_ind];
        // The high level detect enable register to use
        gpio_line_req->priv_dat->_hen_reg     = gpio_base_uaddr + GPHEN_REGS[single_bit_reg_ind];
        // The low level detect enable register to use 
        gpio_line_req->priv_dat->_len_reg     = gpio_base_uaddr + GPLEN_REGS[single_bit_reg_ind];
        // The async rising edge detect register to use
        gpio_line_req->priv_dat->_aren_reg    = gpio_base_uaddr + GPAREN_REGS[single_bit_reg_ind];
        // The async falling edge detect register to use 
        gpio_line_req->priv_dat->_afen_reg    = gpio_base_uaddr + GPAFEN_REGS[single_bit_reg_ind];
        /// Setting internal Functions ///
        // Set the gpio function
        gpio_line_req->priv_dat->_set_gpio_fn = __set_gpio_fn;
        // Write the gpio line
        gpio_line_req->priv_dat->_write_gpio  = __write_gpio;
    }

    // If the return value was set to anything non-nominal, then free the inner private data
    // of the request object and close the file descriptor
    if (0 != req_retval)
    {
        // Close the file descriptor even if it was not open
        close(fd);
        // Free the private data
        free(gpio_line_req->priv_dat);
    }

    return req_retval;
}
// Write GPIO value 
int32_t write_gpio(gpio_line_t* line, 
                   uint8_t high_low)
{
    return (NULL != line->priv_dat) && line->priv_dat->_write_gpio(line->priv_dat, high_low);
}
// Set the GPIO function 
int32_t set_gpio_fn(gpio_line_t* line, 
                    enum FunctionSelect sel)
{
    return (NULL != line->priv_dat) && line->priv_dat->_write_gpio(line->priv_dat, sel);
}

/* "Private" Functions */
// Check the pin is in range
int32_t pin_in_range(uint8_t pin)
{
    // Very simply, check if greater than the max pin value
    return pin > PIN_MAX ? -1 : 0;
}
// Internal set gpio function 
int32_t __set_gpio_fn(_gpio_internals_t * __this_gpio_pdat,
                      enum FunctionSelect __fn_sel)
{
    /// LOCALS ///
    // Bit value that corresponds to the three bit value for function selection
    uint32_t fn_sel_bits    = 0x00;
    // The clear value, used to unset the bits in registers corollary to this pin
    uint32_t clear_bits     = 0x00;
    // Register state
    uint32_t register_state = 0x00;
    // The bit shift that will be used to set the appropriate bits
    uint32_t bit_shift      = 0x00;
    // The set return value
    int32_t set_retval = 0;

    // Check the private data is not NULL
    if (NULL == __this_gpio_pdat)
    {
        set_retval = EPDAT_NULL;
    }
    // Check that the pin is in range
    else if (-1 == pin_in_range(__this_gpio_pdat->_pin_value))
    {
        set_retval = EBAD_PIN;
    }

    if (0 == set_retval)
    {
        // Figure how far to shift the bits for setting function  
        // Explanation: Each FNSEL register is used for 10 pins, bits 0:2 belong to pin n,
        //              3:5 belong to pin n+1, ... 29:27 belong to pin n+9
        bit_shift   = ((__this_gpio_pdat->_pin_value % BASE_TEN) * 3);
        // The function selection bits, shifted for the appropriate pin
        fn_sel_bits = (__fn_sel << bit_shift); 
        // Clear bits, used to clear whatever was set in that register previously. Essentially unsetting
        // any bits that were set for the pin we want to modify.
        clear_bits  = ((0x03 << bit_shift) ^ __this_gpio_pdat->_3_bit_map_mask); 
        // Read the register, mask out the bits we don't care about to 0
        register_state = *(uint32_t *)__this_gpio_pdat->_fn_sel_reg & __this_gpio_pdat->_3_bit_map_mask; 
        // Clear the register of bits pertaining to our pin, then write with what we wish to set
        *(uint32_t *)__this_gpio_pdat->_fn_sel_reg = (register_state & clear_bits) | fn_sel_bits;
        /*                                           ^---- clear operation         ^--- set operation */
    }
    return set_retval;
}
// Internal write gpio line function 
int32_t __write_gpio(_gpio_internals_t * __this_gpio_pdat,
                     uint8_t high_low)
{
    /// LOCALS /// 
    // The write return value
    int32_t write_retval = 0;
    // Figure the bits that correspond to the function selection register
    uint32_t fn_sel_bit_shift = 0x00;
    // The bit shift for set/clear registers
    uint32_t setclr_bit_shift = 0x00;
    // The bit that will be set in the set/clear registers
    uint32_t setclr_bit       = 0x00;
    // The clear value, used to unset the bits in registers corollary to this pin
    uint32_t clear_bits       = 0x00;
    // Register state
    uint32_t register_state   = 0x00;
    // The bits to set
    uint32_t bits_set         = 0x00;

    // Check the private data is not NULL
    if (NULL == __this_gpio_pdat)
    {
        write_retval = EPDAT_NULL;
    }
    // Check that the pin is in range
    else if (-1 == pin_in_range(__this_gpio_pdat->_pin_value))
    {
        write_retval = EBAD_PIN;
    }
    else
    {
        // The function selection bit shift
        fn_sel_bit_shift = ((__this_gpio_pdat->_pin_value % BASE_TEN) * 3);
        // Check that the pin is configured with it's function as output
        if (OUTPUT != ((*(uint32_t *)__this_gpio_pdat->_fn_sel_reg >> fn_sel_bit_shift) & THREE_BIT_MASK))
        {
            write_retval = EPIN_CONFIG;
        }
        // Safety checks passing, write the pin
        else
        {
            // Bit shift for set clear
            setclr_bit_shift = (__this_gpio_pdat->_pin_value % PIN_MAX_BIT);
            // Figure the bit shift for set/clear
            setclr_bit = 0x01 << setclr_bit_shift;
            // Figure the bit to clear
            clear_bits  = ((0x01 << setclr_bit_shift) ^ __this_gpio_pdat->_1_bit_map_mask); 
            // If write high, then write the set register 
            if (1 == high_low)
            {
                // Read the register state, mask out bits we don't care about
                register_state = *(uint32_t *)__this_gpio_pdat->_set_reg & __this_gpio_pdat->_1_bit_map_mask;
                // Now, clear the corollary and set it appropriately 
                *(uint32_t *)__this_gpio_pdat->_set_reg = (register_state & clear_bits) | setclr_bit;
            }
            // If write low, then write the clear register
            else if (0 == high_low)
            {
                // Read the register state, mask out bits we don't care about
                register_state = *(uint32_t *)__this_gpio_pdat->_clr_reg & __this_gpio_pdat->_1_bit_map_mask;
                // Now, clear the corollary and set it appropriately 
                *(uint32_t *)__this_gpio_pdat->_clr_reg = (register_state & clear_bits) | setclr_bit;
            }
            // Otherwise the supplied value is out of range, don't write anything
            else
            {
                write_retval = EOUT_OF_RANGE;
            }
        }
    }
    return write_retval;
}

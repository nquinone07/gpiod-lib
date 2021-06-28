#ifndef SRC_GPIOD_H
#define SRC_GPIOD_H
#include <stdint.h> 

/// CONSTS & ENUMS ///
// Error Return Values 
#define EDEVMEM_OPEN  -1
#define EBAD_PIN      -2
#define EMALLOC       -3
#define EMAP_FAIL     -4
#define EPDAT_NULL    -5
#define EPIN_CONFIG   -6
#define EOUT_OF_RANGE -7
// Function Selection Bit Values
enum FunctionSelect {
   INPUT  = 0x00,
   OUTPUT = 0x01,
   ALT_0  = 0x04,
   ALT_1  = 0x05,
   ALT_2  = 0x06,
   ALT_3  = 0x07,
   ALT_4  = 0x03,
   ALT_5  = 0x02,
};
/// GPIO Structure ///
// "Private" struct encapsulating member data that is used to manipulate
// a gpio line (forward declaration)
typedef struct _gpio_internals _gpio_internals_t; 
// "Public" struct that represents the gpio line
typedef struct gpio_line {
    // GPIO name as a human readable value
    const char* gpio_name;
    // Private structure
    _gpio_internals_t* priv_dat;
} gpio_line_t;
/// FUNCTIONS ///
/* Request a GPIO line */
int32_t request_gpio_line(gpio_line_t*, uint8_t);
/* Write the GPIO Pin n, either high or low */
int32_t write_gpio(gpio_line_t *, uint8_t);
/* Set the pin function for the GPIO Pin n, use the enum above */
int32_t set_gpio_fn(gpio_line_t *, enum FunctionSelect);
#endif

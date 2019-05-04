/*s test LKM circuit assumes that the LEDs are attached to
 * GPIO 48 , GPIO 49, GPIO60, GPIO 112 which are on P9_15, P9_23, P9_12, P9_30 and the button is attached to GPIO 115 on 
 * P9_27. 
*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>       // Required for the GPIO functions
#include <linux/interrupt.h>  // Required for the IRQ code
#include <linux/device.h>  
#include <linux/fs.h>             // Header for the Linux file system support
#include <asm/uaccess.h>          // Required for the copy to user function
#include <linux/mutex.h>          //library to use mutexes in the kernel
#define  DEVICE_NAME "kernelDriver"    ///< The device will appear at /dev/kernelDriver using this value
#define  CLASS_NAME  "char"        ///< The device class -- this is a character device driver
#define NUM_OF_LEDS 4

MODULE_LICENSE("GPL");
MODULE_AUTHOR("MADINENI MANOGNA");
MODULE_DESCRIPTION("A Button/LED test driver for the BBB");
MODULE_VERSION("1.0");

static DEFINE_MUTEX(driver_mutex);  // declaring a mutex 

static unsigned int gpioLED[NUM_OF_LEDS] = {48,49,60,112};       ///< hard coding the LEDs to (GPIO_48,GPIO_49,GPIO_60,GPIO112).ie.,(P9_15,P9_23,P9_12,P9_30) 
static unsigned int gpioButton = 115;   ///< hard coding the button gpio for this example to P9_27 (GPIO115)
static unsigned int irqNumber;          ///< Used to share the IRQ number within this file
static bool	    ledOn = 0;          ///< Is the LED on or off? Used to invert its state (off by default)

static int    majorNumber;                  ///< Stores the device number -- determined automatically
static char   message[4] = {0};           ///< Memory for the string that is passed from userspace
static short  size_of_message;              ///< Used to remember the size of the string stored
static int    numberOpens = 0;              ///< Counts the number of times the device is opened
static struct class*  driverClass  = NULL; ///< The device-driver class struct pointer
static struct device* driverDevice = NULL; ///< The device-driver device struct pointer
static int result;

/// Function prototype for the custom IRQ handler function -- see below for the implementation
static irq_handler_t  ebbgpio_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs);

// The prototype functions for the character driver -- must come before the struct definition
static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

/** @brief Devices are represented as file structure in the kernel. The file_operations structure from
 *  /linux/fs.h lists the callback functions that you wish to associated with your file operations
 *  using a C99 syntax structure. char devices usually implement open, read, write and release calls
 */
static struct file_operations fops =
{
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};

/** @brief The LKM initialization function
 *  The static keyword restricts the visibility of the function to within this C file. The __init
 *  macro means that for a built-in driver (not a LKM) the function is only used at initialization
 *  time and that it can be discarded and its memory freed up after that point. In this example this
 *  function sets up the GPIOs and the IRQ
 *  @return returns 0 if successful
 */
static int __init ebbgpio_init(void){
   int i;
   printk(KERN_INFO "KDriver: Initializing the KDriver LKM\n");

   // Try to dynamically allocate a major number for the device -- more difficult but worth it
   majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
   if (majorNumber<0){
      printk(KERN_ALERT "KDriver failed to register a major number\n");
      return majorNumber;
   }
   printk(KERN_INFO "KDriver: registered correctly with major number %d\n", majorNumber);

   // Register the device class
   driverClass = class_create(THIS_MODULE, CLASS_NAME);
   if (IS_ERR(driverClass)){                // Check for error and clean up if there is
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to register device class\n");
      return PTR_ERR(driverClass);          // Correct way to return an error on a pointer
   }
   printk(KERN_INFO "KDriver: device class registered correctly\n");

   // Register the device driver
   driverDevice = device_create(driverClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
   if (IS_ERR(driverDevice)){               // Clean up if there is an error
      class_destroy(driverClass);          
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to create the device\n");
      return PTR_ERR(driverDevice);
   }

   printk(KERN_INFO "KDriver: device class created correctly\n");
   mutex_init(&driver_mutex);  //Initializes the mutex lock dynamically at runtime 
   printk(KERN_INFO "GPIO_TEST: Initializing the GPIO_TEST LKM\n");
   // Is the GPIO a valid GPIO number (e.g., the BBB has 4x32 but not all available)
   for(i=0;i<NUM_OF_LEDS;i++){
   if (!gpio_is_valid(gpioLED[i])){
      printk(KERN_INFO "GPIO_TEST: invalid LED GPIO\n");
      return -ENODEV;
   }
   }
   // Going to set up the LEDs .It is a GPIO in output mode and will be on by default
   ledOn = true;
   for(i=0;i<NUM_OF_LEDS;i++){
   gpio_request(gpioLED[i], "sysfs");          // gpioLEDs are  hardcoded to 48,49,60,112, request it
   gpio_direction_output(gpioLED[i], ledOn);   // Set the gpio to be in output mode and on
   gpio_export(gpioLED[i], false);             // Causes gpio48,gpio49,gpio60,gpio112 to appear in /sys/class/gpio
               		                       // the bool argument prevents the direction from being changed
   }
   gpio_request(gpioButton, "sysfs");       // Set up the gpioButton
   gpio_direction_input(gpioButton);        // Set the button GPIO to be an input
   gpio_set_debounce(gpioButton, 200);      // Debounce the button with a delay of 200ms
   gpio_export(gpioButton, false);          // Causes gpio115 to appear in /sys/class/gpio
			                    // the bool argument prevents the direction from being changed
   // Perform a quick test to see that the button is working as expected on LKM load
   printk(KERN_INFO "GPIO_TEST: The button state is currently: %d\n", gpio_get_value(gpioButton));

   // GPIO numbers and IRQ numbers are not the same! This function performs the mapping for us
   irqNumber = gpio_to_irq(gpioButton);
   printk(KERN_INFO "GPIO_TEST: The button is mapped to IRQ: %d\n", irqNumber);

   // This next call requests an interrupt line
   result = request_irq(irqNumber,             // The interrupt number requested
                        (irq_handler_t) ebbgpio_irq_handler, // The pointer to the handler function below
                        IRQF_TRIGGER_RISING,   // Interrupt on rising edge (button press, not release)
                        "ebb_gpio_handler",    // Used in /proc/interrupts to identify the owner
                        NULL);                 // The *dev_id for shared interrupt lines, NULL is okay

   printk(KERN_INFO "GPIO_TEST: The interrupt request result is: %d\n", result);
   return result;
   
}

/** @brief The LKM cleanup function
 *  Similar to the initialization function, it is static. The __exit macro notifies that if this
 *  code is used for a built-in driver (not a LKM) that this function is not required. Used to release the
 *  GPIOs and display cleanup messages.
 */
static void __exit ebbgpio_exit(void){
   int i;
   mutex_destroy(&driver_mutex); 	                   //Destroys the dynamically allocated memory used for the mutex
   device_destroy(driverClass, MKDEV(majorNumber, 0));     // remove the device
   class_unregister(driverClass);                          // unregister the device class
   class_destroy(driverClass);                             // remove the device class
   unregister_chrdev(majorNumber, DEVICE_NAME);             // unregister the major number
   printk(KERN_INFO "KDriver: Goodbye from the LKM!\n");
   printk(KERN_INFO "GPIO_TEST: The button state is currently: %d\n", gpio_get_value(gpioButton));
   for(i=0;i<NUM_OF_LEDS;i++){
        gpio_set_value(gpioLED[i], 0);              // Turn the LED off, makes it clear the device was unloaded
        gpio_unexport(gpioLED[i]); 
    }   // Unexport the LED GPIO
   free_irq(irqNumber, NULL);               // Free the IRQ number, no *dev_id required in this case
   gpio_unexport(gpioButton);               // Unexport the Button GPIO
   for(i=0;i<NUM_OF_LEDS;i++){
   	gpio_free(gpioLED[i]);                      // Free the LED GPIO
  }  
   gpio_free(gpioButton);                   // Free the Button GPIO
   printk(KERN_INFO "GPIO_TEST: Goodbye from the LKM!\n");
   
}

/** @brief The GPIO IRQ Handler function
 *  This function is a custom interrupt handler that is attached to the GPIO above. The same interrupt
 *  handler cannot be invoked concurrently as the interrupt line is masked out until the function is complete.
 *  This function is static as it should not be invoked directly from outside of this file.
 *  @param irq    the IRQ number that is associated with the GPIO -- useful for logging.
 *  @param dev_id the *dev_id that is provided -- can be used to identify which device caused the interrupt
 *  Not used in this example as NULL is passed.
 *  @param regs   h/w specific register values -- only really ever used for debugging.
 *  return returns IRQ_HANDLED if successful -- should return IRQ_NONE otherwise.
 */
static irq_handler_t ebbgpio_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs){
   int i;
   ledOn = !ledOn;                          // Invert the LED state on each button press
   for(i=0;i<NUM_OF_LEDS;i++){
        gpio_set_value(gpioLED[i],ledOn);          // Set the physical LED accordingly
   }
   printk(KERN_INFO "GPIO_TEST: Interrupt! (button state is %d)\n", gpio_get_value(gpioButton));
   return (irq_handler_t) IRQ_HANDLED;      // Announce that the IRQ has been handled correctly
   
}
static int dev_open(struct inode *inodep, struct file *filep){
   mutex_lock(&driver_mutex);    //locks the mutex and begins access to the critical region
   numberOpens++;
   printk(KERN_INFO "KDriver: Device has been opened %d time(s)\n", numberOpens);
   return 0;
}
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
   int error_count = 0;
   // copy_to_user has the format ( * to, *from, size) and returns 0 on success
   error_count = copy_to_user(buffer, message, size_of_message);

   if (error_count==0){            // if true then have success
      printk(KERN_INFO "KDriver: Sent %d characters to the user\n", size_of_message);
      return (size_of_message=0);  // clear the position to the start and return 0
   }
   else {
      printk(KERN_INFO "KDriver: Failed to send %d characters to the user\n", error_count);
      return -EFAULT;              // Failed -- return a bad address message (i.e. -14)
   }
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
  int i;
  ledOn=true;   
  sprintf(message, "%s(%zu letters)", buffer, len);   // appending received string with its length
   for(i=0;i<NUM_OF_LEDS;i++){
      if(message[i]=='0'){                //mapping each digit of input string in order to write to the device.
   	gpio_set_value(gpioLED[i], 0);       
   }
   else{
   	gpio_set_value(gpioLED[i], ledOn);          
   }
 }  
   size_of_message = strlen(message);                 // store the length of the stored message
   printk(KERN_INFO "KDriver: Received %zu characters from the user\n", len);
   return len;
}

static int dev_release(struct inode *inodep, struct file *filep){
   mutex_unlock(&driver_mutex);      //unlocks the mutex and allows another process to enter the critical region
   printk(KERN_INFO "KDriver: Device successfully closed\n");
   return 0;
}
/// This next calls are  mandatory -- they identify the initialization function
/// and the cleanup function (as above).
module_init(ebbgpio_init);
module_exit(ebbgpio_exit);


















#ifndef GPIO_CLASS_H
#define GPIO_CLASS_H
#include <string>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

namespace GPIOclass
{
    /* GPIO Class

    * Purpose: Each object instantiated from this class will control a GPIO pin

    * The GPIO pin number must be passed to the overloaded class constructor

    */
    class GPIOClass
    {
    private:
        GPIOClass(GPIOClass const &) = delete; // no copy operations
        GPIOClass(GPIOClass &&) = delete;
        GPIOClass &operator=(const GPIOClass &) = delete; // copy assignment: clean up target and copy -> no allow
        GPIOClass &operator=(GPIOClass &&) = delete;      // move assignment: clean up target and copy -> no allow
        string gpionum; // GPIO number associated with the instance of an object

    public:
        GPIOClass() noexcept;
        // create a GPIO object that controls GPIO4 (default)

        GPIOClass(string) noexcept;
        // create a GPIO object that controls GPIOx, where x is passed to this constructor
        
        ~GPIOClass() = default;                // destructor: clean up
        int export_gpio() noexcept;
        // exports GPIO

        int unexport_gpio() noexcept;
        // unexport GPIO

        int setdir_gpio(string) noexcept;
        // Set GPIO Direction

        int setval_gpio(string) noexcept;
        // Set GPIO Value (putput pins)

        int getval_gpio(string &) noexcept;
        // Get GPIO Value (input/ output pins)

        string get_gpionum() noexcept;
        // return the GPIO number associated with the instance of an object


    };

    GPIOClass::GPIOClass() noexcept : gpionum{"4"}
    {
        // this->gpionum = "4";
        // // GPIO4 is default
    }

    GPIOClass::GPIOClass(string gnum) noexcept : gpionum{gnum}
    {
        // this->gpionum = gnum;
        // // Instatiate GPIOClass object for GPIO pin number "gnum"
    }

    inline int GPIOClass::export_gpio() noexcept
    {
        string export_str = "/sys/class/gpio/export";

        ofstream exportgpio(export_str.c_str());
        // Open "export" file. Convert C++ string to C string. Required for all Linux pathnames

        // if (exportgpio < 0)
        if (!exportgpio.is_open())
        {
            cout << " OPERATION FAILED: Unable to export GPIO"
                 << this->gpionum << " ."
                 << endl;
            return -1;
        }
        exportgpio << this->gpionum;
        // write GPIO number to export

        exportgpio.close();
        // close export file

        return 0;
    }

    inline int GPIOClass::unexport_gpio() noexcept
    {
        string unexport_str = "/sys/class/gpio/unexport";

        ofstream unexportgpio(unexport_str.c_str());
        // Open unexport file

        if (!unexportgpio.is_open())
        {
            cout << " OPERATION FAILED: Unable to unexport GPIO"
                 << this->gpionum << " ."
                 << endl;

            return -1;
        }
        unexportgpio << this->gpionum;
        // write GPIO number to unexport

        unexportgpio.close();
        // close unexport file

        return 0;
    }
    inline int GPIOClass::setdir_gpio(string dir) noexcept
    {

        string setdir_str = "/sys/class/gpio/gpio" + this->gpionum + "/direction";

        ofstream setdirgpio(setdir_str.c_str());
        // open direction file for gpio

        if (!setdirgpio.is_open())
        {
            cout << " OPERATION FAILED: Unable to set direction of GPIO"
                 << this->gpionum
                 << " ."
                 << endl;

            return -1;
        }

        setdirgpio << dir;
        // write direction to direction file

        setdirgpio.close();
        // close direction file

        return 0;
    }

    inline int GPIOClass::setval_gpio(string val) noexcept
    {
        string setval_str = "/sys/class/gpio/gpio" + this->gpionum + "/value";

        ofstream setvalgpio(setval_str.c_str());
        // open value file for gpio

        if (!setvalgpio.is_open())
        {
            cout << " OPERATION FAILED: Unable to set the value of GPIO"
                 << this->gpionum << " ."
                 << endl;

            return -1;
        }

        setvalgpio << val;
        // write value to value file

        setvalgpio.close();
        // close value file

        return 0;
    }

    inline int GPIOClass::getval_gpio(string &val) noexcept
    {
        string getval_str = "/sys/class/gpio/gpio" + this->gpionum + "/value";

        ifstream getvalgpio(getval_str.c_str());
        // open value file for gpio

        if (!getvalgpio.is_open())
        {
            cout << " OPERATION FAILED: Unable to get value of GPIO"
                 << this->gpionum << " ."
                 << endl;

            return -1;
        }

        getvalgpio >> val;
        // read gpio value

        if (val != "0")
            val = "1";
        else
            val = "0";

        getvalgpio.close();
        // close the value file

        return 0;
    }

    inline string GPIOClass::get_gpionum() noexcept
    {
        return this->gpionum;
    }

}
#endif
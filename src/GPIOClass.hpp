#include <fstream>
#include <string>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

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
        std::string gpionum;                              // GPIO number associated with the instance of an object
        int8_t write2file(std::string const &, std::string const &) noexcept;

    public:
        GPIOClass() noexcept;    // create a GPIO object that controls GPIO4 (default)
        GPIOClass(std::string) noexcept;     // create a GPIO object that controls GPIOx, where x is passed to this constructor
        ~GPIOClass() = default; // destructor: clean up
        int8_t export_gpio() noexcept;     // exports GPIO
        int8_t unexport_gpio() noexcept;    // unexport GPIO
        int8_t setdir_gpio(std::string) noexcept;     // Set GPIO Direction
        int8_t setval_gpio(std::string) noexcept;     // Set GPIO Value (putput pins)
        int8_t getval_gpio(std::string &) noexcept;   // Get GPIO Value (input/ output pins)
        std::string get_gpionum() noexcept;   // return the GPIO number associated with the instance of an object
    };

    GPIOClass::GPIOClass() noexcept : gpionum{"4"}
    {
    }

    GPIOClass::GPIOClass(std::string gnum) noexcept : gpionum{gnum}
    {
    }

    inline int8_t GPIOClass::write2file(std::string const &a_path, std::string const &a_str) noexcept
    {
        int8_t ret{0};
        std::ofstream file(a_path, std::ofstream::out);
        if (file.is_open())
        {
            file << a_str;
        }
        else
        {
            std::cerr << " Could not open " << a_path << "." << std::endl;
            // exit(1);
            ret = 1;
        }
        file.flush();
        file.close();
        return ret;
    }

    inline int8_t GPIOClass::export_gpio() noexcept
    {
        int8_t ret;
        std::string const export_str = "/sys/class/gpio/export";
        ret = write2file(export_str, gpionum);
        return ret;
    }

    inline int8_t GPIOClass::unexport_gpio() noexcept
    {
        int8_t ret;
        std::string const unexport_str = "/sys/class/gpio/unexport";
        ret = write2file(unexport_str, gpionum);
        return ret;
    }

    inline int8_t GPIOClass::setdir_gpio(std::string dir) noexcept
    {
        int8_t ret;
        std::string const setdir_str = "/sys/class/gpio/gpio" + gpionum + "/direction";
        ret = write2file(setdir_str, dir);
        return ret;
    }

    inline int8_t GPIOClass::setval_gpio(std::string val) noexcept
    {
        int8_t ret;
        std::string const setval_str = "/sys/class/gpio/gpio" + gpionum + "/value";
        ret = write2file(setval_str, val);
        return ret;
    }

    inline int8_t GPIOClass::getval_gpio(std::string &val) noexcept
    {
        std::string const getval_str = "/sys/class/gpio/gpio" + gpionum + "/value";

        std::ifstream getvalgpio(getval_str.c_str());
        // open value file for gpio

        if (!getvalgpio.is_open())
        {
            std::cout << " OPERATION FAILED: Unable to get value of GPIO" << gpionum << " ." << std::endl;

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

    inline std::string GPIOClass::get_gpionum() noexcept
    {
        return gpionum;
    }

}

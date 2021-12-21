/*
 * Copyright (C) 2021 Diego Jauregui
 * Grupo Minero Bonanza
 */

#include "cluon-complete.hpp"
#include "opendlv-standard-message-set.hpp"

#include <cstdint>
#include <cmath>
#include <iostream>
#include <memory>
#include <iterator>
#include <vector>
#include <ctime>
#include <string>
#include "GPIOClass.hpp"

int32_t main(int32_t argc, char **argv)
{
  int32_t retCode{1};
  auto commandlineArguments = cluon::getCommandlineArguments(argc, argv);
  if ((0 == commandlineArguments.count("cid")) || (0 == commandlineArguments.count("hysterisis-time")) || (0 == commandlineArguments.count("input-id")) || (0 == commandlineArguments.count("alarm-gpio-pin")) || (0 == commandlineArguments.count("initial-state")))
  {
    std::cerr << argv[0] << " activate an physical alarm connected into GPIO pin of a RPI."
              << std::endl;
    std::cerr << "Usage:   " << argv[0] << " --cid=<OD4 session> "
              << "--hysterisis-time=<time in seconds> "
              << "--initial-state=<0 or 1> of the alarm device"
              << "--alarm-gpio-pin=<pin of the RPI where alarm device is connected> "
              << "--input-id=<senderStamp of sender> "
              << "[--verbose] [--debug]" << std::endl;
  }
  else
  {
    bool const debug{commandlineArguments.count("debug") != 0};
    bool const verbose{commandlineArguments.count("verbose") != 0};
    std::string const alarm_pin{commandlineArguments["alarm-gpio-pin"]};
    uint32_t const senderId{static_cast<uint32_t>(std::stoi(commandlineArguments["input-id"]))};
    int32_t const hysterisisTime{std::stoi(commandlineArguments["hysterisis-time"])};
    float const freq{1.0f / hysterisisTime};

    cluon::OD4Session od4{
        static_cast<uint16_t>(std::stoi(commandlineArguments["cid"]))};

    std::mutex alarmStateMutex;

    std::string alarmState{commandlineArguments["initial-state"]};
    std::string prevAlarmState{alarmState};

    GPIOclass::GPIOClass rpi_gpio(alarm_pin);
    rpi_gpio.export_gpio(); // exports GPIO
    rpi_gpio.setdir_gpio("out"); // Set GPIO Direction, como salida

    auto onSwitchStateRequest{[&alarmStateMutex, &alarmState, &senderId](cluon::data::Envelope &&envelope)
                              {
                                uint32_t senderStamp = envelope.senderStamp();
                                auto msg = cluon::extractMessage<opendlv::proxy::SwitchStateRequest>(std::move(envelope));
                                if (senderStamp == senderId)
                                {
                                  std::lock_guard<std::mutex> lck(alarmStateMutex);
                                  alarmState = msg.state();
                                }
                              }};

    auto atFrequency{
        [&od4, &rpi_gpio,
        &prevAlarmState, &alarmState,
         &verbose, &debug, &senderId]() -> bool
        {
          // Extrae la hora de la fecha actual
          time_t actualTime = time(0);
          char *formatTime = ctime(&actualTime);
          std::cout << "The current local date and time is: " << formatTime << std::endl;
          struct tm *t_local = localtime(&actualTime);
          std::cout << "The current local date and time is: " << asctime(t_local) << std::endl;
          int32_t hh_local = t_local->tm_hour;
          std::cout << "La hora es: " << hh_local << std::endl;

          if (prevAlarmState == "0" && alarmState == "0")
          {
            rpi_gpio.setval_gpio("0"); // Set GPIO Value (putput pins)
            std::clog << "Pin value is set to 0." << std::endl;
          }
          else if (prevAlarmState == "0" && alarmState == "1")
          {
            rpi_gpio.setval_gpio("1"); // Set GPIO Value (putput pins)
            std::clog << "Pin value is set to 1." << std::endl;
          }
          else if (prevAlarmState == "1" && alarmState == "0")
          {
            rpi_gpio.setval_gpio("0"); // Set GPIO Value (putput pins)
            std::clog << "Pin value is set to 0." << std::endl;
          }
          else if (prevAlarmState == "1" && alarmState == "1")
          {
            rpi_gpio.setval_gpio("1"); // Set GPIO Value (putput pins)
            std::clog << "Pin value is set to 1." << std::endl;
          }
          else
          {
            std::cerr << "Estado desconocido" << std::endl;
          }

        return true;
        }};


    od4.dataTrigger(opendlv::proxy::SwitchStateRequest::ID(),
                    onSwitchStateRequest);

    od4.timeTrigger(freq, atFrequency);

    // unexport GPIO
    rpi_gpio.unexport_gpio();
    retCode = 0;
  }
  return retCode;
}

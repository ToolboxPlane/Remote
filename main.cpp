//
// Created by paul on 20.01.18.
//
#include <avr/io.h>
extern "C" {
    #include "hal/ili9341.h"
    #include "hal/ili9341gfx.h"
    #include "hal/stmpe610.h"
    #include "drivers/adc.h"
    #include "drivers/uart.h"
}

#include "hal/LoRa.h"
#include "RadioControlProtocol/rcLib.hpp"
#include "util/Joystick.hpp"
#include "ui.h"
#include "controller.h"

Joystick joyRight;
Joystick joyLeft;

int main() {
    rcLib::Package pkgOut(256, 8);
    rcLib::Package pkgUartIn;

    controller::load();
    adc_init();
    uart_init(9600);
    joyLeft.loadConfiguration(0);
    joyRight.loadConfiguration(16);

    controller::setDebug(0, LoRa.begin((long)434E6)?1:0);
    rcLib::Package::transmitterId = 17;
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while(true) {
        controller::handleEvent(controller::getSelection());
        joyLeft.setXValue(adc_read(1));
        joyLeft.setYValue(adc_read(2));
        joyRight.setXValue(adc_read(4));
        joyRight.setYValue(adc_read(5));

        pkgOut.setChannel(joyLeft.getXChannel(), joyLeft.getXValue()+127);
        pkgOut.setChannel(joyLeft.getYChannel(), joyLeft.getYValue()+127);
        pkgOut.setChannel(joyLeft.getBtnChannel(), joyLeft.getButton());
        pkgOut.setChannel(joyRight.getXChannel(), joyRight.getXValue()+127);
        pkgOut.setChannel(joyRight.getYChannel(), joyRight.getYValue()+127);
        pkgOut.setChannel(joyRight.getBtnChannel(), joyRight.getButton());
        pkgOut.setChannel(model::armedChannnel, model::armed);
        pkgOut.setChannel(model::flightmodeChannel, model::flightmode);
        uint8_t outLen = pkgOut.encode();

        if(model::serialEnabled()) {
            uart_send_buffer(pkgOut.getEncodedData(), outLen);
            while (uart_available()) {
                if(pkgUartIn.decode(uart_read())) {
                   /* controller::setDebug(0, pkgUartIn.getChannel(0));
                    controller::setDebug(1, pkgUartIn.getChannel(1));
                    controller::setDebug(2, pkgUartIn.getChannel(2));
                    controller::setDebug(3, pkgUartIn.getChannel(3));
                    controller::setDebug(4, pkgUartIn.getChannel(4));
                    controller::setDebug(5, pkgUartIn.getChannel(5));*/
                }
            }
        }
        if(model::loraEnabled()) {
            LoRa.disableCrc();
            LoRa.beginPacket();
            LoRa.write(pkgOut.getEncodedData(), outLen);
            controller::setDebug(1,LoRa.endPacket()?1:0);
        }
    }
#pragma clang diagnostic pop
}

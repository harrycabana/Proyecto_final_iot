/*! @file : MCUX_FRDM_KL02Z_IoT_RTU_demo.c
 * @author  Harrison Cabana
 * @version 1.0.0
 * @date    8/01/2021
 * @brief   Funcion principal main
 * @details
 *
 *
 */
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL02Z4.h"
#include "fsl_debug_console.h"

#include "sdk_hal_uart0.h"
#include "sdk_hal_gpio.h"
#include "sdk_hal_i2c0.h"
#include "sdk_hal_i2c1.h"
#include "sdk_hal_adc.h"

#include "sdk_mdlw_leds.h"
#include "sdk_pph_ec25au.h"
#include "sdk_pph_bme280.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define HABILITAR_MODEM_EC25		1
#define HABILITAR_SENSOR_BME280		1


/*******************************************************************************
 * Private Prototypes
 ******************************************************************************/

/*******************************************************************************
 * External vars
 ******************************************************************************/

/*******************************************************************************
 * Local vars
 ******************************************************************************/

/*******************************************************************************
 * Private Source Code
 ******************************************************************************/
void waytTime(void) {
	uint32_t tiempo = 0x1FFFF;
	do {
		tiempo--;
	} while (tiempo != 0x0000);
}

/*
 * @brief   Application entry point.
 */
int main(void) {

	uint8_t ec25_estado_actual;

    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();

#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    BOARD_InitDebugConsole();
#endif

    printf("Inicializa UART0:");
    //inicializa puerto UART0 y solo avanza si es exitoso el proceso
    if(uart0Inicializar(115200)!=kStatus_Success){	//115200bps
    	printf("Error");
    	return 0 ;
    };
    printf("OK\r\n");

    printf("Inicializa I2C0:");
    //inicializa puerto I2C0 y solo avanza si es exitoso el proceso
    if(i2c0MasterInit(100000)!=kStatus_Success){	//100kbps
    	printf("Error");
    	return 0 ;
    }
    printf("OK\r\n");

    printf("Inicializa I2C1:");
    //inicializa puerto I2C1 y solo avanza si es exitoso el proceso
    if(i2c1MasterInit(100000)!=kStatus_Success){	//100kbps
    	printf("Error");
    	return 0 ;
    }
    printf("OK\r\n");



#if HABILITAR_SENSOR_BME280

    bool inicio = Bme280_Begin(0x76);//inicializa sensor bme280

#endif

#if HABILITAR_MODEM_EC25
    //Inicializa todas las funciones necesarias para trabajar con el modem EC25
    printf("Inicializa modem EC25\r\n");
    ec25Inicializacion();

#endif

	//Ciclo infinito encendiendo y apagando led verde
	//inicia SUPERLOOP
    while(1) {
    	waytTime();		//base de tiempo fija aproximadamente 200ms



#if HABILITAR_MODEM_EC25
    	ec25_estado_actual = ec25Polling();	//actualiza maquina de estados encargada de avanzar en el proceso interno del MODEM
											//retorna el estado actual de la FSM

		switch(ec25_estado_actual){			//controla color de los LEDs dependiendo de estado modemEC25
    	case kFSM_RESULTADO_ERROR:
    		toggleLedRojo();
    		apagarLedVerde();
    		apagarLedAzul();
    		ec25Inicializacion();
    		break;

    	case kFSM_RESULTADO_EXITOSO:
    		apagarLedRojo();
    		toggleLedVerde();
    		apagarLedAzul();
    		ec25Inicializacion();
    		break;

    	default:
    		apagarLedRojo();
    		apagarLedVerde();
    		toggleLedAzul();
    		break;
    	}
    }
#endif
    return 0 ;
}

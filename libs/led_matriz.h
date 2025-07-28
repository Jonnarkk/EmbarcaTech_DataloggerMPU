#ifndef LED_MATRIZ_H
#define LED_MATRIZ_H

#include "pio_matriz.pio.h"

#define pino_matriz 7
#define NUM_PIXELS 25


// Protótipos das funções existentes
uint32_t matrix_rgb(double b, double r, double g);
void desenho_pio(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b);
void ligar_matriz_toda(PIO pio, uint sm, double r, double g, double b);
void apagar_matriz(PIO pio, uint sm);


uint pio_init(PIO pio);

#endif // LED_MATRIZ_H
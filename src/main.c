#include <stdint.h>
#include <stdlib.h>

#include "sfunc.h"

const char msg[] = "Hello from nucleo_l011k4 !";

volatile __attribute__((used)) uint32_t var1 = 0x12345678U;
volatile __attribute__((used, section(".noinit"))) uint32_t var2;

uint32_t i = 0;

int main(void)
{
	for (;;) {
		i = _sinc(i);
	}
}
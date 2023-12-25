#include "test/minunit.h"
#include <stdio.h>
#include <stdio.h>
#include <Windows.h>
#include <process.h>
#include "test/test_com_init.c"
#include "test/test_com_uart_communite.c"
#include "test/test_com_iic_communite.c"
#include "test/test_com_spi_communite.c"


int main(int argc, char *argv[]) {
    printf("TEST START!\n");
    MU_RUN_SUITE(test_init_suite);
    MU_RUN_SUITE(test_uart_suite);
    MU_RUN_SUITE(test_iic_suite);
    MU_RUN_SUITE(test_spi_suite);
    MU_REPORT();


    return 0;
}

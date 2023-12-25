cmake_minimum_required(VERSION 3.26)
project(com_workQueue C)

set(CMAKE_C_STANDARD 11)

add_executable(com_workQueue main.c
        src/com_wq.c
        src/kfifo.c
        src/klist.h
        example/com.c
        example/com.h
        example/com_config.h
        example/com_port.c
        example/com_port.h
        test/test_com_init.c
        test/test_com_uart_communite.c
        test/test_com_iic_communite.c
        test/test_com_spi_communite.c
        test/minunit.h
        example/com_port_tmple.c
        example/com_port_tmple.h
)

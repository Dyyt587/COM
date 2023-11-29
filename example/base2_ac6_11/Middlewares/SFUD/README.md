# SFUD (Serial Flash Universal Driver) 串行 Flash 通用驱动库

---

## 0、SFUD 是什么

[SFUD](https://github.com/armink/SFUD) 是一款开源的串行 SPI Flash 通用驱动库。由于现有市面的串行 Flash 种类居多，各个 Flash 的规格及命令存在差异， SFUD 就是为了解决这些 Flash 的差异现状而设计，让我们的产品能够支持不同品牌及规格的 Flash，提高了涉及到 Flash 功能的软件的可重用性及可扩展性，同时也可以规避 Flash 缺货或停产给产品所带来的风险。

- 主要特点：支持 SPI/QSPI 接口、面向对象（同时支持多个 Flash 对象）、可灵活裁剪、扩展性强、支持 4 字节地址
- 资源占用
  - 标准占用：RAM:0.2KB ROM:5.5KB
  - 最小占用：RAM:0.1KB ROM:3.6KB
- 设计思路：
  - **什么是 SFDP** ：它是 JEDEC （固态技术协会）制定的串行 Flash 功能的参数表标准，最新版 V1.6B （[点击这里查看](https://www.jedec.org/standards-documents/docs/jesd216b)）。该标准规定了，每个 Flash 中会存在一个参数表，该表中会存放 Flash 容量、写粒度、擦除命令、地址模式等 Flash 规格参数。目前，除了部分厂家旧款 Flash 型号会不支持该标准，其他绝大多数新出厂的 Flash 均已支持 SFDP 标准。所以该库在初始化时会优先读取 SFDP 表参数。
  - **不支持 SFDP 怎么办** ：如果该 Flash 不支持 SFDP 标准，SFUD 会查询配置文件 ( [`/sfud/inc/sfud_flash_def.h`](https://github.com/armink/SFUD/blob/4bee2d0417a7ce853cc7aa3639b03fe825611fd9/sfud/inc/sfud_flash_def.h#L116-L142) ) 中提供的 **Flash 参数信息表** 中是否支持该款 Flash。如果不支持，则可以在配置文件中添加该款 Flash 的参数信息（添加方法详细见 [2.5 添加库目前不支持的 Flash](#25-添加库目前不支持的-flash)）。获取到了 Flash 的规格参数后，就可以实现对 Flash 的全部操作。

## 1、为什么选择 SFUD

- 避免项目因 Flash 缺货、Flash 停产或产品扩容而带来的风险；
- 越来越多的项目将固件存储到串行 Flash 中，例如：ESP8266 的固件、主板中的 BIOS 及其他常见电子产品中的固件等等，但是各种 Flash 规格及命令不统一。使用 SFUD 即可避免，在相同功能的软件平台基础下，无法适配不同 Flash 种类的硬件平台的问题，提高软件的可重用性；
- 简化软件流程，降低开发难度。现在只需要配置好 SPI 通信，即可畅快的开始玩串行 Flash 了；
- 可以用来制作 Flash 编程器/烧写器

## 2、SFUD 如何使用

### 2.1 已支持 Flash 

下表为所有已在 Demo 平台上进行过真机测试过的 Flash。显示为 **不支持** SFDP 标准的 Flash 已经在 Flash 参数信息表中定义，更多不支持 SFDP 标准的 Flash 需要大家以后 **共同来完善和维护**  **([Github](https://github.com/armink/SFUD)|[OSChina](http://git.oschina.net/armink/SFUD)|[Coding](https://coding.net/u/armink/p/SFUD/git))** 。

如果觉得这个开源项目很赞，可以点击 [项目主页](https://github.com/armink/SFUD) 右上角的 **Star** ，同时把它推荐给更多有需要的朋友。

|型号|制造商|容量|最高速度|SFDP  标准|QSPI 模式|备注|
|:--:|:----:|:--:|:--:|:--:|:--:|----|
|[W25Q40BV](http://microchip.ua/esp8266/W25Q40BV(EOL).pdf)|Winbond|4Mb|50Mhz|不支持|双线|已停产|
|[W25Q80DV](http://www.winbond.com/resource-files/w25q80dv_revg_07212015.pdf)|Winbond|8Mb|104Mhz|支持|双线||
|[W25Q16BV](https://media.digikey.com/pdf/Data%20Sheets/Winbond%20PDFs/W25Q16BV.pdf)|Winbond|16Mb|104Mhz|不支持|双线| by [slipperstree](https://github.com/slipperstree)|
|[W25Q16CV](http://www.winbond.com/resource-files/da00-w25q16cvf1.pdf)|Winbond|16Mb|104Mhz|支持|未测试||
|[W25Q16DV](http://www.winbond.com/resource-files/w25q16dv%20revk%2005232016%20doc.pdf)|Winbond|16Mb|104Mhz|支持|未测试| by [slipperstree](https://github.com/slipperstree)|
|[W25Q32BV](http://www.winbond.com/resource-files/w25q32bv_revi_100413_wo_automotive.pdf)|Winbond|32Mb|104Mhz|支持|双线||
|[W25Q64CV](http://www.winbond.com/resource-files/w25q64cv_revh_052214[2].pdf)|Winbond|64Mb|80Mhz|支持|四线||
|[W25Q128BV](http://www.winbond.com/resource-files/w25q128bv_revh_100313_wo_automotive.pdf)|Winbond|128Mb|104Mhz|支持|四线||
|[W25Q256FV](http://www.winbond.com/resource-files/w25q256fv%20revi%2002262016%20kms.pdf)|Winbond|256Mb|104Mhz|支持|四线||
|[MX25L3206E](http://www.macronix.com/Lists/DataSheet/Attachments/3199/MX25L3206E,%203V,%2032Mb,%20v1.5.pdf)|Macronix|32Mb|86MHz|支持|双线||
|[MX25L3233F](https://www.macronix.com/Lists/Datasheet/Attachments/8754/MX25L3233F,%203V,%2032Mb,%20v1.7.pdf)|Macronix|32Mb|133MHz|支持|未测试|by [JiapengLi](https://github.com/JiapengLi)|
|[KH25L4006E](http://www.macronix.com.hk/Lists/Datasheet/Attachments/117/KH25L4006E.pdf)|Macronix|4Mb|86Mhz|支持|未测试| by [JiapengLi](https://github.com/JiapengLi)|
|[KH25L3206E](http://www.macronix.com.hk/Lists/Datasheet/Attachments/131/KH25L3206E.pdf)|Macronix|32Mb|86Mhz|支持|双线||
|[SST25VF016B](http://ww1.microchip.com/downloads/en/DeviceDoc/20005044C.pdf)|Microchip|16Mb|50MHz|不支持|不支持| SST 已被 Microchip 收购|
|[M25P40](https://www.micron.com/~/media/documents/products/data-sheet/nor-flash/serial-nor/m25p/m25p40.pdf)|Micron|4Mb|75Mhz|不支持|未测试| by [redocCheng](https://github.com/redocCheng)|
|[M25P80](https://www.micron.com/~/media/documents/products/data-sheet/nor-flash/serial-nor/m25p/m25p80.pdf)|Micron|8Mb|75Mhz|不支持|未测试| by [redocCheng](https://github.com/redocCheng)|
|[M25P32](https://www.micron.com/~/media/documents/products/data-sheet/nor-flash/serial-nor/m25p/m25p32.pdf)|Micron|32Mb|75Mhz|不支持|不支持||
|[EN25Q32B](http://www.kean.com.au/oshw/WR703N/teardown/EN25Q32B%2032Mbit%20SPI%20Flash.pdf)|EON|32Mb|104MHz|不支持|未测试||
|[GD25Q16B](http://www.gigadevice.com/product/detail/5/410.html)|GigaDevice|16Mb|120Mhz|不支持|未测试| by [TanekLiang](https://github.com/TanekLiang) |
|[GD25Q32C](http://www.gigadevice.com/product/detail/5/410.html)|GigaDevice|32Mb|120Mhz|不支持|未测试| by [gaupen1186](https://github.com/gaupen1186) |
|[GD25Q64B](http://www.gigadevice.com/product/detail/5/364.html)|GigaDevice|64Mb|120Mhz|不支持|双线||
|[S25FL216K](http://www.cypress.com/file/197346/download)|Cypress|16Mb|65Mhz|不支持|双线||
|[S25FL032P](http://www.cypress.com/file/196861/download)|Cypress|32Mb|104Mhz|不支持|未测试| by [yc_911](https://gitee.com/yc_911) |
|[S25FL164K](http://www.cypress.com/file/196886/download)|Cypress|64Mb|108Mhz|支持|未测试||
|[A25L080](http://www.amictechnology.com/datasheets/A25L080.pdf)|AMIC|8Mb|100Mhz|不支持|双线||
|[A25LQ64](http://www.amictechnology.com/datasheets/A25LQ64.pdf)|AMIC|64Mb|104Mhz|支持|支持||
|[F25L004](http://www.esmt.com.tw/db/manager/upload/f25l004.pdf)|ESMT|4Mb|100Mhz|不支持|不支持||
|[PCT25VF016B](http://pctgroup.com.tw/attachments/files/files/248_25VF016B-P.pdf)|PCT|16Mb|80Mhz|不支持|不支持|SST 授权许可，会被识别为 SST25VF016B|
|[AT45DB161E](http://www.adestotech.com/wp-content/uploads/doc8782.pdf)|ADESTO|16Mb|85MHz|不支持|不支持|ADESTO 收购 Atmel 串行闪存产品线|
|[NM25Q128EV](http://www.normem.com/product/278082170)|Nor_Mem|128Mb|未测试|不支持|未测试|SFDP可能会读取到信息后识别为超过32Gb|

> 注：QSPI 模式中，双线表示支持双线快读，四线表示支持四线快读。
>
> 一般情况下，支持四线快读的 FLASH 也支持双线快读。

### 2.2 API 说明

先说明下本库主要使用的一个结构体 `sfud_flash` 。其定义位于 `/sfud/inc/sfud_def.h`。每个 SPI Flash 会对应一个该结构体，该结构体指针下面统称为 Flash 设备对象。初始化成功后在 `sfud_flash->chip` 结构体中会存放 SPI Flash  的常见参数。如果  SPI Flash  还支持 SFDP ，还可以通过  `sfud_flash->sfdp` 看到更加全面的参数信息。以下很多函数都将使用 Flash 设备对象作为第一个入参，实现对指定 SPI Flash 的操作。

#### 2.2.1 初始化 SFUD 库

将会调用 `sfud_device_init` ，初始化 Flash 设备表中的全部设备。如果只有一个 Flash 也可以只使用 `sfud_device_init` 进行单一初始化。

> **注意**：初始化完的 SPI Flash 默认都 **已取消写保护** 状态，如需开启写保护，请使用 sfud_write_status 函数修改 SPI Flash 状态。

```C
sfud_err sfud_init(void)
```

#### 2.2.2 初始化指定的 Flash 设备

```C
sfud_err sfud_device_init(sfud_flash *flash)
```

|参数                                    |描述|
|:-----                                  |:----|
|flash                                   |待初始化的 Flash 设备|

#### 2.2.3 使能快速读模式（仅当 SFUD 开启 QSPI 模式后可用）

当 SFUD 开启 QSPI 模式后，SFUD 中的 Flash 驱动支持使用 QSPI 总线进行通信。相比传统的 SPI 模式，使用 QSPI 能够加速 Flash 数据的读取，但当数据需要写入时，由于 Flash 本身的数据写入速度慢于 SPI 传输速度，所以 QSPI 模式下的数据写入速度提升并不明显。

所以 SFUD 对于 QSPI 模式的支持仅限于快速读命令。通过该函数可以配置 Flash 所使用的 QSPI 总线的实际支持的数据线最大宽度，例如：
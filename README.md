# lgt-grbl-v1.1
基于grbl 1.1版本使用国产替代芯片LGT8F328P修改


注意：在开始之前，从ArduinoIDE中删除先前的Grbl库安装。否则，将有编译问题！在Linux上路径 /root/Arduino/libraries。

先需要下载LGT芯片的[**arduno支持包**]()(https://github.com/nulllaborg/arduino_nulllab)

 **1. 下载LGT-Grbl源代码**

git clone https://github.com/nulllaborg/lgt-grbl-v1.1

解压下载的文件，得到lgt-grbl的文件夹

 **2. 启动Arduino IDE**

**3. 将Grbl作为库加载到ArduinoIDE中**

单击 sketch下拉菜单，导航到**加载库**（Include Library）并选择**添加一个.ZIP库..**。

选择**lgt-grbl**文件夹中的名为**Grbl**文件夹

重要信息：该文件夹仅包含源文件和示例目录。如果您不小心选择.zip文件或错误的文件夹，您将需要导航到您的Arduino库，删除错误，然后重新执行步骤3

**4.打开Grbl Upload Arduino示例**

单击**文件**（File）下拉菜单，导航到示例（Examples）Examples->第三方库示例（下拉最下方）Grbl，然后选择GrblUpload

**5.烧录工具LGT-ISP工具**
工具->编程器 选择为Nulllab-CH5xxDuino as ISP
串口选择LGT-ISP的串口，不是arduino通讯用的端口

**6.将Grbl编译并上传到您的Arduino。**

 将您的LGT8F328P主控的连接到您的计算机

确保您的电路板已在Tool->开发板菜单中设置为DIY Board
CPU Frequency -> 16M
Clock Source  -> Exteral 16M
EEROM Size    -> 0K
Upload Speed  -> 115200
Variant       -> LQFP32
点击项目->使用编程器上传到你的LGT8F328P芯片中！（不要直接使用上传工具，必须要用编程器上传grbl）
高级用户：大多数用户使用Grbl的默认程序就足够了，但您可以通过编辑Arduino库（而不是下载）文件夹中的config.h文件来自定义Grbl。该文件启用或禁用所有Grbl的附加编译时选项。 文件中有他们功能描述。 编辑并保存后，只需按照上述步骤即可刷您的自定义Grbl程序！

 **7.验证**

 打开Arduino串口检测或其他GRBL控制软件，选择串口，选择波特率115200点击链接，打印出grbl版本号代表成功了

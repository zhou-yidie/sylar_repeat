# sylar-repeat
从零开始重写sylar C++高性能分布式服务器框架
模仿大佬重写sylar，原项目地址：https://github.com/zhongluqiang/sylar-from-scratch

## 模块概述

### 日志模块
支持流式日志风格写日志和格式化风格写日志，支持日志格式自定义，日志级别，多日志分离等等功能。
流式日志使用：
```cpp
SYLAR_LOG_INFO(g_logger) << "this is a log";
```
格式化日志使用：
```cpp
SYLAR_LOG_FMT_INFO(g_logger, "%s", "this is a log"); 
```
日志支持自由配置日期时间，累计运行毫秒数，线程id，线程名称，协程id，日志线别，日志名称，文件名，行号。
与日志模块相关的类：
`LogEvent`: 日志事件，用于保存日志现场，比如所属日志器的名称，时期时间，线程/协程id，文件名/行号等，以及日志消息内容。
`LogFormatter`: 日志格式器，构造时可指定一个格式模板字符串，根据该模板字符串定义的模板项对一个日志事件进行格式化，提供format方法对LogEvent对象进行格式化并返回对应的字符串或流。
`LogAppender`：日志输出器，用于将日志输出到不同的目的地，比如终端和文件等。LogAppender内部包含一个LogFormatter成员，提供log方法对LogEvent对象进行输出到不同的目的地。这是一个虚类，通过继承的方式可派生出不同的Appender，目前默认提供了StdoutAppender和FileAppender两个类，用于输出到终端和文件。
`Logger`: 日志器，用于写日志，包含名称，日志级别两个属性，以及数个LogAppender成员对象，一个日志事件经过判断高于日志器自身的日志级别时即会启动Appender进行输出。日志器默认不带Appender，需要用户进行手动添加。
`LoggerManager`：日志器管理类，单例模式，包含全部的日志器集合，并且提供工厂方法，用于创建或获取日志器。LoggerManager初始化时自带一个root日志器，这为日志模块提供一个初始时可用的日志器。
本项目的日志模块基于sylar的进行了简化，同时参考了log4cpp的一些设计，在保证功能可用的情况下，简化了几个类的设计，降低了耦合度。目前来看，在LogAppender上仍然需要丰富，以下几种类型的Appender在实际项目中都非常有必要实现：
1. Rolling File Appender，循环覆盖写文件
2. 内存Rolling Appender
3. 支持Log日志按大小分片
4. 支持网络日志服务器，比如syslog


### Util与Marco模块

工具接口与工具类，功能宏定义。包括获取时间，日期时间格式转换，栈回溯，文件系统操作接口，类型转换接口，以及SYLAR_ASSERT宏。详细接口参考util.h，macro.h。

### 环境变量模块

提供管理环境变量管理功能，包括系统环境变量，自定义环境变量，命令行参数，帮助信息，exe名称与程序路径相关的信息。环境变量全部以key-value的形式进行存储，key和value都是字符串格式。提供add/get/has/del接口用于操作自定义环境变量和命令行选项与参数，提供setEnv/getEnv用于操作系统环境变量，提供addHelp/removeHelp/printHelp用于操作帮忙信息，提供getExe/getCwd用于获取程序名称及程序路径，提供getAbsolutePath/getAbsoluteWorkPath/getConfigPath用于获取路径相关的信息。

待改进：
使用getopt系列接口解析命令行选项与参数，这样可以支持解析选项合并和长选项，比如像"ps -auf"和"ps --help"。

## 当前进度

| 日期       | 进度       |
| ---------- | ---------- |
| 2024.12.14 | 项目初始化 |
| 2024.12.18 | 实现日志模块 |
| 2024.12.19 | Util与Macro模块 |
| 2024.12.19 | 环境变量模块 |
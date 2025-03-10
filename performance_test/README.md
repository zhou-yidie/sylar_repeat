测试环境：2核2GB腾讯云服务器

测试工具：ApacheBench， Version 2.3 

测试说明：million -> 百万连接
，
libevent -> 测试libevent的百万连接

### 性能对比

![alt text](image.png)

详细分析

- 吞吐量与响应时间

libevent 在吞吐量和平均响应时间上更优，表明其事件驱动模型在高并发下处理效率更高。
sylar 的响应时间分布更稳定（标准差较小），适合对延迟敏感的场景。
- 连接管理

sylar 的连接时间更短（平均 2 ms vs. 38 ms），可能得益于更高效的连接池或 IO 调度策略。
libevent 的连接时间波动极大（标准差 90.9 ms），存在部分极端延迟（最长 1,095 ms），需关注连接复用或超时机制。
- 处理能力

sylar 的请求处理时间较长（平均 91 ms vs. 44 ms），但稳定性更强（标准差 6.8 ms vs. 8.1 ms），可能受业务逻辑复杂度影响。
- 数据传输

sylar 传输速率更高（2.6 MB/s vs. 1.0 MB/s），可能因返回数据量更大（136 字节 vs. 21 字节）或带宽利用率优化更好。

### sylar测试数据

ab -n 1000000 -c 1000 

http://localhost:8020/


This is ApacheBench, Version 2.3 <$Revision: 1903618 $>


Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/

Licensed to The Apache Software Foundation, http://www.apache.org/

Benchmarking localhost (be patient)
Completed 100000 requests，
Completed 200000 requests，
Completed 300000 requests，
Completed 400000 requests，
Completed 500000 requests，
Completed 600000 requests，
Completed 700000 requests，
Completed 800000 requests，
Completed 900000 requests，
Completed 1000000 requests，
Finished 1000000 requests;


Server Software:        sylar/1.0.0

Server Hostname:        localhost

Server Port:            8020


Document Path:          /

Document Length:        136 bytes

Concurrency Level:      1000

Time taken for tests:   92.155 seconds

Complete requests:      1000000

Failed requests:        0

Non-2xx responses:      1000000

Total transferred:      248000000 bytes

HTML transferred:       136000000 bytes

Requests per second:    **10851.24** [#/sec] (mean)

Time per request:       92.155 [ms] (mean)

Time per request:       0.092 [ms] (mean, across all concurrent requests)

Transfer rate:          2628.03 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0    2   2.7      1      56
Processing:    20   91   6.8     90     205
Waiting:        0   90   6.5     89     199
Total:         45   92   6.1     90     208

Percentage of the requests served within a certain time (ms)
  50%     90
  66%     91
  75%     91
  80%     92
  90%     97
  95%    102
  98%    112
  99%    116
 100%    208 (longest request)

 ### libevent测试数据

 This is ApacheBench, Version 2.3 <$Revision: 1903618 $>

Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/

Licensed to The Apache Software Foundation, http://www.apache.org/

Benchmarking localhost (be patient)
Completed 100000 requests，
Completed 200000 requests，
Completed 300000 requests，
Completed 400000 requests，
Completed 500000 requests，
Completed 600000 requests，
Completed 700000 requests，
Completed 800000 requests，
Completed 900000 requests，
Completed 1000000 requests，
Finished 1000000 requests；


Server Software:        

Server Hostname:        localhost

Server Port:            8080

Document Path:          /

Document Length:        21 bytes

Concurrency Level:      1000

Time taken for tests:   82.564 seconds

Complete requests:      1000000

Failed requests:        0

Total transferred:      85000000 bytes

HTML transferred:       21000000 bytes

Requests per second:    **12111.87** [#/sec] (mean)

Time per request:       82.564 [ms] (mean)

Time per request:       0.083 [ms] (mean, across all concurrent requests)

Transfer rate:          1005.38 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0   38  90.9     30    1095
Processing:    13   44   8.1     45     278
Waiting:        0   34   8.4     36     262
Total:         31   82  91.7     76    1364

Percentage of the requests served within a certain time (ms)
  50%     76
  66%     80
  75%     82
  80%     82
  90%     83
  95%     87
  98%     95
  99%    106
 100%   1364 (longest request)

 ### 性能对比
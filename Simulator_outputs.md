## Simulation outputs ##
The simulator produces two different output files. The first one is directly printed in **stdout** and records runtime events e.g. the termination of a content download process. Notice that in principle each event handled by the simulator (packet received, packet sent, packet lost, etc.) can be printed to **stdout**. However print operations significantly increment the simulation execution time and
the **stdout** statistics collected by the released version of the simulator are limited to events that represent the end of a content download process. The syntax of the **stdout** output file is represented in the following table:

|column|syntax|example|
|:-----|:-----|:------|
|1     |simulation time | 3169053.0000000000 |
|2,3   |event | closed socket |
|4     |node id | 2     |
|5     | content\_name+port\_id | /Orange/wgpfowikcv/port:0  |
|6     | delivery time `[`sec`]` | 1.023  |
|7     | file size `[`bits`]` | 80000  |
|8     | average RTT `[`sec`]` | 0.0010  |
|9     | average packaet delay (take into account retransmissions) `[`sec`]` | 0.0015  |
|10    | keyword | class  |
|11    | popularity class | 2     |
|12    | sockets still opened in this node | 10    |
|13    | avg receiver interest window size | 5.4   |
|14    | number of exploited routes | 5     |
|15    | average number of exploited routes | 2.4   |

The second output file is a log saved to the path/file name specified to the simulator through the **outputfile** input parameter. The statistics collected in this file are of three different types. The first one represents network performance as the average queue occupancy) and is preceded by the **QUEUE stats:** keyword. The following table present an example QUEUE statistics:

|column|syntax|example|
|:-----|:-----|:------|
|1     |keyword | QUEUE `[`FINAL`]`|
|2     |time (only if not final queue statistic)| 1.0   |
|3     |keyword | NODE  |
|4     | node id | 1     |
|5     | keyword | to    |
|6     | node id | 2     |
|7     | average queue size [packets](packets.md) | 10.43  |

The second set of statistics collected in the **outputfile** represents per popularity class cache performance (miss, hit, input/output request rate) and is preceded by the Stats: keyword. The following table present an example CACHE statistics:

|column|syntax|example|
|:-----|:-----|:------|
|1     |keyword | CACHE `[`FINAL`]` NODE |
|2     |time (only if not final queue statistic) `[`sec`]`| 2.0   |
|3     |node id | 1     |
|4     |request miss rate | 2     |
|5     |request hit rate | 0.988 |
|6     |request rate| 2.988  |
|7     | miss probability (with filtered requests as miss)| 0.6711  |
|8     | hit probability | 0.3289  |
|9     | request filter probability | 0     |
|10    | miss probability (without filtered requests)| 0.6711 |
|11    | keyword | class |
|12    | calass\_id (popularity class in our settings)| 1     |

The third and last set of statistic represents per node and per forwarding prefix split ratio/rate in case of multipath scenario. An example of the split statistic is represented in the following table:

|column|syntax|example|
|:-----|:-----|:------|
|1     |keyword | SPLIT `[`FINAL`]` NODE|
|2     |time (only if not final queue statistic) `[`sec`]`| 2.0   |
|3     |keyword | NODE  |
|4     | node id | 1     |
|5     | keyword | to    |
|6     | node id | 2     |
|7     | forwarded requests `[`\%`]` | 23.3  |
|8     | forwarded requests `[`rate`]` | 1.4   |

Notice that the three groups of statistics are printed out each t<sub>i</sub> seconds as specified by the parameter **dci** (data collection interval) and at the end of the simulation (if t<sub>i</sub> = 0 only final statistics are collected).
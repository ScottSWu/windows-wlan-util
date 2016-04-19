# windows-wlan-util
Windows CLI for WLAN tasks

Open up the solution and build in whatever configuration you want.

### Examples

```
> windows-wlan-util.exe if
 0  Intel(R) Dual Band Wireless
```
Lists available wireless interfaces (0-indexed).

```
> windows-wlan-util.exe scan [interface]
```
Initiate a scan for wireless networks (usually takes around 3 seconds to fully populate the list). An interface can be specified by number (as returned by the `if` subcommand), otherwise it will default to interface 0. If no wireless interfaces are available, then it will print an error.

```
> windows-wlan-util.exe list [interface]
 -73    45  xx:xx:xx:xx:72:b1  ssid1
 -73    45  xx:xx:xx:xx:72:b2  ssid2
 -84    26  xx:xx:xx:xx:6f:52  ssid1
 -53    78  xx:xx:xx:xx:72:c2  ssid3
```
Queries the list of wireless networks found by the last scan. The first column is the RSSI in dbm. The second column is the "signal quality". The third column is the MAC address. The fourth column is the SSID.

```
> windows-wlan-util.exe query [interface]
```
Does both a scan and a query (does not wait for the scan, but will update it for the next listing).

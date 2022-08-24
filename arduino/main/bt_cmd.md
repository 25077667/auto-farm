# Bluetooth command manual


* Basic information
1. Bluetooth baud rate: 9600 
2. Bluetooth SSID: scc-farm

* Supported commands(from client)
1. set_time
2. active_relay

Command format:
```
<cmd> = (<key>:<value>' ')*\n
<key>,<value> are non-whitespace strings, both of them could not have ':'
char each <key>,<value> pair has a whitespace at the end
```

## Supported command manual
It is **undefined** behavior if the operands' (or context) are not defined here.

### set_time
**Prototype**: void detail::set_time(const string_view &s);

@s is "hh.mm.index".

*index* holds one byte of a literial digit.

*index* means the index of the irrigation activation table, which is [0, 2)

**Description**
It will set the time *hh*:*mm* at the *index* meaning that irrigation should be actively performed at that time.

### active_relay
**Prototype**: void detail::active_relay(const string_view &s);

@s us "1" or "0".

"1" means enable, "0" otherwise.

**Description**
Do irrigation, which is on the relay.


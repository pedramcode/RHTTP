# RHTTP

>Redis HTTP server for hybrid back-end services

This server can provide a microservice backend in the shortest time and with the highest efficiency.

**RHTTP** (Redis HTTP) Takes HTTP requests and publishes them into Redis. Then other servers can receive requests and make corresponding response for them, Then publishes response into Redis again and RHTTP server sends response to user.

<div style="display: flex;justify-content: center">
<img alt="RHTTP General Diagram" src="docs/images/rhttp_diagram.png" title="General RHTTP diagram" width="80%"/>
</div>

Services can seamlessly join/leave to RHTTP network without any interruption. RHTTP uses some mechanism I named it "**Bat Vision**" to detect new services or detect some service that left the network. RHTTP can send request to new services immediately after joining them and receive repsonses from them without any overhead.

<div style="display: flex;justify-content: center">
<img alt="RHTTP Bat vision diagram" src="docs/images/bat_vision.png" title="RHTTP Bat vision" width="80%"/>
</div>

Actually in a constant time interval, RHTTP publishes a signal called "**Heartbeat**" into redis, Any service in scope can receive this signal and publishes service info into "**Acknowledge pipe**". Then RHTTP stores service info into database. If a defined service does not provide acknowledge for a couple of heartbeats, RHTTP will remove service from database. 

## Requirements
* [CArgs](https://github.com/pedramcode/cargs)
* [HIRedis](https://github.com/redis/hiredis)
* [Libevent](https://libevent.org/)
* LibSSL
* SQLite3

## License
This project is licensed under the terms of the MIT License. See the [LICENSE](LICENSE.txt) file for details.
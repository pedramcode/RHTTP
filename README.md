# RHTTP

>Redis HTTP server for hybrid back-end services

This server can provide a microservice backend in the shortest time and with the highest efficiency.

**RHTTP** (Redis HTTP) Takes HTTP requests and publishes them into Redis. Then other servers can receive requests and make corresponding response for them, Then publishes response into Redis again and RHTTP server sends response to user.

<img alt="RHTTP General Diagram" src="docs/images/rhttp_diagram.png" title="General RHTTP diagram" width="100%"/>


Services can seamlessly join/leave to RHTTP network without any interruption. RHTTP uses some mechanism I named it "**Bat Vision**" to detect new services or detect some service that left the network. RHTTP can send request to new services immediately after joining them and receive repsonses from them without any overhead.


<img alt="RHTTP Bat vision diagram" src="docs/images/bat_vision.png" title="RHTTP Bat vision" width="100%"/>


Actually in a constant time interval, RHTTP publishes a signal called "**Heartbeat**" into redis, Any service in scope can receive this signal and publishes service info into "**Acknowledge pipe**". Then RHTTP stores service info into database. If a defined service does not provide acknowledge for a couple of heartbeats, RHTTP will remove service from database. 


## Interfaces

Here is a list of available and trusted interfaces for services to communicate with RHTTP server:
* [NodeJS interface (npm)](https://www.npmjs.com/package/rhttp_node)
* [Python interface (pip)](https://pypi.org/project/rhttp-python)

## Requirements
* [CArgs](https://github.com/pedramcode/cargs)
* [HIRedis](https://github.com/redis/hiredis)
* [Libevent](https://libevent.org/)
* LibSSL
* SQLite3
* [libconfig](https://github.com/hyperrealm/libconfig)

## Installation
1. Clone repository `git clone git@github.com:pedramcode/RHTTP.git`
2. Change directory to project root `cd RHTTP` 
3. Create "build" folder: `mkdir build`
4. Change directory to build: `cd build`
5. Run cmake `cmake ..`
6. Make project using make command `make`
7. Install project `sudo make install`

## Usage
After installation, you need to create a config file to pass it to rhttp executable, Here is an example of config file:

```text
version: "1.0";

server:
{
	name: "Development RHTTP server";
	debug: true;

	heartbeat: {
        interval: 1;
        dead_server_beats: 3;
    };

	http:
	{
		host: "127.0.0.1";
		port: 8080;
		timeout: 120;
	};
	redis:
	{
		host: "127.0.0.1";
		port: 6379;
	};
};
```

It's better save the file with "*.cfg" extension. For example `/etc/rhttp/rhttp.cfg`.

Then run RHTTP server with this command:
```shell
rhttp --config /etc/rhttp/rhttp.cfg
```

or 

```shell
rhttp -c /etc/rhttp/rhttp.cfg
```

## License
This project is licensed under the terms of the MIT License. See the [LICENSE](LICENSE.txt) file for details.
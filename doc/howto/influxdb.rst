==========================
Running influxdb in docker
==========================

Installing required packages
----------------------------

In order to install `docker` and the `influxdb` CLI run the following command:

::
  
  sudo apt -y install docker influxdb-client


Pulling the `influxdb` container
--------------------------------

The latest version of `influxdb` can be pulled with:

::

  docker pull influxdb


Initial setup of `influxdb` instance
------------------------------------

Run the `influxdb` image in one terminal:

::

  docker run --name influxdb -p 8086:8086 influxdb

While the container is up do the following to do initial setup of the database
and fill in the required information:

::

  docker exec -it influxdb influx setup 
  docker exec -it influxdb influx config create \
    -n eagine \
    -u http://localhost:8086 \
    -p 'eagine:<password>' \
    -o OGLplus \
    --active

The basic connectivity can be tested with:

::

  docker exec -it influxdb influx ping 

Ensuring the client and server major versions match
---------------------------------------------------

Check the client version:

::

  influx version

Check the server version:

::

  docker exec -it influxdb influx version
  

Examples
--------

Define a helper function that prints the CPU load:

::

  function cpuLoad { awk -v oldidle=$(awk '/cpu / {print $5}' /proc/stat; sleep 1) '/cpu / {perc=100-($5-oldidle)/100 ; printf "%0.2f", perc}' /proc/stat; }


Write CPU load every second (using the CLI):

::

  while echo $(date --iso-8601=seconds),$(cpuLoad),cpuLoad,longTerm
  do sleep 1
  done | influx write \
    --bucket EAGine \
    --format csv \
    --header "#constant measurement,sensors" \
    --header "#datatype dateTime:RFC3339,double,tag,tag" \
    --header "#group false,false,true,true" \
    --header "time,load,cpuLoad,period"

Alternatively the data can be formatted using the influx default line protocol:

::

  while echo sensors,source=cpu,kind=load cpuLoad=$(cpuLoad) $(date +%s%N)
  do sleep 1
  done | influx write --bucket EAGine

Query the `cpuLoad` values in the last 12 hours:

::

  influx query '
    from(bucket: "EAGine")
      |> range(start: -12h, stop: now())
      |> filter(fn: (r) => r._measurement == "sensors")
      |> filter(fn: (r) => r._field == "load")
      |> filter(fn: (r) => r.sensor == "cpuLoad")'

Delete everything from the specified bucket:

::

  influx delete \
    --bucket EAGine \
    --start $(date --iso-8601=seconds --date=@0) \
    --stop $(date --iso-8601=seconds)

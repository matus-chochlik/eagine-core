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

  influx --version

Check the server version:

::

  docker exec -it influxdb influx version
  

Examples
--------

Writing CPU load every second (using the CLI):

::

  while echo cpuLoad,$(date --iso-8601=seconds),$(awk -v oldidle=$(awk '/cpu / {print $5}' /proc/stat; sleep 1) '/cpu / {perc=100-($5-oldidle)/100 ; printf "%s", perc}' /proc/stat)
  do sleep 1
  done | influx write \
    --bucket OGLplus \
    --format csv \
    --header "#constant tag,measurement,sensors" \
    --header "#datatype tag,dateTime:RFC3339,double" \
    --header "#group true,false,false" \
    --header "sensor,time,load"

Querying the `cpuLoad` values in the last 12 hours:

::

  influx query '
    from(bucket: "EAGine")
      |> range(start: -12h, stop: now())
      |> filter(fn: (r) => r._measurement == "sensors")
      |> filter(fn: (r) => r._field == "load")
      |> filter(fn: (r) => r.sensor == "cpuLoad")'

Deleting everything from the specified bucket:

::

  influx delete \
    --bucket EAGine \
    --start $(date --iso-8601=seconds --date=@0) \
    --stop $(date --iso-8601=seconds)

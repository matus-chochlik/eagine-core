from(bucket: "eagilog")
  |> range(start: v.timeRangeStart, stop: v.timeRangeStop)
  |> filter(fn: (r) => 
       r._measurement == "interval" and
       r.app == "RouterExe" and
       r.tag == "parentUpdt" and
       (r._field == "avg" or r._field == "max"))
  |> map(fn: (r) => ({ r with _value: r._value }))
  |> aggregateWindow(every: v.windowPeriod, fn: median, createEmpty: false)
  |> yield(name: "median")

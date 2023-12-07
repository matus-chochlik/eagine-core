from(bucket: "eagilog")
  |> range(start: v.timeRangeStart, stop: v.timeRangeStop)
  |> filter(fn: (r) => 
       r._measurement == "arg" and
       r.source == "RouterExe" and
       r.name == "workRate" and
       r._field == "value")
  |> map(fn: (r) => ({ r with _value: r._value * 100.0 }))
  |> aggregateWindow(every: v.windowPeriod, fn: median, createEmpty: false)
  |> yield(name: "median")

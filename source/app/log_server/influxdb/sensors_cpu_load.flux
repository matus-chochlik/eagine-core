from(bucket: "eagilog")
  |> range(start: v.timeRangeStart, stop: v.timeRangeStop)
  |> filter(fn: (r) => 
      r._measurement == "arg" and
      r.source == "SensorsLog" and
      r.msg_tag == "cpuLoad" and
      (r.name == "short" or r.name == "long"))
  |> map(fn: (r) => ({ r with _value: r._value * 100.0 }))
  |> timedMovingAverage(every: 2m, period: 6m)
  |> aggregateWindow(every: v.windowPeriod, fn: mean, createEmpty: false)
  |> yield(name: "mean")

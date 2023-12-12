from(bucket: "eagilog")
  |> range(start: v.timeRangeStart, stop: v.timeRangeStop)
  |> filter(fn: (r) => 
      r._measurement == "arg" and
      r.source == "SensorsLog" and
      r.msg_tag == "ramUsage")
  |> pivot(rowKey: ["_time"], columnKey: ["name"], valueColumn: "_value")
  |> map(fn: (r) => ({ r with _value: (1.0 - r.free / r.total) * 100.0 }))
  |> timedMovingAverage(every: 3m, period: 6m)
  |> aggregateWindow(every: v.windowPeriod, fn: mean, createEmpty: false)
  |> yield(name: "mean")

from(bucket: "eagilog")
  |> range(start: v.timeRangeStart, stop: v.timeRangeStop)
  |> filter(fn: (r) => 
       r._measurement == "arg" and
       r.source == "TilingNode" and
       r.msg_tag == "solvdBoard" and
       r.name == "time")
  |> aggregateWindow(every: v.windowPeriod, fn: mean, createEmpty: false)
  |> yield(name: "mean")

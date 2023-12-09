from(bucket: "eagilog")
  |> range(start: v.timeRangeStart, stop: v.timeRangeStop)
  |> filter(fn: (r) => 
      r._measurement == "arg" and
      r.app == "RouterExe" and
      r.msg_tag == "msgStats" and
      r.name == "msgsPerSec")
  |> aggregateWindow(every: v.windowPeriod, fn: mean, createEmpty: false)
  |> yield(name: "mean")

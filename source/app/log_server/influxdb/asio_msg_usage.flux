from(bucket: "eagilog")
  |> range(start: v.timeRangeStart, stop: v.timeRangeStop)
  |> filter(fn: (r) => 
       r._measurement == "arg" and
       r.source == "AsioConnSt" and
       r.msg_tag == "msgSlack" and
       r.name == "slack" and
       r._field == "value")
  |> map(fn: (r) => ({ r with _value: (1.0 - r._value) * 100.0 }))
  |> aggregateWindow(every: v.windowPeriod, fn: median, createEmpty: false)
  |> yield(name: "median")

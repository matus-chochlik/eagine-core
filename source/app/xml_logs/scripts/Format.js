.pragma library

function durationStr(seconds) {
	if(seconds) {
		if(seconds < 5.0) {
			if(seconds < 0.005) {
				return "%1 μs".arg(Math.trunc(seconds*1000000))
			} else {
				return "%1 ms".arg(Math.trunc(seconds*1000))
			}
		} else {
			seconds = Math.trunc(seconds)
			var days = Math.floor(seconds / 86400)
			seconds = seconds - days * 86400
			var hours = Math.floor(seconds / 3600)
			seconds = seconds - hours * 3600
			var minutes = Math.floor(seconds / 60)
			seconds = seconds - minutes * 60

			if(days > 0) {
				if(hours == 0) {
					return "%1 d".arg(days)
				}
				return "%1 d %2 hr".arg(days).arg(hours)
			}
			if(hours > 0) {
				if(minutes == 0) {
					return "%1 hr".arg(hours)
				}
				return "%1 hr %2 min".arg(hours).arg(minutes)
			}
			if(minutes > 0) {
				if(seconds == 0) {
					return "%1 min".arg(minutes)
				}
				return "%1 min %2 sec".arg(minutes).arg(seconds)
			}
			return "%1 sec".arg(seconds)
		}
	}
	return "-"
}

function remainingTimeStr(seconds) {
	if(seconds) {
		if(seconds < 5.0) {
			return "less than 5s"
		} else {
			return durationStr(seconds);
		}
	}
	return "estimating..."
}


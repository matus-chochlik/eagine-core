.pragma library

var lc = Qt.locale("C")

function progressStr(min, value, max) {
	var progress = null
	if((min != null) && (value != null) && (max != null) && (max - min > 0)) {
		progress = (value - min) / (max - min);
	}
	return progress
		? "%1 %".arg(Number(progress * 100.0).toLocaleString(lc, "f", 1))
		: "-"
}

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
			return durationStr(seconds)
		}
	}
	return "estimating..."
}

function version(major, minor, patch) {
	if(major != null) {
		if(minor != null) {
			if(patch != null) {
				return "%1.%2.%3".arg(major).arg(minor).arg(patch)
			}
			return "%1.%2".arg(major).arg(minor)
		}
		return "%1".arg(major)
	}
	return "-"
}

formatDuration = function(sec) {
    if (sec > 604800) {
        return (sec / 604800).toFixed(0) + ' wk';
    }
    if (sec > 86400) {
        return (sec / 86400).toFixed(0) + ' dy';
    }
    if (sec > 3600) {
        return (sec / 3600).toFixed(0) + ' hr';
    }
    if (sec > 60) {
        return (sec / 60).toFixed(0) + ' min';
    }
    return sec.toFixed(0) + ' sec';
};


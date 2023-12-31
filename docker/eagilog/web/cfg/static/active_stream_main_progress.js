var isActive = false;
var chartReloadCount = 0;

doReload = function() {
    chartReloadCount += 1;
    if (isActive || chartReloadCount >= 10) {
        $('#active_stream_main_progress')
            .attr('src', '/image/active_stream_main_progress.svg?t=' + new Date().getTime())
        chartReloadCount = 0;
    }
};

$(window).focus(function() {
    isActive = true;
    doReload();
});

$(window).blur(function() {
    isActive = false;
    chartReloadCount = 0;
});

setInterval(doReload, 15000);

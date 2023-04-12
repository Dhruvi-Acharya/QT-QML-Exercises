var selectStatus = {
    ready: false,
    start_x: 0, start_y: 0,
    end_x: 0, end_y: 0,

    start: function(x, y) {
        this.ready = true;
        this.start_x = x;
        this.start_y = y;
        this.end_x = x;
        this.end_y = y;
    },
    update: function(x, y) {
        if (this.ready) {
            //console.log("selectStatus update()");
            zoomTargetRect.visible = true;
            zoomTargetRect.width = (Math.abs (x - this.start_x));
            zoomTargetRect.height =(Math.abs (y - this.start_y));
            zoomTargetRect.x = (x < this.start_x) ? x : this.start_x;
            zoomTargetRect.y = (y < this.start_y) ? y : this.start_y;
        }
    },
    stop: function(contentRect, orientation) {
        var success = false;
        if (this.ready) {
            // normalize된 rect 좌표 계산 (TODO)
            var cRect = videoOutput.contentRect;
            if (orientation === 0) {
                var nx = zoomTargetRect.x/cRect.width;
                var ny = zoomTargetRect.y/cRect.height;
                var nw = zoomTargetRect.width/cRect.width;
                var nh = zoomTargetRect.height/cRect.height;
            }
            else if (orientation === 90) {
                nw = zoomTargetRect.height/cRect.height;
                nh = zoomTargetRect.width/cRect.width;
                nx = 1 - nw - (zoomTargetRect.y/cRect.height)
                ny = zoomTargetRect.x/cRect.width
            }
            else if (orientation === 180) {
                nw = zoomTargetRect.width/cRect.width;
                nh = zoomTargetRect.height/cRect.height;
                nx = 1 - nw - (zoomTargetRect.x/cRect.width)
                ny = 1 - nh - (zoomTargetRect.y/cRect.height)

            }
            else if (orientation === 270) {
                nw = zoomTargetRect.height/cRect.height;
                nh = zoomTargetRect.width/cRect.width;
                nx = zoomTargetRect.y/cRect.height
                ny = 1 - nh - (zoomTargetRect.x/cRect.width)
            }

            zoomTargetRect.nomRect = Qt.rect(nx,ny,nw,nh);
            success = true;
            //console.log('targetRect=', zoomTargetRect.nomRect, cRect);
        }
        this.reset();
        return success;

    },
    reset: function() {
        this.ready = false;
        zoomTargetRect.visible = false;
        this.start_x = 0;
        this.start_y = 0;
        this.end_x = 0;
        this.end_y = 0;
    }
}


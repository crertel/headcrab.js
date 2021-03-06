alert_status = function(str) {
    $("#status").text(str);
};

var boidDemo = (function () {

    var canvas;
    var rt;
    var boidlist = [];
    var waitingOnUpdate = false;
    var requestInterval;

    var init = function () {

        var requestAnimationFrame = window.requestAnimationFrame || window.mozRequestAnimationFrame ||
                              window.webkitRequestAnimationFrame || window.msRequestAnimationFrame;
        window.requestAnimationFrame = requestAnimationFrame;

        // setup canvas 
         canvas = document.getElementById("boidcanvas");
        rt = canvas.getContext('2d');
        HC.connect(8080);

        // hookup start button
        $("#startbutton").on("click", function (){
            drawBoids();
            HC.issue("flock", "getall", {}, function() {
                requestInterval = setInterval(function() {
                    fetchBoids();
                }, 1000);
            });
            //$(this).hide();
        });
        // Connect stop button
        $("#stopbutton").on("click", function (){
            drawBoids();
            HC.issue("flock", "stop", {}, clearInterval(requestInterval) );
            //$(this).hide();
        });
        // hookup step button
        $("#stepbutton").on("click", function (){
            drawBoids();
            HC.issue("flock", "getall", {}, fetchBoids);
            //$(this).hide();
        });
    };

    var fetchBoids = function () {
        // schedule ourselves again
        if (waitingOnUpdate == false) {
            waitingOnUpdate = true;
            HC.issue("flock", "getall", {}, function (d) { waitingOnUpdate = false; updateBoids(d); } );
        }
    }

    var updateBoids = function (data) {
        boidlist = data.boids;
    }

    var drawBoids = function () {
        // clear canvas
        rt.save();
        rt.setTransform(1, 0, 0, 1, 0, 0);
        rt.clearRect(0, 0, canvas.width, canvas.height);
        rt.restore();

        // draw boids
        _.each(boidlist, function(boid) {
            rt.fillRect(boid.x-5, boid.y-5,10,10);
        });

        window.requestAnimationFrame(drawBoids);
    };

    return { init: init };
})();
var boidDemo = (function () {

    var canvas;
    var rt;
    var boidlist = [];

    var init = function () {

        var requestAnimationFrame = window.requestAnimationFrame || window.mozRequestAnimationFrame ||
                              window.webkitRequestAnimationFrame || window.msRequestAnimationFrame;
        window.requestAnimationFrame = requestAnimationFrame;

        // setup canvas 
        canvas = document.getElementById("boidcanvas");
        rt = canvas.getContext('2d');
        //HC.connect(666);

        // hookup start button
        $("#startbutton").on("click", function (){
            drawBoids();
            //HC.issue("flock", "getall", {}, updateBoidsRunner );
            $(this).hide();
        });
    };

    var updateBoids = function (data) {
        // get the boid info
        boidlist = [ {x:30, y:30, theta:Math.PI/4} ];
        //boidlist = data.boids;

        // schedule ourselves again
        //HC.issue("flock", "getall", {}, updateBoids );
        requestAnimationFrame(updateBoids);
    }

    var drawBoids = function () {

        // clear canvas
        rt.save();
        rt.setTransform(1, 0, 0, 1, 0, 0);
        rt.clearRect(0, 0, canvas.width, canvas.height);
        rt.restore();

        // draw boids
        _.each(boidlist, function(boid) {
            rt.moveto( boid.x, boid.y);
            rt.save();
            rt.rotate(boid.theta);
            rt.beginPath();
            rt.arc(0,0,5,0,Math.PI,false);
            rt.fill();
            rt.beginPath();
            rt.lineto(0,10);
            rt.stroke();
            rt.restore();
        });

        window.requestAnimationFrame(drawBoids);
    };

    return {    init: init,
            };
})();
// notbookies 8/5/17
// Contains HTML/CSS/JavaScript for door project website

const char SITE_index[] PROGMEM = R"=====(

    <!DOCTYPE html>
    <html>
        <head>
            <meta charset="utf-8"/>
            <meta name="viewport" content="width=device-width"/>
            <link rel="shortcut icon" type="image/png" href="https://image.flaticon.com/icons/png/512/194/194432.png"/>
            <title>Door Sensor</title>
            <style>
                /* local styles */
                .opened     {color: green;}
                .closed     {color: red;}
                #door-state {font-size: 400%;}
    
                /* global styles */
                body {
                    font-family: Arial, Helvetica, sans-serif;
                    font-size: 100%;
                    background-color: #f4f4f4;
                }
                .container {
                    width: 80%;
                    margin: auto;
                    overflow: hidden;
                }
                .column1 {float: left;}
                .column2 {float: right;}
                p{
                    font-size: 200%;
                    color: #696969;
                }
            </style>
        </head>
    
        <body>
            <div class="container">
                <div align="center">
                    <h1>The door is:</h1>
                    <h2 id="door-state"></h2>
                    <h1>And has been for:</h1>
                    <p id="current-timer"></p>
                    <h1>Number of visitors:</h1>
                    <p id="visitors"></p>
                </div>
                <div align="center" class="column1">
                    <h1>Occupancy record:</h1>
                    <p id="closed-record"></p>
                    <h1>Vacancy record:</h1>
                    <p id="open-record"></p>
                </div>
                <div align="center" class="column2">
                    <h1>Occupancy average:</h1>
                    <p id="closed-avg"></p>
                    <h1>Vacancy average:</h1>
                    <p id="open-avg"></p>
                </div>
            </div>
    
        <script>
            /* declaring variable to make logic below cleaner */
            var doorState       = document.getElementById('door-state')
              , currentTimer    = document.getElementById('current-timer')
              , closedRecord    = document.getElementById('closed-record')
              , openRecord      = document.getElementById('open-record')
              , thumbs          = document.getElementById('thumbs')
              , visitors        = document.getElementById('visitors')
              , closedAvg       = document.getElementById('closed-avg')
              , openAvg         = document.getElementById('open-avg');
    
            /* converts ms to hrs/min/sec */
            function msFormat(duration) {
                var seconds = parseInt( (duration / 1000)         % 60 )
                , minutes   = parseInt( (duration / (1000*60))    % 60 )
                , hours     = parseInt( (duration / (1000*60*60)) % 24 );
    
                /* multiple returns to remove hr/min if =0 */
                if (hours != 0){
                    return hours + " hr " + minutes + " min " + seconds + " sec";
                } else if (minutes != 0){
                    return minutes + " min " + seconds + " sec";
                } else if (seconds != 0){
                    return seconds + " sec";
                } else {
                    return 0 + " sec";
                }
            }
            /* main 'loop' */
            function updateData() {
                console.log("update ran");
                var xhttp = new XMLHttpRequest();
                    xhttp.onreadystatechange = function() {
                    /* readyState 4: request finished and response is ready */
                    /* status 200: "OK" */
                    if (this.readyState == 4 && this.status == 200) {
                        /* JSON.parse takes JSON string and constructs object(s) */
                        var data = JSON.parse(this.responseText);
                        console.log(data);
                        
                        currentTimer.innerHTML  = msFormat(data.timer);
                        visitors.innerHTML      = data.visitors;
                        openRecord.innerHTML    = msFormat(data.longestOpen);
                        closedRecord.innerHTML  = msFormat(data.longestClosed);
                        openAvg.innerHTML       = msFormat(data.avgOpen);
                        closedAvg.innerHTML     = msFormat(data.avgClosed);
    
                        /* change door message text and color */
                        if (data.doorMsg == false) {
                            doorState.innerHTML = "OPEN";
                            doorState.className = "opened";
                        }
                        if (data.doorMsg == true){
                            doorState.innerHTML = "CLOSED";
                            doorState.className = "closed";
                        }
                    }
                };
                /* GET request at "IP_address/data" (JSON string) */
                xhttp.open("GET", "/data", true);
                xhttp.send();
            }
            /* run update function once every 250ms */
            updateData();
            window.setInterval(updateData, 250);
        </script>
    
        </body>
    </html>

)=====";


const Chart = require('chart.js')

class Graph extends Chart {

    constructor(canvas, type, units, title) {
        let config = {
            type: 'line',
            data: {
                datasets: [{
                    borderColor: '#ffb300',
                    backgroundColor: 'rgba(255, 179, 0, 0.25)',
                    steppedLine: type == 'digital',
                    fill: type == 'digital',
                    pointRadius :  0,
                    data: [],
                }]
            },
            options: {
                title: {
                    text: title,
                    display: title != undefined
                },
                legend: {
                    display: false
                },
                scales: {
                    xAxes: [{
                        type: 'time',
                        time: {
                            // round: 'day'
                            tooltipFormat: 'll HH:mm'
                        },
                        scaleLabel: {
                            display: false,
                            labelString: 'Temps'
                        }
                    }],
                    yAxes: [{
                        scaleLabel: {
                            display: true,
                            labelString: units
                        }
                    }]
                },
            }
        }

        if (type == 'digital')
        {
            config.data.yLabels = ['On','Off'];
            config.options.scales.yAxes[0].type = 'category';
        }

        super(canvas, config);
    }


    addPoint (timeStamp, value)
    {
        if(value === true)
            value = 'On';
        else if(value === false)
            value = 'Off';

        this.config.data.datasets[0].data.push({
            x: timeStamp,
            y: value
        })
    }
}

module.exports = Graph;
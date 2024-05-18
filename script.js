
let app = document.getElementById("app");
app.width = 800;
app.height = 600;
let ctx = app.getContext("2d");
let w = null;

let A = 0.0;
let B = 0.0;
let C = 0.0;

let dt = 0;
let mode = 0;
let dimension= 3;

let toRotate= 0;

console.log(dt);

let speed = 0.001;

let spinning = false;

let loc = 0;

const MAX_DIMENSION = 10;
const MAX_CUBES = (MAX_DIMENSION * MAX_DIMENSION * MAX_DIMENSION - (MAX_DIMENSION-2) * (MAX_DIMENSION-2) * (MAX_DIMENSION-2));

startDemo();
let intervalID;

function spin(instance, dt) {
    intervalID = setInterval(() => {
        dt += 1; // Increment dt by 1 every second
        A += speed;
        B += speed;
        C += speed;
        // console.log(dt);
        render(instance, dt, A, B, C, dimension, mode, loc);
    }, 25); // Interval set to 1000ms (1 second)
}
async function startDemo() {
    const response = await fetch('abcd.wasm');
    const bytes = await response.arrayBuffer();
    const {instance} = await WebAssembly.instantiate(bytes);
    // instance.exports.resetFaces(1);
    render(instance, dt, A, B, C, dimension, mode, loc);



    document.addEventListener('keydown', (event) => {
        console.log("pA");
        if (event.key === 'w') {
            A += 0.1;
            dt++;
        } else if (event.key === 's') {
            A -= 0.1;
        } else if (event.key === 'd') {
            B += 0.1;
        } else if (event.key === 'a') {
            B -= 0.1;
        } else if (event.key === 'j') {
            C += 0.1;
        } else if (event.key === 'k') {
            C -= 0.1;
        } else if (event.key === 'p') {
            if(!spinning) {
                spinning = true;
                spin(instance, dt);
            }
            else {
                spinning = false;
                clearInterval(intervalID);
            }
        }
        else if (event.key === 'i') {
            speed *= 2;
        }
        else if (event.key === 'o') {
            speed /= 2;
        }

        else if (event.key === ',') {
            if(loc < dimension-1) loc += 1;
        }
        else if (event.key === '.') {
            if(loc > 0) loc -= 1;
        }
        else if (event.key === 'm') {
            mode = (mode + 1) % 3;
        }
        else if (event.key === 'r') {
            console.log("rotate");
            console.log(dt);

            instance.exports.rotate_mode_length_i(1);
            toRotate = 1;
        }
        render(instance, dt, A, B, C, dimension, mode, loc, toRotate);
        toRotate = 0;
    });
}

function render(instance, dt, A, B, C, dimension, mode, loc, toRotate) {
    const pixels = instance.exports.render(dt, A, B, C, dimension, mode, loc, toRotate);
    const buffer = instance.exports.memory.buffer;
    const imageData = new ImageData(new Uint8ClampedArray(buffer, pixels, app.width * app.height * 4), app.width);
    ctx.putImageData(imageData, 0, 0);
}


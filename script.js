
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
let direction = 0;

let magnitude = 0.5

let angleJump = 20;
let turnSpeed = 10;

let speed = 0.001;

let spinning = false;

let isRotating = false;

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
        C += speed/4;
        render(instance, dt, A, B, C, dimension, mode, loc);
    }, 25); // Interval set to 1000ms (1 second)
}
async function startDemo() {
    const response = await fetch('abcd.wasm');
    const bytes = await response.arrayBuffer();
    const {instance} = await WebAssembly.instantiate(bytes);
    // instance.exports.resetFaces(1);
    render(instance, dt, A, B, C, dimension, mode, loc);



    document.addEventListener('keydown', async (event) => {

        // if(isRotating) {
        //     return;
        // }

        if (event.key === 'w') {
            A += magnitude;
            dt++;
        } else if (event.key === 's') {
            A -= magnitude;
        } else if (event.key === 'd') {
            B += magnitude;
        } else if (event.key === 'a') {
            B -= magnitude;
        } else if (event.key === 'j') {
            C += magnitude;
        } else if (event.key === 'k') {
            C -= magnitude;
        } else if (event.key === 'p') {
            if (!spinning) {
                spinning = true;
                spin(instance, dt);
            } else {
                spinning = false;
                clearInterval(intervalID);
            }
        } else if (event.key === 'i') {
            speed *= 2;
        } else if (event.key === 'o') {
            speed /= 2;
        } else if (event.key === ',') {
            if (loc < dimension - 1) loc += 1;
        } else if (event.key === '.') {
            if (loc > 0) loc -= 1;
        } else if (event.key === 'm') {
            mode = (mode + 1) % 3;
        } else if (event.key === 'r') {
            if(dimension === 1) {
                return;
            }
            isRotating = true;
            for (let i = 1; i < 90; i += angleJump){
                render(instance, dt, A, B, C, dimension, mode, loc, toRotate, direction, i);
                await new Promise(resolve => setTimeout(resolve, turnSpeed));
            }
            toRotate = 1;
            direction = 0;
            isRotating = false;
        } else if (event.key === 'R') {
            if(dimension === 1) {
                return;
            }
            isRotating = true;
            for (let i = -1; i > -90; i -= angleJump){
                render(instance, dt, A, B, C, dimension, mode, loc, toRotate, direction, i);
                await new Promise(resolve => setTimeout(resolve, turnSpeed));
            }
            toRotate = 1;
            direction = 1;
            isRotating = false;
        } else if (event.key === '-') {
            loc = 0;
            dimension -= 1;
            if(dimension < 1) {
                dimension = 1;
            }
        } else if (event.key === '=') {
            loc = 0;
            dimension += 1;
        } else if (event.key === 'S') {
            await scramble(instance);
        } else if (event.key === 'c') {
            A = 0;
            B = 0;
            C = 0;
        }
        render(instance, dt, A, B, C, dimension, mode, loc, toRotate, direction, 0);
        toRotate = 0;
    });
}

function getRandomInt(max) {
    return Math.floor(Math.random() * max);
}
async function scramble(instance) {
    for(let i=0; i<100; i++) {
        let rand_mode = getRandomInt(3);
        let rand_loc = getRandomInt(dimension);
        render(instance, dt, A, B, C, dimension, rand_mode, rand_loc, 1, direction, 0);
        await new Promise(resolve => setTimeout(resolve, turnSpeed));
    }
}

function render(instance, dt, A, B, C, dimension, mode, loc, toRotate, direction, angle) {
    const pixels = instance.exports.render(dt, A, B, C, dimension, mode, loc, toRotate, direction, angle);
    const buffer = instance.exports.memory.buffer;
    const imageData = new ImageData(new Uint8ClampedArray(buffer, pixels, app.width * app.height * 4), app.width);
    ctx.putImageData(imageData, 0, 0);
}


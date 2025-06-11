let app = document.getElementById("app");
app.width = 800;
app.height = 600;

let ctx = app.getContext("2d");

let dt = 0;

let pitch = 0;
let roll = 0;
let yaw = 0;

const scale = 50;

// uint32_t* render(int dt, float a, float b, float c) {
function render(instance, dt, pitch, roll, yaw) {
    const pixels = instance.exports.render(dt, pitch, roll, yaw);
    const buffer = instance.exports.memory.buffer;
    const imageData = new ImageData(new Uint8ClampedArray(buffer, pixels, app.width * app.height * 4), app.width);
    ctx.putImageData(imageData, 0, 0);
}

async function main() {
    const response = await fetch('abcd.wasm');
    const bytes = await response.arrayBuffer();
    const {instance} = await WebAssembly.instantiate(bytes);

    setInterval(() => {
        dt += 1; // Increment dt by 1 every second

        // just make some changes, these are mostly random
        pitch -= Math.pow(Math.sin(dt), 2) / scale
        roll -= Math.pow(Math.cos(dt), 2) / scale
        yaw += 0.5 / scale

        render(instance, dt, pitch, roll, yaw)
    }, 25); // Interval set to 1000ms (1 second)
}

main();


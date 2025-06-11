let app = document.getElementById("app");
app.width = 800;
app.height = 600;
let ctx = app.getContext("2d");

var dt = 0;

main();


async function main() {
    const response = await fetch('abcd.wasm');
    const bytes = await response.arrayBuffer();
    const {instance} = await WebAssembly.instantiate(bytes);
    // instance.exports.resetFaces(1);
    // render(instance, dt, A, B, C, dimension, mode, loc);

    setInterval(() => {
        dt += 1; // Increment dt by 1 every second
        render(instance, dt);
    }, 25); // Interval set to 1000ms (1 second)
}

function render(instance, dt) {
    const pixels = instance.exports.render(dt);
    const buffer = instance.exports.memory.buffer;
    const imageData = new ImageData(new Uint8ClampedArray(buffer, pixels, app.width * app.height * 4), app.width);
    ctx.putImageData(imageData, 0, 0);
}


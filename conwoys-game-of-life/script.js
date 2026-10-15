const canvas = document.getElementById("canvas");
const ctx = canvas.getContext("2d");

let WIDTH = window.innerWidth;
let HEIGHT = window.innerHeight;

canvas.height = HEIGHT;
canvas.width = WIDTH;

const SPEED = 100;
const SIZE = 50;
const dx = [0, 0, 1, -1, 1, -1, 1, -1];
const dy = [1, -1, 0, 0, 1, -1, -1, 1];


let n, m;
let cells = [];

function resetGrid() {
    cells = Array.from(
        { length: n },
        () => Array(m).fill(0)
    );
}

function resizeCanvas() {
    WIDTH = window.innerWidth;
    HEIGHT = window.innerHeight;

    canvas.width = WIDTH;
    canvas.height = HEIGHT;

    m = Math.ceil(WIDTH / SIZE);
    n = Math.ceil(HEIGHT / SIZE);
}
resizeCanvas();
resetGrid()

window.addEventListener("resize", () => {
    resizeCanvas();
    resetGrid();
    initialConfig();
    render();
});


function initialConfig() {
    const row = 20;
    const col = 20;

    cells[row][col] = 1;
    cells[row + 1][col] = 1;
    cells[row + 1][col - 1] = 1;
    cells[row + 1][col - 2] = 1;
    cells[row + 2][col - 2] = 1;
    cells[row + 2][col - 3] = 1;
    cells[row + 3][col - 3] = 1;
}

function neighbours(x, y) {
    let live = 0;
    for (let k = 0; k < 8; k += 1) {
        let nx = dx[k] + x;
        let ny = dy[k] + y;
        if (nx < 0 || ny < 0 || nx >= n || ny >= m) {
            continue;
        }
        live += cells[nx][ny];
    }
    return live
}

/**
    Any live cell with fewer than two live neighbours dies, as if by underpopulation.
    Any live cell with two or three live neighbours lives on to the next generation.
    Any live cell with more than three live neighbours dies, as if by overpopulation.
    Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
* 
*/
function update() {
    const nextCells = Array.from(
        { length: n},
        () => Array(m).fill(0)
    );

    for (let i = 0; i < n; i += 1) {
        for (let j = 0; j < m; j += 1) {
            const live = neighbours(i, j);
            if (cells[i][j] === 1) {
                nextCells[i][j] = live === 2 || live === 3 ? 1: 0
            } else {
                nextCells[i][j] = live === 3 ? 1: 0
            }
        }
    }

    cells = nextCells;
}

function render() {
    ctx.fillStyle = "black";
    ctx.fillRect(0, 0, WIDTH, HEIGHT);

    ctx.fillStyle = "white"

    for (let i = 0; i < n; i++) {
        for (let j = 0; j < n; j++) {
            if (cells[i][j] === 1) {
                ctx.fillRect(
                    i * SIZE,
                    j * SIZE,
                    SIZE,
                    SIZE
                );
            }
        }
    } 

    ctx.fillStyle = "#444"
    for (let x = 0; x <= WIDTH; x += SIZE) {
        ctx.fillRect(x, 0, 1, HEIGHT);
    }

    for (let y = 0; y <= HEIGHT; y += SIZE) {
        ctx.fillRect(0, y, WIDTH, 1);
    }

}

let lastUpdate = 0;
function gameLoop(timestamp) {
    if (timestamp - lastUpdate >= SPEED) {
        update()
        lastUpdate = timestamp
    }
    render();
    requestAnimationFrame(gameLoop);
}

initialConfig()
render();
requestAnimationFrame(gameLoop);
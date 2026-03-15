const WORD_LENGTH = 5;
const MAX_ATTEMPT = 6;
let wordSet = new Set();
let answer = "";
let gameOver = false;
let currentRow = 0;
let currentCol = 0;
let currentGuess = "";
const message = document.getElementById("message");
const board = document.getElementById("board");
const keys = document.querySelectorAll(".key");
async function loadWords() {
    // words.txt を HTTP で取得
    try {
        const res = await fetch("../words.txt");
        // レスポンスをテキストとして読み込む
        const text = await res.text();
        wordSet = new Set(text
            .split("\n")
            .map((w) => w.trim().toUpperCase())
            .filter((w) => w.length == WORD_LENGTH));
        if (wordSet.size === 0) {
            message.textContent = "単語リストが空です";
            message.classList.add("red");
            gameOver = true;
            return;
        }
        const arr = Array.from(wordSet);
        // Math.random() 0以上1未満の少数
        // Math.floor() 切り捨て整数化
        answer = arr[Math.floor(Math.random() * arr.length)];
    }
    catch (e) {
        message.textContent = "単語リストの読み込みに失敗しました";
        message.classList.add("red");
        gameOver = true;
    }
}
loadWords();
document.addEventListener("keydown", (e) => {
    if (gameOver)
        return;
    if (e.key === "Enter") {
        submitGuess();
    }
    else if (e.key === "Backspace") {
        deleteLetter();
    }
    else if (/^[a-zA-Z]$/.test(e.key)) {
        addLetter(e.key.toUpperCase());
    }
});
keys.forEach((key) => {
    key.addEventListener("click", () => {
        if (gameOver)
            return;
        const letter = key.textContent;
        if (letter == "Enter") {
            submitGuess();
        }
        else if (letter === "Backspace") {
            deleteLetter();
        }
        else {
            addLetter(letter);
        }
    });
});
function addLetter(letter) {
    if (currentCol >= WORD_LENGTH || currentRow >= MAX_ATTEMPT || gameOver)
        return;
    const cell = board.querySelectorAll(".row")[currentRow].querySelectorAll(".cell")[currentCol];
    cell.textContent = letter;
    currentGuess += letter;
    currentCol++;
}
function deleteLetter() {
    if (currentCol == 0)
        return;
    currentCol--;
    const cell = board.querySelectorAll(".row")[currentRow].querySelectorAll(".cell")[currentCol];
    cell.textContent = "";
    // 末尾1文字を除いた文字列を返す
    // 負の値は末尾からのインデックス
    currentGuess = currentGuess.slice(0, -1);
}
function submitGuess() {
    if (currentGuess.length != WORD_LENGTH || currentRow >= MAX_ATTEMPT || gameOver)
        return;
    if (!wordSet.has(currentGuess)) {
        message.textContent = "単語が見つかりません";
        return;
    }
    else {
        message.textContent = "";
    }
    const row = board.querySelectorAll(".row")[currentRow];
    const cells = row.querySelectorAll(".cell");
    const result = Array(WORD_LENGTH).fill("absent");
    const count = {};
    for (const letter of answer) {
        // その文字がまだ追加されていないなら 0 + 1 する
        count[letter] = (count[letter] ?? 0) + 1;
    }
    for (let i = 0; i < WORD_LENGTH; ++i) {
        if (currentGuess[i] === answer[i]) {
            result[i] = "correct";
            // letter === answer[i] の場合のみ実行されるので、count[currentGuess[i]!] は必ず存在します。 「?? 0」 は、型チェックを通すための記述
            count[currentGuess[i]] = (count[currentGuess[i]] ?? 0) - 1;
        }
    }
    for (let i = 0; i < WORD_LENGTH; ++i) {
        if (result[i] === "correct")
            continue;
        if ((count[currentGuess[i]] ?? 0) > 0) {
            result[i] = "present";
            count[currentGuess[i]] = (count[currentGuess[i]] ?? 0) - 1;
        }
    }
    cells.forEach((cell, i) => {
        cell.classList.add(result[i]);
    });
    // 使用済みのキーボードに色付け
    const priority = {
        "correct": 3,
        "present": 2,
        "absent": 1,
        "": 0,
    };
    // result.forEach((status, i) => {
    // 	const letter = currentGuess[i]!;
    // 	keys.forEach((key) => {
    // 		if (key.textContent === letter) {
    // 			// 前回の状態を取得
    // 			const classList: string[] = Array.from(key.classList);
    // 			const prevStatus = classList.find((c) => c in priority) ?? "";
    // 			// 今回のほうが優先度が高ければ、更新
    // 			if (priority[status]! > priority[prevStatus]!) {
    // 				key.classList.remove(prevStatus);
    // 				key.classList.add(status);
    // 			}
    // 		}
    // 	});
    // });
    currentRow++;
    currentCol = 0;
    currentGuess = "";
    if (result.every((s) => s === "correct")) {
        message.textContent = `Congratulations!! ${currentRow}回で正解しました!!`;
        message.classList.add("green");
        gameOver = true;
    }
    else if (currentRow === MAX_ATTEMPT) {
        message.textContent = `残念!! 正解は ${answer} でした!`;
        message.classList.add("red");
        gameOver = true;
    }
}
export {};
//# sourceMappingURL=main.js.map
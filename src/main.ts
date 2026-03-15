const WORD_LENGTH = 5;
const MAX_ATTEMPT = 6;

let wordSet: Set<string> = new Set();
let answer = "";

let gameOver = false;
let currentRow = 0;
let currentCol = 0;
let guess = "";

const message = document.getElementById("message")!;
const board = document.getElementById("board")!;
const keys = document.querySelectorAll(".key");

init();

async function init(): Promise<void> {
	// DOMチェック
	if (!document.getElementById("message") || !document.getElementById("board")) {
		console.error("必要なDOM要素が見つかりません");
		return;
	}

	//　単語セットの読み込みを待つ
	await loadWords();
	
	if (gameOver)
		return;

	// 物理キーボードからの入力
	document.addEventListener("keydown", (e: KeyboardEvent) => {
		if (gameOver)
			return;

		if (e.key === "Enter") {
			submitGuess();
		} else if (e.key === "Backspace") {
			deleteLetter();
		} else if (/^[a-zA-Z]$/.test(e.key)) {
			addLetter(e.key.toUpperCase());
		} 
	});

	// UI上のキーボードから入力
	keys.forEach((key) => {
		key.addEventListener("click", () => {
			if (gameOver)
				return;
			const letter: string = key.textContent!;
			if (letter === "Enter") {
				submitGuess();
			} else if (letter === "Backspace") {
				deleteLetter();
			} else {
				addLetter(letter);
			}
		})
	})
}

async function loadWords(): Promise<void> {
	try {
		// words.txt を HTTP で取得
		const res = await fetch("../words.txt");
		if (!res.ok)
			throw new Error(`HTTP error: ${res.status}`);
		// レスポンスをテキストとして読み込む
		const text = await res.text();

		// 正規表現（大文字のみで5文字）
		const wordPattern = new RegExp(`^[A-Z]{${WORD_LENGTH}}$`);
		// 改行毎にバラして、前後の空白をトリム＆大文字にして、正規表現で確認
		wordSet = new Set(
			text.split("\n")
				.map((w) => w.trim().toUpperCase())
				.filter((w) => wordPattern.test(w))
		)

		if (wordSet.size === 0) {
			message.textContent = "単語リストが空です";
			message.classList.add("red");
			gameOver = true;
			return;
		}
		// ランダムアクセスのため配列を作成
		const arr = Array.from(wordSet);
		// Math.random() 0以上1未満の少数
		// Math.floor() 切り捨て整数化
		answer = arr[Math.floor(Math.random() * arr.length)]!;
	} catch (e) {
		message.textContent = "単語リストの読み込みに失敗しました";
		message.classList.add("red");
		gameOver = true;
	}
}

function addLetter(letter: string): void {
	if (!letter || currentCol >= WORD_LENGTH || gameOver)
		return;

	// セルを指定して、文字を入れる
	const cell = board.querySelectorAll(".row")[currentRow]!.querySelectorAll(".cell")[currentCol];
	cell!.textContent = letter;

	// 文字を追加
	guess += letter;
	// 次のマスへ
	currentCol++;

	// メッセージリセット
	message.textContent = "";
}

function deleteLetter(): void {
	if (currentCol == 0 || gameOver)
		return;
	// 1つ前のセルに戻る
	currentCol--;

	// セルを指定して、中身を空にする
	const cell = board.querySelectorAll(".row")[currentRow]!.querySelectorAll(".cell")[currentCol];
	cell!.textContent = "";

	// 末尾1文字を除いた文字列を返す
	// 負の値は末尾からのインデックス
	guess = guess.slice(0, -1);

	// メッセージリセット
	message.textContent = "";
}

function submitGuess(): void {
	if (guess.length != WORD_LENGTH || gameOver)
		return;

	// 単語セットに含まれるかチェック
	if (!wordSet.has(guess)) {
		message.textContent = "単語が見つかりません";
		return;
	}

	// 現在の行のセル（マス）をすべて取得
	const cells = board.querySelectorAll(".row")[currentRow]!.querySelectorAll("cells");

	// 状態　correct(緑), present(黃), absent(灰)
	type TileStatus = "correct" | "present" | "absent"; 
	// 
	const result: TileStatus[] = Array(WORD_LENGTH).fill("absent");

	// Recode(辞書)に答えに含まれるその文字の個数をもたせる
	const count: Record<string, number> = {};
	for (const letter of answer) {
		// その文字がまだ追加されていないなら 0 + 1 する
		count[letter] = (count[letter] ?? 0) + 1;
	}

	// correct(緑)の判定
	for (let i = 0; i < WORD_LENGTH; ++i)
	{
		if (guess[i] === answer[i])
		{
			result[i] = "correct";
			// letter === answer[i] の場合のみ実行されるので、count[guess[i]!] は必ず存在します。 「?? 0」 は、型チェックを通すための記述
			count[guess[i]!] = (count[guess[i]!] ?? 0) - 1
		}
	}

	// present(黃)の判定
	for (let i = 0; i < WORD_LENGTH; ++i)
	{
		if (result[i] === "correct")
			continue;
		if ((count[guess[i]!] ?? 0) > 0)
		{
			result[i] = "present";
			count[guess[i]!] = (count[guess[i]!] ?? 0) - 1;
		}
	}

	// 各セルに色付け
	cells.forEach((cell, i) => {
		cell.classList.add(result[i]!);
	})


	// 使用済みのキーボードに色付け
	const priority: Record<string, number> = {
		"correct": 3,
		"present": 2,
		"absent": 1,
		"": 0,
	}

	result.forEach((status, i) => {
		const letter = guess[i]!;
		keys.forEach((key) => {
			if (key.textContent === letter) {
				// 前回の状態を取得
				const classList: string[] = Array.from(key.classList);
				// hasOwnProperty() は Object インスタンスのメソッドで、オブジェクト自身が（継承されていない）指定されたプロパティを持っているかどうかを示す論理値を返します。
				const prevStatus = classList.find((c) => priority.hasOwnProperty(c)) ?? "";

				// 今回のほうが優先度が高ければ、更新
				if (priority[status]! > priority[prevStatus]!) {
					if (prevStatus !== "")
						key.classList.remove(prevStatus);
					key.classList.add(status);
				}
			}
		});
	});

	// 状態更新
	currentRow++;
	currentCol = 0;
	guess = "";

	// ゲームリザルト
	if (result.every((s) => s === "correct")) {
		message.textContent = `Congratulations!! ${currentRow}回で正解しました!!`
		message.classList.add("green");
		gameOver = true;
	} else if (currentRow === MAX_ATTEMPT) {
		message.textContent = `残念!! 正解は ${answer} でした!`
		message.classList.add("red");
		gameOver = true;
	}
}



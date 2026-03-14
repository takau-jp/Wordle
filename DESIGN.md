# Wordle CLI 設計

## 1. 設計方針

実装は以下を優先する。

- ルール判定と表示を分離する
- 辞書読み込みとゲーム進行を分離する
- テストしやすいように正解語選択を差し替え可能にする
- ターミナル依存の色表示を表示層に閉じ込める

## 2. ディレクトリ構成

```text
.
|-- Makefile
|-- include/
|   |-- Color.hpp
|   |-- Dictionary.hpp
|   |-- Evaluator.hpp
|   |-- Game.hpp
|   |-- TerminalView.hpp
|   `-- Types.hpp
|-- src/
|   |-- Dictionary.cpp
|   |-- Evaluator.cpp
|   |-- Game.cpp
|   |-- TerminalView.cpp
|   `-- main.cpp
|-- data/
|   `-- words.txt
|-- REQUIREMENTS.md
`-- DESIGN.md
```

## 3. モジュール責務

### `Types`

ゲーム全体で共有する基本型を持つ。

- `LetterState`: `gray`, `yellow`, `green`
- `GuessResult`: 1 回の推測文字列と各文字評価
- `GameConfig`: 辞書パス、試行回数、テスト用固定語の利用有無

### `Dictionary`

辞書の読み込みと単語検証を担当する。

- `data/words.txt` のロード
- 5 文字英小文字への正規化
- 入力語が辞書に存在するかの判定
- ランダムな正解語の選択
- テスト用既定語 `apple` の返却

内部では以下を持つ。

- 表示順を保つための `std::vector<std::string>`
- 高速検索のための `std::unordered_set<std::string>`

### `Evaluator`

Wordle の判定ルールだけを担当する。

- 完全一致を先に `green` 判定
- 残数管理を使って `yellow` 判定
- それ以外を `gray` 判定

このモジュールは入出力を持たず、純粋関数に近い形にする。

### `TerminalView`

ターミナル表示を担当する。

- ゲーム開始メッセージ
- 入力プロンプト
- エラーメッセージ
- 試行履歴のボード表示
- 勝利 / 敗北メッセージ

ANSI エスケープシーケンスはここだけで使う。

### `Game`

ゲーム進行の制御を担当する。

- 正解語の決定
- 入力受付
- バリデーションの実行
- 判定結果の蓄積
- 勝敗判定

`Dictionary`、`Evaluator`、`TerminalView` を束ねるオーケストレータとする。

## 4. 実行フロー

1. `main.cpp` で設定を作る
2. `Dictionary` が辞書を読む
3. `Game` が正解語を決める
4. 6 ターン以内で入力と判定を繰り返す
5. 各ターンで `TerminalView` が履歴を描画する
6. 勝利または敗北で終了する

## 5. インターフェース案

### `Dictionary`

```cpp
class Dictionary {
public:
    explicit Dictionary(const std::string& path);
    void load();
    bool contains(const std::string& word) const;
    std::string chooseRandomWord() const;
    std::string sampleWord() const;
};
```

### `Evaluator`

```cpp
class Evaluator {
public:
    GuessResult evaluate(const std::string& guess, const std::string& answer) const;
};
```

### `TerminalView`

```cpp
class TerminalView {
public:
    void printWelcome() const;
    void printBoard(const std::vector<GuessResult>& history) const;
    void printError(const std::string& message) const;
    void printWin(int attempts) const;
    void printLose(const std::string& answer) const;
};
```

### `Game`

```cpp
class Game {
public:
    Game(Dictionary& dictionary, TerminalView& view, GameConfig config);
    int run();
};
```

## 6. 入力バリデーション

入力文字列は以下の順で検証する。

1. 空入力でないこと
2. 長さが 5 であること
3. 英字のみで構成されること
4. 小文字へ正規化できること
5. 辞書に存在すること

失敗時は `Game` が理由文字列を決めて `TerminalView` に表示する。

## 7. テスト観点

実装後は最低限以下を確認する。

- `apple` を正解語にした固定実行で勝敗が想定通りになる
- 重複文字ありの判定が正しい
- 5 文字以外を拒否できる
- 辞書外単語を拒否できる
- ANSI 色表示付きで盤面が崩れない

## 8. 今回作る初期スケルトン

今回は以下まで作る。

- `Makefile`
- 共有型ヘッダ
- 各モジュールのヘッダ
- 空実装ではなく、コンパイル可能な最小スケルトン

ゲーム本体ロジックは次の段階で埋める。

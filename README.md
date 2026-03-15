# Wordle (TypeScript + HTML + CSS)

ブラウザで動くWordleゲームの実装ハンズオン。

---

## Step 1: 環境構築

### TypeScript のインストール

Homebrew の場合:

```bash
brew install typescript       # TypeScript をインストール
export PATH=~/.linuxbrew/bin:$PATH  # shell にパスを通す
node -v                       # Node.js のバージョン確認
tsc -v                        # TypeScript コンパイラのバージョン確認
```

### npm プロジェクトの初期化

SSL 証明書エラーが出る環境では、先に SSL 検証を無効化する:

```bash
npm config set strict-ssl false   # 42PC内で証明書エラーが出る場合の回避策
```

プロジェクトを初期化する:

```bash
npm init -y                        # package.json を自動生成（質問をスキップ）
npm install --save-dev typescript  # TypeScript を開発依存としてインストール
npx tsc --init                     # tsconfig.json（TypeScript の設定ファイル）を生成
```

インストール後は念のため元に戻す:

```bash
npm config set strict-ssl true
```

これで以下のファイルが生成される:

- `package.json` — プロジェクト設定・依存パッケージの管理
- `tsconfig.json` — TypeScript コンパイラの設定（対象ブラウザ、出力先など）

---

## Step 2: ファイル構成を作る

```bash
touch src/index.html src/style.css src/main.ts
```

```text
src/
├── index.html   # ゲームの画面構造
├── style.css    # デザイン
└── main.ts      # ゲームロジック
```

---

## Step 3: `tsconfig.json` の設定

`tsconfig.json` を開き、以下の項目を設定する:

```json
{
  "compilerOptions": {
    "target": "ES2020",   // 出力する JS のバージョン（ESNext は草案含む最新、ES2020 は安定版）
    "outDir": "./dist",   // コンパイル後の JS の出力先
    "rootDir": "./src",   // TypeScript ソースの場所
    "strict": true        // 型チェックを厳格にする
  }
}
```

### `target` の選択肢

| 値 | 安定性 | ブラウザ互換 | 使いどころ |
| --- | --- | --- | --- |
| `ES2020` | 高い（仕様固定） | 主要ブラウザ全対応 | 本番向け |
| `ESNext` | 低い（草案含む） | 最新ブラウザのみ | 最新機能を試したい時 |

### `module` の設定

```json
"module": "ES2020"
```

`module` はコンパイル後の JS でどの**モジュール形式**を使うかの設定。

`index.html` で `type="module"` を使う場合はセットで必要:

```html
<!-- type="module" をつけることでモジュールスコープが有効になる -->
<!-- トップレベル変数が window に乗らず、開発者ツールから触れなくなる -->
<script type="module" src="../dist/main.js"></script>
```

| 設定なし（デフォルト） | `type="module"` あり |
| --- | --- |
| 変数がグローバル（`window`）に乗る | 変数がモジュールスコープに閉じられる |
| 開発者ツールから書き換え可能 | 外部からアクセス不可 |

---

## Step ４: コンパイルの設定

`package.json` の `scripts` に以下を追加する:

```json
"scripts": {
  "build": "tsc",         // 一回だけコンパイル
  "watch": "tsc --watch"  // ファイル変更を検知して自動コンパイル
}
```

開発中は別ターミナルで watch を起動しておく:

```bash
npm run watch  # main.ts を保存するたびに自動で dist/main.js が更新される
```

---

## Step 5: ブラウザで確認する

`type="module"` は `file://` で直接開くと CORS エラーで動かないため、ローカルサーバーが必要。

**プロジェクトルートから**サーバーを起動する:

```bash
python3 -m http.server 8080
```

ブラウザで以下にアクセス:

```text
http://localhost:8080/src/
```

> `http://localhost:8080/` だとディレクトリ一覧が表示されるだけ。`/src/` まで含めること。


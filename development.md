# zmk-trackball-threshold モジュール 最終要件定義

## 1. プロジェクト概要

- **モジュール名**: `zmk-trackball-threshold`
- **目的**: ZMK 標準 input processor による AML 制御において、トラックボール移動量の閾値を設定し、軽微な振動による誤動作を防止
- **開発方針**: zmk-mouse-gesture の実装パターンを参考とした独立モジュール

## 2. 機能仕様

### 2.1 Input Processor 仕様

- **Compatible**: `"zmk,input-processor-trackball-threshold"`
- **Cells**: `#input-processor-cells = <0>`
- **閾値計算方式**: `abs(x) + abs(y) > threshold`
- **動作**: 閾値以下の入力は破棄、閾値超過の入力は無変更で転送

### 2.2 設定方式

**Device Tree 設定（.keymap ファイル）**:

```c
zip_trackball_threshold: zip_trackball_threshold {
    compatible = "zmk,input-processor-trackball-threshold";
    movement-threshold = <5>; // 省略時はKconfigデフォルト値
};

&trackball_listener {
    input-processors = <&zip_trackball_threshold>, <&zip_temp_layer AML 30000>;
};
```

**Kconfig 設定（.conf ファイル）**:

```c
CONFIG_ZMK_INPUT_PROCESSOR_TRACKBALL_THRESHOLD=y
CONFIG_ZMK_INPUT_PROCESSOR_TRACKBALL_THRESHOLD_DEFAULT_THRESHOLD=5
```

## 3. 技術仕様

### 3.1 モジュール構造

```
zmk-trackball-threshold/
├── CMakeLists.txt
├── Kconfig
├── README.md
├── development.md
├── dts/
│   └── bindings/
│       └── zmk,input-processor-trackball-threshold.yaml
├── include/
│   └── zmk/
│       └── trackball_threshold.h
├── src/
│   └── trackball_threshold.c
└── zephyr/
    └── module.yml
```

### 3.2 パラメータ仕様

- **デフォルト閾値**: `5`
- **設定可能範囲**: `0`以上の整数値
- **適用範囲**: `trackball_listener`のみ

## 4. 実装要件

### 4.1 必須機能

- トラックボール入力の移動量計算
- 閾値との比較判定
- 閾値以下の入力破棄
- 閾値超過の入力透過転送
- Device Tree プロパティからの設定読み込み

### 4.2 非機能要件

- **パフォーマンス**: 入力レスポンスに影響しない軽量実装
- **互換性**: 既存 input processor chain との完全互換
- **保守性**: zmk-mouse-gesture と同様の構造

## 5. 開発制約

- 既存の zmk-config-roBa には変更を加えない
- 既存の zmk-pmw3610-driver には変更を加えない
- west.yml 設定は後段階で実施
- 独立したモジュールとして完成させる

## 6. 開発ステップ

1. モジュール基本構造の作成
2. Device Tree Binding の定義
3. Kconfig 設定の実装
4. Input Processor コアロジックの実装
5. テスト用設定の作成
6. 動作確認とデバッグ

---

**要件定義確定完了**

この要件定義に基づいて、zmk-trackball-threshold モジュールの実装を進める。
